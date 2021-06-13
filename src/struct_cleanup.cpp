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

    On *ONE* compilation unit, include the definition of `#define VK_STRUCT_CLEANUP_CONFIG_MAIN`
    so that the definitions are compiled somewhere following the one definition rule.
*/
)USE";

std::string_view functionDoc = R"FUNCDOC(
/** @brief Cleans up a Vulkan sType-based structure of pointer data
 * @param pData Pointer to the struct to be cleaned up
 * 
 * This function is only to be called on Vulkan structures that *have* a VkStructureType
 * member 'sType'. Any pointer members that start with 'p[A-Z]' are assumed to be *owned*
 * by the struct and are themselves cleaned up recusrively, and then deleted.
 *
 * This means any other pointer members, or pointer to pointer members are not cleaned up
 * and would still require manual deletion.
 */
)FUNCDOC";

constexpr std::string_view helpStr = R"HELP(
Generates header files for C++. The functions deal are used to cleanup any
data that the struct points to externally, recusrively. This assumes that the
struct is the *owner* of said data.

Program Arguments:
    -h, --help  : Help Blurb
    -i, --input : Input vk.xml file to parse. These can be found from the 
                    KhronosGroup, often at this repo:
                    https://github.com/KhronosGroup/Vulkan-Docs
    -d, --dir   : Output directory
    -o, --out   : Output file name (Default: `vk_struct_cleanup.hpp`)
)HELP";

bool canCleanup(StructData const &structData) {
    bool hasType{false};
    bool hasPointerData{false};
    for (auto const &mem : structData.members) {
        if (mem.typeSuffix == "*")
            hasPointerData = true;
        if (mem.type == "VkStructureType")
            hasType = true;
    }

    return hasType && hasPointerData;
}

bool nonOwnedMember(MemberData const &memberData) {
    if (memberData.name[0] != 'p')
        return true;

    if (tolower(memberData.name[1]) == memberData.name[1])
        return true;

    return false;
}

int main(int argc, char **argv) {
    std::string inputFile;
    std::string outputDir;
    std::string outputFile = "vk_struct_cleanup.hpp";

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

    outFile << "#ifndef VK_STRUCT_CLEANUP_V" << vkHeaderVersion << "_HPP\n";
    outFile << "#define VK_STRUCT_CLEANUP_V" << vkHeaderVersion << "_HPP\n";

    outFile << headerUsageStr;

    outFile << "\n#include <vulkan/vulkan.h>\n";
    outFile << "\n";

    // Static assert checking correct/compatible header version
    outFile << "static_assert(VK_HEADER_VERSION == " << vkHeaderVersion
            << ", \"Incompatible VK_HEADER_VERSION!\" );\n";

    // Declarations
    outFile << functionDoc;
    outFile << "void vk_struct_cleanup(void const* pData);\n";

    // Definitions
    outFile << "\n#ifdef VK_STRUCT_CLEANUP_CONFIG_MAIN\n";
    outFile << "\n#include <cstdlib>\n";
    outFile << "\nvoid vk_struct_cleanup(void const *pData) {\n";
    outFile << "    if (pData == nullptr)\n";
    outFile << "        return;\n";
    outFile << "\n    struct VkTempStruct {\n";
    outFile << "        VkStructureType sType;\n";
    outFile << "    };\n";
    outFile << "    VkTempStruct const *pTemp = static_cast<VkTempStruct const *>(pData);\n";

    outFile << "\n    switch (pTemp->sType) {\n";

    for (auto const &it : structs) {
        if (it.name == "VkBaseOutStructure" || it.name == "VkBaseInStructure" ||
            it.name == "VkCuLaunchInfoNVX")
            continue;

        bool hasType{false};
        bool hasPointerData{false};
        for (auto const &mem : it.members) {
            if (mem.typeSuffix == "*")
                hasPointerData = true;
            if (mem.type == "VkStructureType")
                hasType = true;
        }

        if (!hasPointerData)
            continue;
        if (hasPointerData && !hasType) {
            std::cout << "Info: Skipping Vk structure " << it.name
                      << " that has pointer members but not a VkStructureType" << std::endl;
            continue;
        }

        std::string_view platformDefine;
        if (!it.platform.empty()) {
            // Find the platform
            for (auto &platform : platforms) {
                if (it.platform == platform.name) {
                    platformDefine = platform.define;
                    break;
                }
            }

            outFile << "#ifdef " << platformDefine << "\n";
        }

        for (auto const &mem : it.members) {
            if (mem.type == "VkStructureType") {
                outFile << "    case " << mem.values << ":\n";
                outFile << "        {   // " << it.name << "\n";
                outFile << "            " << it.name << " const *pStruct = static_cast<" << it.name
                        << " const *>(pData);\n";
                break;
            }
        }

        for (auto const &mem : it.members) {
            if (mem.typeSuffix == "*") {
                // Determine if we need to recursively cleanup
                if (nonOwnedMember(mem))
                    continue;

                outFile << "\n            // " << mem.type << " - " << mem.name;
                if (!mem.len.empty()) {
                    outFile << " / " << mem.len;
                }
                outFile << "\n";

                if (mem.name == "pNext") {
                    outFile << "            vk_struct_cleanup(pStruct->pNext);\n";
                } else {
                    for (auto const &inIt : structs) {
                        if (inIt.name == mem.type && canCleanup(inIt)) {
                            if (mem.len.empty()) {
                                outFile << "            vk_struct_cleanup(pStruct->" << mem.name
                                        << ");\n";
                            } else {
                                outFile << "            for (uint32_t i = 0; i < pStruct->"
                                        << mem.len << "; ++i)\n";
                                outFile << "                vk_struct_cleanup(&pStruct->"
                                        << mem.name << "[i]);\n";
                            }
                        }
                    }
                }

                if (mem.len.empty()) {
                    outFile << "            free(const_cast<" << mem.type << " *>(pStruct->"
                            << mem.name << "));\n";
                } else {
                    outFile << "            free(const_cast<" << mem.type << " *>(pStruct->"
                            << mem.name << "));\n";
                }
            }
        }

        outFile << "        }\n";
        outFile << "\n        break;\n";
        if (!platformDefine.empty())
            outFile << "#endif // " << platformDefine << "\n\n";
        else
            outFile << "\n";
    }

    outFile << "\n    default:\n";
    outFile << "        break;\n";
    outFile << "    }\n";
    outFile << "}\n";

    outFile << "\n#endif // VK_STRUCT_CLEANUP_CONFIG_MAIN\n";

    // Finish Up
    outFile << "\n#endif // VK_STRUCT_CLEANUP_V" << vkHeaderVersion << "_HPP\n";

    return 0;
}
