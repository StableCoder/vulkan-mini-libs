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

#include "stringifier_strings.h"

/**
 * @brief Removes a vendor tag from the end of the given string view
 * @param vendorTags List of vendor tags to check against
 * @param view String view to remove the vendor tag from
 * @return A string_view without the vendor tag, if it was suffixed
 */
std::string_view removeVendorTag(const std::vector<std::string> &vendorTags,
                                 std::string_view view) {
    for (auto &it : vendorTags) {
        if (strncmp(view.data() + view.size() - it.size(), it.data(), it.size()) == 0) {
            view = view.substr(0, view.size() - it.size());
            break;
        }
    }

    return view;
}

/**
 * @brief Converts a Vulakn Flag typename into the prefix that is used for it's enums
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

int main(int argc, char **argv) {
    std::string inputFile;
    std::string outputDir;
    std::string enumOutputFile = "vk_enum_stringifier.cpp";

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
                enumOutputFile = argv[i + 1];
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

    std::vector<std::string> vendorTagList;
    std::stringstream vendorTags;
    { // Vendor Tags
        vendorTags << "constexpr const char* vendorTags[] = {\n";

        std::size_t tagCount = 0;

        // Process 'tags' node
        auto *tagsNode = registryNode->first_node("tags");
        auto const *endTagNode = tagsNode->last_node("tag");
        for (auto tagNode = tagsNode->first_node("tag"); true; tagNode = tagNode->next_sibling()) {
            ++tagCount;
            auto *name = tagNode->first_attribute("name");

            vendorTags << "    \"" << name->value() << "\",\n";
            vendorTagList.emplace_back(name->value());

            if (tagNode == endTagNode)
                break;
        }

        vendorTags << "};\n";
        vendorTags << "constexpr std::size_t vendorTagCount = " << tagCount << ";\n";
    }

    std::stringstream enumDecl;
    std::stringstream valueSets;
    std::stringstream valueSetArr;

    // Starts of each stream
    enumDecl << R"DECL(
struct EnumDeclaration {
    const char *name;
    std::size_t count;
};
EnumDeclaration enumTypes[] = {
)DECL";

    valueSets << R"SETS(
struct EnumValueSet {
    const char *name;
    uint32_t value;
};
)SETS";

    valueSetArr << R"ARR(
constexpr const EnumValueSet *valueSets[] = {
)ARR";

    std::size_t enumsCount = 0;

    // Process 'enums' nodes
    auto const *endEnumsNode = registryNode->last_node("enums");
    for (auto *enumsNode = registryNode->first_node("enums"); true;
         enumsNode = enumsNode->next_sibling()) {
        auto *nameAttr = enumsNode->first_attribute("name");
        if (nameAttr == nullptr || strcmp("API Constants", nameAttr->value()) == 0 ||
            strcmp("VkResult", nameAttr->value()) == 0)
            continue;

        ++enumsCount;
        auto *typeAttr = enumsNode->first_attribute("type")->value();

        // Generate prefix
        auto nonVendorName = removeVendorTag(vendorTagList, nameAttr->value());
        std::string prefix = processEnumPrefix(vendorTagList, nonVendorName);

        std::size_t enumCount = 0;
        if (enumsNode->first_node() != nullptr) {
            // Process each 'enum' within
            std::vector<std::pair<std::string, std::string>> enums;

            auto const *endEnum = enumsNode->last_node("enum");
            for (auto *enumNode = enumsNode->first_node("enum"); true;
                 enumNode = enumNode->next_sibling()) {
                if (strcmp(enumNode->name(), "enum") != 0)
                    continue;

                ++enumCount;

                if (enumCount == 1) {
                    valueSets << "constexpr EnumValueSet " << nameAttr->value() << "Sets[] = {\n";
                }

                std::string_view name = enumNode->first_attribute("name")->value();
                name = removeVendorTag(vendorTagList, name);
                name = trimNonAlNum(name);
                name = stripBit(name);

                if (const auto *value = enumNode->first_attribute("value"); value != nullptr) {
                    // Plain value
                    enums.emplace_back(std::pair{name, value->value()});
                } else if (const auto *bitpos = enumNode->first_attribute("bitpos");
                           bitpos != nullptr) {
                    // Bitpos value
                    std::stringstream ss;
                    ss << "0x" << std::hex << std::setw(8) << std::uppercase << std::setfill('0')
                       << (1U << atoi(bitpos->value()));
                    enums.emplace_back(std::pair{name, ss.str()});
                } else if (const auto *alias = enumNode->first_attribute("alias");
                           alias != nullptr) {
                    // Alias
                    bool found = false;
                    for (auto &it : enums) {
                        std::string_view aliasView = alias->value();
                        aliasView = removeVendorTag(vendorTagList, aliasView);
                        aliasView = trimNonAlNum(aliasView);
                        if (std::get<0>(it) == aliasView) {
                            enums.emplace_back(name, std::get<1>(it));
                            found = true;
                            break;
                        }
                    }
                    if (!found) {
                        std::cerr << "Error: Couldn't find alias for " << name << std::endl;
                        return 1;
                    }
                } else {
                    std::cerr << "Error: Enum type unsupported: " << typeAttr << std::endl;
                    return 1;
                }

                if (endEnum == enumNode)
                    break;
            }

            for (auto &it : enums) {
                if (strncmp(std::get<0>(it).data(), prefix.data(), prefix.size()) == 0) {
                    valueSets << "    {\"" << std::get<0>(it).substr(prefix.size()) << "\", ";
                } else {
                    valueSets << "    {\"" << std::get<0>(it) << "\", ";
                }
                valueSets << std::get<1>(it) << "},\n";
            }
        }

        // Finish up the end of the enum for the streams
        enumDecl << "    {\"" << nameAttr->value() << "\", " << enumCount << "},\n";
        if (enumCount != 0) {
            valueSets << "};\n";
            valueSetArr << "    " << nameAttr->value() << "Sets,\n";
        } else {
            valueSetArr << "    nullptr,\n";
        }

        if (enumsNode == endEnumsNode)
            break;
    }

    valueSetArr << "};\n";
    enumDecl << "};\n";
    enumDecl << "constexpr std::size_t enumTypesCount = " << enumsCount << ";\n";

    { // Source File
        if (!outputDir.empty() && outputDir[outputDir.size() - 1] != '/' &&
            outputDir[outputDir.size() - 1] != '\\') {
            outputDir += '/';
        }

        std::ofstream outFile(outputDir + enumOutputFile);
        if (!outFile.is_open()) {
            std::cerr << "Error: Failed to open output file for writing: " << outputDir
                      << enumOutputFile << ".cpp" << std::endl;
            return 1;
        }

        // First, write the license/disclaimer
        outFile << descriptionStr;
        outFile << includesStr;

        outFile << "constexpr uint32_t generatedVulkanVersion = " << vkHeaderVersion << "u;\n\n";
        outFile << vendorTags.str();

        outFile << enumDecl.str();
        outFile << valueSets.str();
        outFile << valueSetArr.str();

        outFile << commonFunctionsStr;
    }

    return 0;
}