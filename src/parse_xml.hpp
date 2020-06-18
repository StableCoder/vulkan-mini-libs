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

#include <regex>
#include <string_view>

struct PlatformData {
    std::string_view name;
    std::string_view define;
};

std::vector<PlatformData> getPlatforms(rapidxml::xml_node<> *platformsNode) {
    std::vector<PlatformData> platforms;

    for (auto platform = platformsNode->first_node("platform"); platform != nullptr;
         platform = platform->next_sibling()) {
        PlatformData temp;
        temp.name = platform->first_attribute("name")->value();
        temp.define = platform->first_attribute("protect")->value();

        platforms.emplace_back(temp);

        // Check if this is the last platform
        if (platform == platformsNode->last_node("platform"))
            break;
    }

    return platforms;
}

struct MemberData {
    std::string_view type;
    std::string_view name;
    std::string sizeEnum;
};

struct StructData {
    std::string_view name;
    std::vector<MemberData> members;
    std::string platform;
    bool hasUnionType;
};

std::vector<StructData> getStructData(rapidxml::xml_node<> *typesNode) {
    std::vector<StructData> structs;

    for (auto *typeNode = typesNode->first_node("type"); typeNode != nullptr;
         typeNode = typeNode->next_sibling()) {
        StructData newStruct;

        // Check for category='struct'
        auto *categoryAttr = typeNode->first_attribute("category");
        if (categoryAttr == nullptr || strcmp("struct", categoryAttr->value()) != 0) {
            goto END_OF_TYPE;
        }

        newStruct.name = typeNode->first_attribute("name")->value();

        for (auto memberNode = typeNode->first_node("member"); memberNode != nullptr;
             memberNode = memberNode->next_sibling()) {

            if (std::string_view{memberNode->name()} == "member") {
                std::string sizeEnum = memberNode->value();

                std::regex searchRegex("[(0-9)+]");
                auto words_begin =
                    std::sregex_iterator(sizeEnum.begin(), sizeEnum.end(), searchRegex);
                auto words_end = std::sregex_iterator();

                if (words_begin != std::sregex_iterator()) {
                    sizeEnum = words_begin->str();
                } else {
                    sizeEnum = "";
                }

                auto *enumNode = memberNode->first_node("enum");
                if (enumNode != nullptr)
                    sizeEnum = enumNode->value();

                newStruct.members.emplace_back(MemberData{
                    .type = memberNode->first_node("type")->value(),
                    .name = memberNode->first_node("name")->value(),
                    .sizeEnum = sizeEnum,
                });
            }

        MEMBER_LOOP_END:
            if (memberNode == typeNode->last_node("member"))
                break;
        }

        structs.emplace_back(newStruct);

    END_OF_TYPE:
        if (typeNode == typesNode->last_node("type"))
            break;
    }

    return structs;
}

struct UnionData {
    std::string_view name;
    std::vector<MemberData> members;
    std::string platform;
};

std::vector<UnionData> getUnionData(rapidxml::xml_node<> *typesNode) {
    std::vector<UnionData> unions;

    for (auto *typeNode = typesNode->first_node("type"); typeNode != nullptr;
         typeNode = typeNode->next_sibling()) {
        UnionData newStruct;

        // Check for category='struct'
        auto *categoryAttr = typeNode->first_attribute("category");
        if (categoryAttr == nullptr || strcmp("union", categoryAttr->value()) != 0) {
            goto END_OF_TYPE;
        }

        newStruct.name = typeNode->first_attribute("name")->value();

        for (auto memberNode = typeNode->first_node("member"); memberNode != nullptr;
             memberNode = memberNode->next_sibling()) {

            if (std::string_view{memberNode->name()} == "member") {
                std::string sizeEnum;
                auto *enumNode = memberNode->first_node("enum");
                if (enumNode != nullptr)
                    sizeEnum = enumNode->value();

                newStruct.members.emplace_back(MemberData{
                    .type = memberNode->first_node("type")->value(),
                    .name = memberNode->first_node("name")->value(),
                    .sizeEnum = sizeEnum,
                });
            }

        MEMBER_LOOP_END:
            if (memberNode == typeNode->last_node("member"))
                break;
        }

        unions.emplace_back(newStruct);

    END_OF_TYPE:
        if (typeNode == typesNode->last_node("type"))
            break;
    }

    return unions;
}

bool structHasUnion(StructData const &structData, std::vector<UnionData> const &unions) {
    for (auto const &member : structData.members) {
        for (auto const &it : unions) {
            if (member.type == it.name) {
                return true;
            }
        }
    }

    return false;
}

void getStructPlatforms(std::vector<StructData> &structs, rapidxml::xml_node<> *extensionsNode) {
    for (auto *extension = extensionsNode->first_node("extension"); extension != nullptr;
         extension = extension->next_sibling()) {
        std::string_view platform;

        // Check for platform=
        auto *platformNode = extension->first_attribute("platform");
        if (platformNode == nullptr)
            goto END_OF_EXTENSION;
        platform = platformNode->value();

        for (auto *require = extension->first_node("require"); require != nullptr;
             require = require->next_sibling()) {

            for (auto *type = require->first_node("type"); type != nullptr;
                 type = type->next_sibling()) {
                std::string_view typeName = type->first_attribute("name")->value();

                for (auto &it : structs) {
                    if (typeName == it.name) {
                        it.platform = platform;
                    }
                }

                if (type == require->last_node("type"))
                    break;
            }

            if (require == extension->last_node("require"))
                break;
        }

    END_OF_EXTENSION:
        if (extension == extensionsNode->last_node("extension"))
            break;
    }
}