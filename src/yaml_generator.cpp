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

int main(int argc, char **argv) {
    std::string inputFile;
    std::string whiteListFile;
    std::string outputDir;
    std::string outputFile = "vk_yaml_parsing.cpp";

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
        std::cout << "Error: No input file given. Type --help for help." << std::endl;
        return 1;
    }

    // Get Whitelist
    std::vector<std::string> whiteList;
    if (!whiteListFile.empty()) {
        std::ifstream whiteListFD(whiteListFile, std::ifstream::in);
        if (!whiteListFD) {
            std::cerr << "Error: Could not open whitelist file" << std::endl;
            return 1;
        }

        std::string line;
        while (std::getline(whiteListFD, line)) {
            whiteList.push_back(line);
        }
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

    // Need to be in the 'types' node
    auto *typesNode = registryNode->first_node("types");
    if (typesNode == nullptr) {
        std::cerr << "Error: Could not find the 'types' node." << std::endl;
        return 1;
    }

    struct Member {
        std::string type;
        std::string name;
        bool optional{false};
    };

    std::stringstream yamlConversion;
    std::vector<std::string> structNames;

    // Process 'type'/'struct' types
    auto const *endTypeNode = typesNode->last_node("type");
    for (auto *typeNode = typesNode->first_node("type"); typeNode != nullptr;
         typeNode = typeNode->next_sibling()) {
        std::string_view structName;
        std::vector<Member> members;
        std::string_view sType{};
        bool outputStruct{true};

        // Check for category='struct'
        auto *categoryAttr = typeNode->first_attribute("category");
        if (categoryAttr == nullptr || strcmp("struct", categoryAttr->value()) != 0) {
            goto END_OF_TYPE;
        }
        // Don't do return-only types
        if (auto returnedOnlyAttr = typeNode->first_attribute("returnedonly");
            returnedOnlyAttr != nullptr && std::string_view{returnedOnlyAttr->value()} == "true") {
            goto END_OF_TYPE;
        }

        structName = typeNode->first_attribute("name")->value();
        if (structName == "VkPipelineColorBlendStateCreateInfo") {
            int red = 5;
        }

        structNames.push_back(std::string{structName});

        if (!whiteList.empty()) {
            // Check to see if the function is part of the whitelist and to be generated.
            outputStruct = false;
            for (auto const &it : whiteList) {
                if (it == structName) {
                    outputStruct = true;
                    break;
                }
            }
        }

        for (auto memberNode = typeNode->first_node("member"); memberNode != nullptr;
             memberNode = memberNode->next_sibling()) {
            if (std::string_view{memberNode->name()} == "member") {
                std::string_view memberValue = memberNode->value();
                Member newItem{
                    .type = memberNode->first_node("type")->value(),
                    .name = memberNode->first_node("name")->value(),
                };

                if (auto lenAttr = memberNode->first_attribute("len"); lenAttr != nullptr) {
                    std::string_view lenTarget = lenAttr->value();

                    for (auto it = members.begin(); it != members.end(); ++it) {
                        if (it->name == lenTarget) {
                            members.erase(it);
                            break;
                        }
                    }
                    goto MEMBER_LOOP_END;
                }

                for (auto &it : structNames) {
                    if (it == newItem.type) {
                        goto MEMBER_LOOP_END;
                    }
                }

                if (newItem.name == "sType" && newItem.type == "VkStructureType") {
                    auto sTypeNode = memberNode->first_attribute("values");
                    if (sTypeNode != nullptr) {
                        sType = sTypeNode->value();
                    }
                    goto MEMBER_LOOP_END;
                }

                if (auto optionalAttr = memberNode->first_attribute("optional");
                    optionalAttr != nullptr && std::string_view{optionalAttr->value()} == "true") {
                    newItem.optional = true;
                }

                if (memberValue.empty() && newItem.name != "pNext") {
                    members.emplace_back(newItem);
                }
            }

        MEMBER_LOOP_END:
            if (memberNode == typeNode->last_node("member"))
                break;
        }

        if (!members.empty() && outputStruct) {
            // Start of read function
            yamlConversion << "\n\ntemplate <>\n";
            yamlConversion << "auto readVulkanNode<" << structName << ">(YAML::Node const& node)\n";
            yamlConversion << "    -> " << structName << " {\n";
            yamlConversion << "        return " << structName << "{\n";

            if (!sType.empty()) {
                yamlConversion << "            .sType = " << sType << ",\n";
            }

            for (auto const &it : members) {
                yamlConversion << "            ." << it.name << " = ";
                // Check for Vulkan type
                if (it.type == "VkBool32") {
                    if (it.optional) {
                        yamlConversion << "readOptional<bool>(\"" << it.name << "\", node),\n";
                    } else {
                        yamlConversion << "readRequired<bool>(\"" << it.name << "\", node),\n";
                    }
                } else if (it.type.find("Vk") != std::string::npos) {
                    if (it.optional) {
                        yamlConversion << "READ_OPTIONAL_VULKAN(" << it.type << ", \"" << it.name
                                       << "\", node),\n";
                    } else {
                        yamlConversion << "READ_REQUIRED_VULKAN(" << it.type << ", \"" << it.name
                                       << "\", node),\n";
                    }
                } else {
                    if (it.optional) {
                        yamlConversion << "readOptional<" << it.type << ">(\"" << it.name
                                       << "\", node),\n";
                    } else {
                        yamlConversion << "readRequired<" << it.type << ">(\"" << it.name
                                       << "\", node),\n";
                    }
                }
            }

            // End of read function
            yamlConversion << "        };\n";
            yamlConversion << "}";

            // Begin of write function
            yamlConversion << "\n\ntemplate <>\n";
            yamlConversion << "auto writeVulkanNode<" << structName << ">(" << structName
                           << " const& data) -> YAML::Node {\n";
            yamlConversion << "        YAML::Node node;\n\n";

            for (auto const &it : members) {
                // Check for Vulkan type
                if (it.type == "VkBool32") {
                    if (it.optional) {
                        yamlConversion << "        node[\"" << it.name
                                       << "\"] = static_cast<bool>(data." << it.name << ");\n";
                    } else {
                        yamlConversion << "        node[\"" << it.name
                                       << "\"] = static_cast<bool>(data." << it.name << ");\n";
                    }
                } else if (it.type.find("Vk") != std::string::npos) {
                    if (it.optional) {
                        yamlConversion << "        WRITE_OPTIONAL_VULKAN(" << it.type << ", \""
                                       << it.name << "\", data." << it.name << ", node);\n";
                    } else {
                        yamlConversion << "        WRITE_REQUIRED_VULKAN(" << it.type << ", \""
                                       << it.name << "\", data." << it.name << ", node);\n";
                    }
                } else {
                    if (it.optional) {
                        yamlConversion << "        node[\"" << it.name << "\"] = static_cast<"
                                       << it.type << ">(data." << it.name << ");\n";
                    } else {
                        yamlConversion << "        node[\"" << it.name << "\"] = static_cast<"
                                       << it.type << ">(data." << it.name << ");\n";
                    }
                }
            }

            // End of write function
            yamlConversion << "\n        return node;\n";
            yamlConversion << "}";
        }

    END_OF_TYPE:
        if (typeNode == endTypeNode)
            break;
    }

    { // Yaml-Cpp
        std::ofstream outFile(outputDir + outputFile);
        if (!outFile.is_open()) {
            std::cerr << "Error: Failed to open output file for writing: " << outputDir
                      << outputFile << std::endl;
            return 1;
        }

        outFile << "#include <vulkan/vulkan.h>\n";
        outFile << "\n";

        outFile << yamlConversion.str();
    }
}