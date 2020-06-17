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

std::string_view headerUsageStr = R"USE(
/*
    To use, include this header where the declarations for the boolean checks are required.

    On *ONE* compilation unit, include the definition of `#define VK_EQUALITY_CHECK_CONFIG_MAIN`
    so that the definitions are compiled somewhere following the one definition rule.
*/
)USE";

struct Member {
    std::string type;
    std::string name;
};

int main(int argc, char **argv) {
    std::string inputFile;
    std::string whiteListFile;
    std::string outputDir;
    std::string outputFile = "vk_equality_checks.hpp";

    for (int i = 0; i < argc; ++i) {
        if (strcmp(argv[i], "-h") == 0 || strcmp(argv[i], "--help") == 0) {
            //   std::cout << helpStr << std::endl;
            return 0;
        } else if (strcmp(argv[i], "-i") == 0 || strcmp(argv[i], "--input") == 0) {
            if (i + 1 <= argc) {
                inputFile = argv[i + 1];
            }
        } else if (strcmp(argv[i], "-w") == 0 || strcmp(argv[i], "--whitelist") == 0) {
            if (i + 1 <= argc) {
                whiteListFile = argv[i + 1];
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

    // Need to be in the 'types' node
    auto *typesNode = registryNode->first_node("types");
    if (typesNode == nullptr) {
        std::cerr << "Error: Could not find the 'types' node." << std::endl;
        return 1;
    }

    // Process type/struct types
    std::stringstream declarationStr;
    std::stringstream definitionStr;

    auto const *endTypeNode = typesNode->last_node("type");
    for (auto *typeNode = typesNode->first_node("type"); typeNode != nullptr;
         typeNode = typeNode->next_sibling()) {
        std::string_view structName;
        std::vector<Member> members;

        // Check for category='struct'
        auto *categoryAttr = typeNode->first_attribute("category");
        if (categoryAttr == nullptr || strcmp("struct", categoryAttr->value()) != 0) {
            goto END_OF_TYPE;
        }

        structName = typeNode->first_attribute("name")->value();

        for (auto memberNode = typeNode->first_node("member"); memberNode != nullptr;
             memberNode = memberNode->next_sibling()) {

            if (std::string_view{memberNode->name()} == "member") {
                Member newItem;
                newItem.type = memberNode->first_node("type")->value();
                newItem.name = memberNode->first_node("name")->value();

                members.emplace_back(newItem);
            }

        MEMBER_LOOP_END:
            if (memberNode == typeNode->last_node("member"))
                break;
        }

        if (!members.empty()) {
            // Declarations
            declarationStr << "\nbool operator==(" << structName << " const &lhs,\n";
            declarationStr << "                " << structName << " const &rhs) noexcept;\n";
            declarationStr << "bool operator!=(" << structName << " const &lhs,\n";
            declarationStr << "                " << structName << " const &rhs) noexcept;\n";

            // == definition
            definitionStr << "\nbool operator==(" << structName << " const &lhs,\n";
            definitionStr << "                " << structName << " const &rhs) noexcept {\n";
            definitionStr << "  return ";
            bool isFirst = true;
            for (auto const &member : members) {
                if (isFirst) {
                    isFirst = false;
                } else {
                    definitionStr << " &&\n         ";
                }
                definitionStr << "(lhs." << member.name << " == rhs." << member.name << ")";
            }
            definitionStr << ";\n";
            definitionStr << "}\n";

            // != definition
            definitionStr << "\nbool operator!=(" << structName << " const &lhs,\n";
            definitionStr << "                " << structName << " const &rhs) noexcept {\n";
            definitionStr << "  return !(lhs == rhs);\n";
            definitionStr << "}\n";
        }

    END_OF_TYPE:
        if (typeNode == endTypeNode)
            break;
    }

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
            << ", \"Wrong VK_HEADER_VERSION!\" );\n";

    outFile << declarationStr.str();

    outFile << "\n#ifdef VK_EQUALITY_CHECK_CONFIG_MAIN\n";

    outFile << definitionStr.str();

    outFile << "\n#endif // VK_EQUALITY_CHECK_CONFIG_MAIN\n";

    outFile << "\n#endif // VK_EQUALITY_CHECK_V" << vkHeaderVersion << "_HPP\n";

    return 0;
}