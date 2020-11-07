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
#include <sstream>
#include <string>
#include <vector>

#include "header_str.hpp"
#include "parse_xml.hpp"

std::string_view headerUsageStr = R"USE(
/*  USAGE:
    To use, include this header where the declarations for the boolean checks are required.

    On *ONE* compilation unit, include the definition of `#define VK_EQUALITY_CHECK_CONFIG_MAIN`
    so that the definitions are compiled somewhere following the one definition rule.
*/
)USE";

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

struct Member {
    std::string type;
    std::string name;
};

int main(int argc, char **argv) {
    std::string inputFile;
    std::string outputDir;
    std::string outputFile = "vk_equality_checks.hpp";

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
    auto fileSize = inFile.tellg();

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

    // Platforms
    auto *platformsNode = registryNode->first_node("platforms");
    if (platformsNode == nullptr) {
        std::cerr << "Error: Could not find the 'platforms' node." << std::endl;
        return 1;
    }

    auto platforms = getPlatforms(platformsNode);

    // Need to be in the 'types' node
    auto *typesNode = registryNode->first_node("types");
    if (typesNode == nullptr) {
        std::cerr << "Error: Could not find the 'types' node." << std::endl;
        return 1;
    }

    auto structs = getStructData(typesNode);
    auto unions = getUnionData(typesNode);

    // Extensions for type platforms
    auto *extensionsNode = registryNode->first_node("extensions");
    if (extensionsNode == nullptr) {
        std::cerr << "Error: Could not find the 'extensions' node." << std::endl;
        return 1;
    }

    getStructPlatforms(structs, extensionsNode);

    // Output to final file
    std::ofstream outFile(outputDir + outputFile);
    if (!outFile.is_open()) {
        std::cerr << "Error: Failed to open output file for writing: " << outputDir << outputFile
                  << std::endl;
        return 1;
    }

    outFile << headerStr;

    outFile << "#ifndef VK_EQUALITY_CHECK_V" << vkHeaderVersion << "_HPP\n";
    outFile << "#define VK_EQUALITY_CHECK_V" << vkHeaderVersion << "_HPP\n";

    outFile << headerUsageStr;

    outFile << "\n#include <vulkan/vulkan.h>\n";
    outFile << "\n";

    // Static assert checking correct/compatible header version
    outFile << "static_assert(VK_HEADER_VERSION == " << vkHeaderVersion
            << ", \"Incompatible VK_HEADER_VERSION!\" );\n";

    // Declarations
    for (auto &it : structs) {
        // If no members to compare, then no point
        if (it.members.empty())
            continue;

        if (it.name == "VkDeviceCreateInfo" || it.name == "VkInstanceCreateInfo")
            continue;

        if (structHasUnion(it, unions))
            continue;

        std::string_view platformDefine;
        if (!it.platform.empty()) {
            // Find the platform
            for (auto &platform : platforms) {
                if (it.platform == platform.name) {
                    platformDefine = platform.define;
                    break;
                }
            }

            outFile << "\n#ifdef " << platformDefine;
        }

        outFile << "\nbool operator==(" << it.name << " const &lhs,\n";
        outFile << "                " << it.name << " const &rhs) noexcept;\n";
        outFile << "bool operator!=(" << it.name << " const &lhs,\n";
        outFile << "                " << it.name << " const &rhs) noexcept;\n";

        if (!platformDefine.empty())
            outFile << "#endif // " << platformDefine << "\n";
    }

    // Definitions
    outFile << "\n#ifdef VK_EQUALITY_CHECK_CONFIG_MAIN\n";
    outFile << "\n#include <cstdint>\n";
    outFile << "#include <cstring>\n";

    for (auto &it : structs) {
        // If no members to compare, then no point
        if (it.members.empty())
            continue;

        if (it.name == "VkDeviceCreateInfo" || it.name == "VkInstanceCreateInfo")
            continue;

        if (structHasUnion(it, unions))
            continue;

        std::string_view platformDefine;
        if (!it.platform.empty()) {
            // Find the platform
            for (auto &platform : platforms) {
                if (it.platform == platform.name) {
                    platformDefine = platform.define;
                    break;
                }
            }

            outFile << "\n#ifdef " << platformDefine;
        }

        // == definition
        outFile << "\nbool operator==(" << it.name << " const &lhs,\n";
        outFile << "                " << it.name << " const &rhs) noexcept {\n";
        // Len members
        for (auto const &member : it.members) {
            if (member.len.empty() || member.len == "null-terminated")
                continue;

            outFile << "  if(lhs." << member.len << " != rhs." << member.len << ")\n";
            outFile << "    return false;\n\n";
        }
        // Array members
        for (auto const &member : it.members) {
            if (!member.sizeEnum.empty()) {
                outFile << "  for(uint32_t i = 0; i < " << member.sizeEnum << "; ++i) {\n";
                outFile << "    if(lhs." << member.name << "[i] != rhs." << member.name << "[i])\n";
                outFile << "      return false;\n";
                outFile << "  }\n\n";
            } else if (!member.len.empty()) {
                if (member.len == "null-terminated") {
                    outFile << "  if (lhs." << member.name << " != rhs." << member.name << ") {\n";
                    outFile << "    if(lhs." << member.name << " == nullptr || rhs." << member.name
                            << " == nullptr)\n";
                    outFile << "      return false;\n";
                    outFile << "    if(strcmp(lhs." << member.name << ", rhs." << member.name
                            << ") != 0)\n";
                    outFile << "      return false;\n";
                    outFile << "  }\n\n";
                } else if (member.type == "void" && member.typeSuffix == "*") {
                    outFile << "  if(memcmp(lhs." << member.name << ", rhs." << member.name
                            << ", lhs." << member.altlen << ") != 0)\n";
                    outFile << "    return false;\n\n";
                } else {
                    auto searchIt = member.altlen.find(member.len);
                    std::string tempLen = member.altlen;
                    tempLen.insert(searchIt, std::string{"lhs."});
                    outFile << "  for(uint32_t i = 0; i < " << tempLen << "; ++i) {\n";
                    outFile << "    if(lhs." << member.name << "[i] != rhs." << member.name
                            << "[i])\n";
                    outFile << "      return false;\n";
                    outFile << "  }\n\n";
                }
            }
        }
        // Regular members
        bool isFirst = true;
        for (auto const &member : it.members) {
            // Don't do array members here
            if (!member.sizeEnum.empty() || !member.len.empty())
                continue;

            // Don't do one if it's a member length count
            for (auto const &inMem : it.members) {
                if (member.name == inMem.len)
                    goto MEMBER_END;
            }

            // Don't do 'void' pointer types
            if (member.name == "pNext" && member.type == "void" && member.typeSuffix == "*")
                continue;

            // If it's the first, then we don't prefix with '&&'
            if (isFirst) {
                isFirst = false;
                outFile << "  return ";
            } else {
                outFile << " &&\n         ";
            }
            outFile << "(lhs." << member.name << " == rhs." << member.name << ")";
        MEMBER_END:;
        }
        if (isFirst)
            outFile << "  return true";

        outFile << ";\n";
        outFile << "}\n";

        // != definition
        outFile << "\nbool operator!=(" << it.name << " const &lhs,\n";
        outFile << "                " << it.name << " const &rhs) noexcept {\n";
        outFile << "  return !(lhs == rhs);\n";
        outFile << "}\n";

        if (!platformDefine.empty())
            outFile << "#endif // " << platformDefine << "\n";
    }
    outFile << "\n#endif // VK_EQUALITY_CHECK_CONFIG_MAIN\n";

    outFile << "\n#endif // VK_EQUALITY_CHECK_V" << vkHeaderVersion << "_HPP\n";

    return 0;
}