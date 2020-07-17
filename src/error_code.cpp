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

#include <cstring>
#include <fstream>
#include <iostream>
#include <string>
#include <string_view>

#include "header_str.hpp"
#include "parse_xml.hpp"

constexpr std::string_view helpStr = R"HELP(
Generates header files for C++. Contains `operator==` and `operator!=`
functions for a large set of the available Vulkan struct types, checking
just the objects for equality. These do *NOT* peroform a deep comparison,
such as any objects pointed to by `pNext` or any other pointed-to objects.

Program Arguments:
    -h, --help  : Help Blurb
    -i, --input : Input vk.xml file to parse. These can be found from the 
                    KhronosGroup, often at this repo:
                    https://github.com/KhronosGroup/Vulkan-Docs
    -d, --dir   : Output directory
    -o, --out   : Output file name (Default: `vk_equality_checks.hpp`)
)HELP";

constexpr std::string_view usageStr = R"USAGE(
/*  USAGE
    To use, include this header where the declarations for the boolean checks are required.

    On *ONE* compilation unit, include the definition of `#define VK_ERROR_CODE_CONFIG_MAIN`
    so that the definitions are compiled somewhere following the one definition rule.
*/
)USAGE";

int main(int argc, char **argv) {
    std::string inputFile;
    std::string outputDir;
    std::string outputFile = "vk_error_code.hpp";

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
        std::cerr << "Error: No input file given. Type --help for help." << std::endl;
        return 1;
    }

    // In order to parse a file, we need to load the whole thing into memory.
    std::ifstream inFile(inputFile.c_str(), std::ifstream::in);
    if (!inFile.is_open()) {
        std::cerr << "Error: Failed to open file " << inputFile << std::endl;
        return 1;
    }
    // Seek to the end.
    inFile.seekg(0, std::ifstream::end);
    // Tell us how many chars to set aside.
    std::size_t fileSize = inFile.tellg();

    // Allocate a large enough block, and read it all into memory
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

    // Need to be in the 'registry' node
    auto enums = getEnumData(registryNode);

    // Reduce to just the 'VkResult' enum that we're interested in
    for (auto &it : enums) {
        if (it.name == "VkResult") {
            enums = {it};
            break;
        }
    }

    // Extensions for type platforms
    auto *extensionsNode = registryNode->first_node("extensions");
    if (extensionsNode == nullptr) {
        std::cerr << "Error: Could not find the 'extensions' node." << std::endl;
        return 1;
    }

    getEnumExtensions(enums, extensionsNode);

    { // Header File
        std::ofstream outFile(outputDir + outputFile);
        if (!outFile.is_open()) {
            std::cerr << "Error: Failed to open output file for writing: " << outputDir
                      << outputFile << std::endl;
            return 1;
        }

        outFile << headerStr;

        outFile << "#ifndef VK_ERROR_CODE_V" << vkHeaderVersion << "_HPP\n";
        outFile << "#define VK_ERROR_CODE_V" << vkHeaderVersion << "_HPP\n";

        outFile << "\n#include <vulkan/vulkan.h>\n";

        outFile << "\n";
        outFile << "#include <system_error>\n";

        outFile << usageStr;
        outFile << "\n";

        // Static assert checking correct/compatible header version
        outFile << "static_assert(VK_HEADER_VERSION == " << vkHeaderVersion
                << ", \"Incompatible VK_HEADER_VERSION!\");\n";
        outFile << "\n";

        // Declarations
        outFile << R"DECL(namespace std {
template <>
struct is_error_code_enum<VkResult> : true_type {};
} // namespace std

std::error_code make_error_code(VkResult);
)DECL";

        // Definitions
        outFile << "\n#ifdef VK_ERROR_CODE_CONFIG_MAIN\n";

        outFile << R"DEFSTART(
namespace {

struct VulkanErrCategory : std::error_category {
    const char *name() const noexcept override;
    std::string message(int ev) const override;
};

const char *VulkanErrCategory::name() const noexcept {
    return "VkResult";
}

std::string VulkanErrCategory::message(int ev) const {
    switch (static_cast<VkResult>(ev)) {
)DEFSTART";

        for (auto &it : enums[0].values) {
            outFile << "    case " << it.name << ":\n";
            outFile << "        return \"" << it.name << "\";\n";
        }

        outFile << R"DEFEND(
    default:
        if (ev > 0)
            return "(unrecognized positive VkResult value)";
        else
            return "(unrecognized negative VkResult value)";
    }
}

const VulkanErrCategory vulkanErrCategory{};

} // namespace

std::error_code make_error_code(VkResult e) {
    return {static_cast<int>(e), vulkanErrCategory};
}
)DEFEND";

        outFile << "\n#endif // VK_ERROR_CODE_CONFIG_MAIN\n";

        outFile << "#endif // VK_ERROR_CODE_V" << vkHeaderVersion << "_HPP\n";
    }

    return 0;
}