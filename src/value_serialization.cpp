/*
    Copyright (C) 2020 George Cave - gcave@stablecoder.ca

    Licensed under the Apache License, Version 2.0 (the "License");
    you may not use this file except in compliance with the License.
    You may obtain a copy of the License at

       http://www.apache.org/licenses/LICENSE-2.0

    Unless required by applicable law or agreed to in writing, software
    distributed under the License is distributed on an "AS IS" BASIS,
    WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
    See the License for the specific language governing permissions and
    limitations under the License.
*/

#include <rapidxml-1.13/rapidxml.hpp>

#include <algorithm>
#include <cstring>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

#include "header_str.hpp"
#include "parse_xml.hpp"
#include "serialization_strings.hpp"

/**
 * @brief Removes a vendor tag from the end of the given string view
 * @param vendorTags List of vendor tags to check against
 * @param view String view to remove the vendor tag from
 * @return A string_view without the vendor tag, if it was suffixed
 */
std::string_view removeVendorTag(const std::vector<std::string> &vendorTags,
                                 std::string_view view) {
    for (auto &it : vendorTags) {
        if (view == it)
            break;

        if (strncmp(view.data() + view.size() - it.size(), it.data(), it.size()) == 0) {
            view = view.substr(0, view.size() - it.size());
            break;
        }
    }

    return view;
}

/**
 * @brief Converts a Vulkan Flag typename into the prefix that is used for it's enums
 * @param typeName Name of the type to generate the Vk enum prefix for
 * @return Generated prefix string
 *
 * Any capitalized letters except for the first has an underscore inserted before it, an underscore
 * is added to the end, and all characters are converted to upper case.
 */
std::string processEnumPrefix(const std::vector<std::string> &vendorTags,
                              std::string_view typeName) {
    std::size_t size = strlen("FlagBits");
    if (typeName.size() > size) {
        if (strncmp(typeName.data() + typeName.size() - size, "FlagBits", size) == 0) {
            typeName = typeName.substr(0, typeName.size() - strlen("FlagBits"));
        }
    }

    std::string retStr;
    for (auto it = typeName.begin(); it != typeName.end(); ++it) {
        if (it == typeName.begin()) {
            retStr += ::toupper(*it);
        } else if (::isupper(*it)) {
            retStr += '_';
            retStr += *it;
        } else {
            retStr += toupper(*it);
        }
    }
    retStr += '_';

    return retStr;
}

/**
 * @brief Trims non alphanumeric characters from the string view
 * @param view Itme to trim
 * @return Trimmed string view
 */
std::string_view trimNonAlNum(std::string_view view) {
    if (view.empty())
        return view;

    // Trim left
    for (std::size_t i = 0; i < view.size(); ++i) {
        if (::isalnum(view[i])) {
            view = view.substr(i);
            break;
        }
    }

    if (view.empty())
        return view;

    // Trim right
    for (std::size_t i = view.size() - 1; i >= 0; --i) {
        if (::isalnum(view[i])) {
            view = view.substr(0, i + 1);
            break;
        }
    }

    return view;
}

/**
 * @brief Strips '_BIT' from the end of a string, if there
 */
std::string_view stripBit(std::string_view view) {
    if (view.size() > strlen("_BIT")) {
        if (view.substr(view.size() - strlen("_BIT")) == "_BIT") {
            return view.substr(0, view.size() - strlen("_BIT"));
        }
    }

    return view;
}

std::string replaceFlagBitsSuffix(std::string str) {
    if (str.size() > strlen("FlagBits")) {
        if (str.substr(str.size() - strlen("FlagBits")) == "FlagBits") {
            str = str.substr(0, str.size() - strlen("FlagBits"));
            str += "Flags";
        }
    }

    return str;
}

int main(int argc, char **argv) {
    std::string inputFile;
    std::string outputDir;
    std::string outputFile = "vk_value_serialization.hpp";

    for (int i = 0; i < argc; ++i) {
        if (strcmp(argv[i], "-h") == 0 || strcmp(argv[i], "--help") == 0) {
            std::cout << helpStr << std::endl;
            return 0;
        } else if (strcmp(argv[i], "-i") == 0 || strcmp(argv[i], "--input") == 0) {
            if (i + 1 <= argc) {
                inputFile = argv[i + 1];
            }
        } else if (strcmp(argv[i], "-d") == 0 || strcmp(argv[i], "--dir") == 0) {
            if (i + 1 <= argc) {
                outputDir = argv[i + 1];
            }
        } else if (strcmp(argv[i], "-o") == 0 || strcmp(argv[i], "--out") == 0) {
            if (i + 1 <= argc) {
                outputFile = argv[i + 1];
            }
        }
    }

    if (inputFile == "") {
        std::cout << "Error: No input file given. Type --help for help." << std::endl;
        return 1;
    }

    // In order to parse a file, we need to load the whole thing into memory.
    std::ifstream inFile(inputFile.c_str(), std::ifstream::in);
    if (!inFile.is_open()) {
        std::cout << "Error: Failed to open file " << inputFile << std::endl;
        return 1;
    }
    // Seek to the end.
    inFile.seekg(0, std::ifstream::end);
    // Tell us how many chars to set aside.
    std::size_t fileSize = inFile.tellg();

    char *xmlContent = new char[fileSize];

    inFile.seekg(0, std::ifstream::beg);
    inFile.read(xmlContent, fileSize);
    inFile.close();

    // XML Parsing
    rapidxml::xml_document<> vkDoc;
    vkDoc.parse<0>(xmlContent);

    // Baselevel node should be <registry>
    rapidxml::xml_node<> *registryNode = vkDoc.first_node("registry");
    if (registryNode == nullptr) {
        std::cerr << "Error: No <registry> top-level tag found. Invalid vk.xml document."
                  << std::endl;
        return 1;
    }

    // Types node
    // To retrieve the Vulkan version information
    int vkHeaderVersion = -1;
    {
        rapidxml::xml_node<> *typesNode = registryNode->first_node("types");

        rapidxml::xml_node<> *endTypeNode = typesNode->last_node("type");
        for (auto *typeNode = typesNode->first_node("type"); typeNode != endTypeNode;
             typeNode = typeNode->next_sibling()) {
            if (strcmp("// Version of this file\n#define ", typeNode->value()) == 0) {
                // Go through the contents, the third sibling will have the header version
                auto *node = typeNode->first_node()->next_sibling()->next_sibling();
                vkHeaderVersion = std::stoi(node->value());
                break;
            }
        }
    }
    if (vkHeaderVersion == -1) {
        std::cerr << "Error: Could not determine vk.xml header version." << std::endl;
        return 1;
    }

    // Vendors
    auto *tagsNode = registryNode->first_node("tags");
    if (tagsNode == nullptr) {
        std::cerr << "Error: Could not find the 'tags' node." << std::endl;
        return 1;
    }

    auto vendors = getVendorTags(tagsNode);

    // Platforms
    auto *platformsNode = registryNode->first_node("platforms");
    if (platformsNode == nullptr) {
        std::cerr << "Error: Could not find the 'platforms' node." << std::endl;
        return 1;
    }

    auto platforms = getPlatforms(platformsNode);

    // Need to be in the 'registry' node
    auto enums = getEnumData(registryNode);

    // Remove the 'VkResult' enum, we're not using it here
    for (auto it = enums.begin(); it != enums.end(); ++it) {
        if (it->name == "VkResult") {
            enums.erase(it);
            break;
        }
    }

    // Extensions for type platforms
    auto *extensionsNode = registryNode->first_node("extensions");
    if (extensionsNode == nullptr) {
        std::cerr << "Error: Could not find the 'extensions' node." << std::endl;
        return 1;
    }

    getEnumPlatforms(enums, extensionsNode);

    { // Header file
        std::ofstream outFile(outputDir + outputFile);
        if (!outFile.is_open()) {
            std::cerr << "Error: Failed to open output file for writing: " << outputDir
                      << outputFile << std::endl;
            return 1;
        }

        outFile << headerStr;

        outFile << "#ifndef VK_VALUE_SERIALIZATION_V" << vkHeaderVersion << "_HPP\n";
        outFile << "#define VK_VALUE_SERIALIZATION_V" << vkHeaderVersion << "_HPP\n";

        outFile << usageStr;

        outFile << "\n#include <vulkan/vulkan.h>\n";

        outFile << "\n";
        outFile << "#include <string>\n";
        outFile << "#include <string_view>\n";
        outFile << "\n";

        // Static assert checking correct/compatible header version
        outFile << "static_assert(VK_HEADER_VERSION == " << vkHeaderVersion
                << ", \"Incompatible VK_HEADER_VERSION!\");\n";

        // Declarations
        outFile << declarationStr;

        // Definitions
        outFile << "\n#ifdef VK_VALUE_SERIALIZATION_CONFIG_MAIN\n";

        outFile << "\n";
        outFile << "#include <algorithm>\n";
        outFile << "#include <array>\n";
        outFile << "#include <cstring>\n";
        outFile << "#include <tuple>\n";
        outFile << "\nnamespace {\n";

        // Vendors
        outFile << "\nconstexpr std::array<std::string_view, " << vendors.size()
                << "> vendors = {{\n";
        for (auto &it : vendors) {
            outFile << "  \"" << it << "\",\n";
        }
        outFile << "}};\n";

        // Enum value sets
        outFile << R"(
struct EnumValueSet {
    std::string_view name;
    uint32_t value;
};
)";

        for (auto const &it : enums) {
            if (it.values.empty()) {
                continue;
            }

            outFile << "constexpr EnumValueSet " << it.name << "Sets[] = {\n";
            std::string prefix = processEnumPrefix(vendors, removeVendorTag(vendors, it.name));
            for (auto const &val : it.values) {
                std::string_view name = val.name;

                // Strip prefix
                if (strncmp(name.data(), prefix.data(), prefix.size()) == 0)
                    name = name.substr(prefix.size());

                name = removeVendorTag(vendors, name);
                name = trimNonAlNum(name);
                name = stripBit(name);

                if (strncmp(name.data(), prefix.data(), prefix.size()) == 0) {
                    outFile << "    {\"" << name.substr(prefix.size());
                } else {
                    outFile << "    {\"" << name;
                }
                outFile << "\", " << val.value << "},\n";
            }
            outFile << "};\n";
        }

        // value set pointers
        outFile << R"(
struct EnumType {
    std::string_view name;
    EnumValueSet const* data;
    uint32_t count;
};
)";
        outFile << "\nconstexpr std::array<EnumType, " << enums.size() << "> enumTypes = {{\n";
        for (auto const &it : enums) {
            outFile << "  {\"" << it.name << "\", ";
            if (it.values.empty()) {
                outFile << "nullptr, ";
            } else {
                outFile << it.name << "Sets, ";
            }
            outFile << it.values.size() << "},\n";
        }
        outFile << "}};\n";

        // Functions
        outFile << stripFuncsStr;
        outFile << otherFuncsStr;

        outFile << stringifyFuncsStr;
        outFile << parseFuncsStr;

        outFile << "\n} // namespace\n";

        outFile << publicFuncsStr;

        outFile << "\n#endif // VK_VALUE_SERIALIZATION_CONFIG_MAIN\n";

        outFile << "#endif // VK_VALUE_SERIALIZATION_V" << vkHeaderVersion << "_HPP\n";
    }

    return 0;
}