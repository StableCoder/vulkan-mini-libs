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

#include <string_view>

constexpr std::string_view usageStr = R"(
/*  USAGE:
    To use, include this header where the declarations for the boolean checks are required.

    On *ONE* compilation unit, include the definition of `#define VK_VALUE_SERIALIZATION_CONFIG_MAIN`
    so that the definitions are compiled somewhere following the one definition rule.
*/
)";

constexpr std::string_view declarationStr = R"(
/**
 * @brief Macro that automatically stringifies the given Vulkan type for serialization
 * @param VKTYPE Actual Vulkan type
 * @param VALUE Value to be serialized
 * @param STRPTR Pointer to the string to store the serialization in. Only modified if true is
 * returned.
 * @return True if serialization was successful. False otherwise.
 */
#define VK_SERIALIZE(VKTYPE, VALUE, STRPTR) vk_serialize<VKTYPE>(#VKTYPE, VALUE, STRPTR)

/**
 * @brief Macro that automatically stringifies the given Vulkan type for parsing
 * @param VKTYPE Actual Vulkan type
 * @param STRING String to be parsed
 * @param VALPTR Pointer to the value to store the parsed value in. Only modified if true is
 * returned.
 * @return True if serialization was successful. False otherwise.
 */
#define VK_PARSE(VKTYPE, STRING, VALPTR) vk_parse<VKTYPE>(#VKTYPE, STRING, VALPTR)

/**
 * @brief Serializes a Vulkan enumerator/flag type
 * @param vkType Name of the Vulkan enumerator/flag type
 * @param vkValue Value being serialized
 * @param pString Pointer to a string that will be modified with the serialized value. Only modified
 * if true is returned.
 * @return True the value was successfully serialized. False otherwise.
 */
bool vk_serialize(std::string_view vkType, uint32_t vkValue, std::string *pString);

/**
 * @brief Parses a Vulkan enumerator/flag serialized string
 * @param vkType Name of the Vulkan enumerator/flag type
 * @param vkString String being parsed
 * @param pValue Pointer to a value that will be modified with the parsed value. Only modified if
 * true is returned.
 * @return True the value was successfully serialized. False otherwise.
 */
bool vk_parse(std::string_view vkType, std::string vkString, uint32_t *pValue);

/**
 * @brief Serializes a Vulkan enumerator/flag type
 * @tparam Vulkan type being serialized
 * @param vkType Name of the Vulkan enumerator/flag type
 * @param vkValue Value being serialized
 * @param pString Pointer to a string that will be modified with the serialized value. Only modified
 * if true is returned.
 * @return True the value was successfully serialized. False otherwise.
 */
template <typename T>
bool vk_serialize(std::string_view vkType, T vkValue, std::string *pString) {
    return vk_serialize(vkType, static_cast<uint32_t>(vkValue), pString);
}

/**
 * @brief Parses a Vulkan enumerator/flag serialized string
 * @tparam Vulkan type being parsed
 * @param vkType Name of the Vulkan enumerator/flag type
 * @param vkString String being parsed
 * @param pValue Pointer to a value that will be modified with the parsed value. Only modified if
 * true is returned.
 * @return True the value was successfully serialized. False otherwise.
 */
template <typename T>
bool vk_parse(std::string_view vkType, std::string vkString, T *pValue) {
    uint32_t retVal = 0;
    auto found = vk_parse(vkType, vkString, &retVal);
    if (found) {
        *pValue = static_cast<T>(retVal);
    }
    return found;
}
)";

constexpr std::string_view stripFuncsStr = R"(
/**
 * @brief Removes a vendor tag from the end of the given string view
 * @param view String view to remove the vendor tag from
 * @return A string_view without the vendor tag, if it was suffixed
 */
std::string_view stripVendor(std::string_view view) {
    for (auto const &it : vendors) {
        // Don't strip if it's all that's left
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
)";

constexpr std::string_view otherFuncsStr = R"(
std::tuple<EnumValueSet const *, EnumValueSet const *> getEnumType(std::string_view vkType) {
    // Check for a conversion from Flags -> FlagBits
    std::string localString;
    if (vkType.rfind("Flags") != std::string::npos) {
        localString = vkType;
        auto it = localString.rfind("Flags");
        localString = localString.replace(it, strlen("Flags"), "FlagBits");
        vkType = localString;
    }

    // Try the original name
    for (auto const &it : enumTypes) {
        if (vkType == std::string_view{it.name}) {
            if (it.data == nullptr)
                return std::make_tuple(nullptr, nullptr);
            return std::make_tuple(it.data, it.data + it.count);
        }
    }

    // Try a vendor-stripped name
    vkType = stripVendor(vkType);
    for (auto const &it : enumTypes) {
        if (vkType == std::string_view{it.name}) {
            if (it.data == nullptr)
                return std::make_tuple(nullptr, nullptr);
            return std::make_tuple(it.data, it.data + it.count);
        }
    }

    return std::make_tuple(nullptr, nullptr);
}

/**
 * @brief Converts a Vulkan Flag typename into the prefix that is used for it's enums
 * @param typeName Name of the type to generate the Vk enum prefix for
 * @return Generated prefix string
 *
 * Any capitalized letters except for the first has an underscore inserted before it, an underscore
 * is added to the end, and all characters are converted to upper case.
 *
 * It also removed the 'Flags' or 'FlagBits' suffixes.
 */
std::string processEnumPrefix(std::string_view typeName) {
    // Flag Bits
    std::size_t flagBitsSize = strlen("FlagBits");
    if (typeName.size() > flagBitsSize) {
        if (strncmp(typeName.data() + typeName.size() - flagBitsSize, "FlagBits", flagBitsSize) ==
            0) {
            typeName = typeName.substr(0, typeName.size() - strlen("FlagBits"));
        }
    }
    // Flags
    std::size_t flagsSize = strlen("Flags");
    if (typeName.size() > flagsSize) {
        if (strncmp(typeName.data() + typeName.size() - flagsSize, "Flags", flagsSize) == 0) {
            typeName = typeName.substr(0, typeName.size() - strlen("Flags"));
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

bool findValue(std::string_view findValue,
               std::string_view prefix,
               uint32_t *pValue,
               EnumValueSet const *start,
               EnumValueSet const *end) {
    // Remove the vendor tag suffix if it's on the value
    findValue = stripVendor(findValue);
    if (findValue[findValue.size() - 1] == '_')
        findValue = findValue.substr(0, findValue.size() - 1);

    // Remove '_BIT' if it's there
    findValue = stripBit(findValue);

    // Iterate until we find the value
    while (start != end) {
        if (findValue == start->name) {
            *pValue |= start->value;
            return true;
        }

        std::string prefixedName{prefix};
        prefixedName += start->name;
        if (findValue == prefixedName) {
            *pValue |= start->value;
            return true;
        }

        ++start;
    }

    return false;
}

/**
 * @brief Takes a given string and formats it for use with parsing
 * @param str The string to format
 * @return Formatted string
 *
 * First, any non alphanumeric characters are trimmed from both ends of the string.
 * After than, any spaces are replaced with underscores, and finally all the characters are
 * capitalized. This will generate the string closest to the original ones found in the XML spec.
 */
std::string formatString(std::string str) {
    // Trim left
    std::size_t cutOffset = 0;
    for (auto c : str) {
        if (::isalnum(c))
            break;
        else
            ++cutOffset;
    }
    str = str.substr(cutOffset);

    // Trim right
    cutOffset = 0;
    for (std::size_t i = 0; i < str.size(); ++i) {
        if (::isalnum(str[i]))
            cutOffset = i + 1;
    }
    str = str.substr(0, cutOffset);

    std::replace(str.begin(), str.end(), ' ', '_');
    std::for_each(str.begin(), str.end(), [](char &c) { c = ::toupper(c); });

    return str;
}
)";

constexpr std::string_view stringifyFuncsStr = R"(
bool serializeBitmask(std::string_view vkType, uint32_t vkValue, std::string *pString) {
    auto [end, start] = getEnumType(vkType);
    --end;
    --start;

    std::string retStr;
    while (start != end && vkValue != 0) {
        if ((start->value & vkValue) == start->value) {
            // Found a compatible bit mask, add it
            if (!retStr.empty()) {
                retStr += " | ";
            }
            retStr += start->name;
            vkValue = vkValue ^ start->value;
        }

        --start;
    }

    if (vkValue != 0) {
        // Failed to find a valid bitmask for the value
        return false;
    }

    *pString = retStr;
    return true;
}

bool serializeEnum(std::string_view vkType, uint32_t vkValue, std::string *pString) {
    auto [start, end] = getEnumType(vkType);

    while (start != end) {
        if (start->value == vkValue) {
            *pString = start->name;
            return true;
        }

        ++start;
    }

    return false;
}
)";

constexpr std::string_view parseFuncsStr = R"(
bool parseBitmask(std::string_view vkType, std::string_view vkString, uint32_t *pValue) {
    auto [start, end] = getEnumType(vkType);
    std::string prefix = processEnumPrefix(stripVendor(vkType));
    uint32_t retVal = 0;

    auto startCh = vkString.begin();
    auto endCh = startCh;
    for (; endCh != vkString.end(); ++endCh) {
        if (*endCh == '|') {
            std::string token(startCh, endCh);
            token = formatString(token);

            bool foundVal = findValue(token, prefix, &retVal, start, end);
            if (!foundVal)
                return false;

            startCh = endCh + 1;
        }
    }
    if (startCh != endCh) {
        std::string token(startCh, endCh);
        token = formatString(token);

        bool foundVal = findValue(token, prefix, &retVal, start, end);
        if (!foundVal)
            return false;
    }

    *pValue = retVal;
    return true;
}

bool parseEnum(std::string_view vkType, std::string_view vkString, uint32_t *pValue) {
    auto [start, end] = getEnumType(vkType);
    std::string prefix = processEnumPrefix(stripVendor(vkType));
    uint32_t retVal = 0;

    std::string token = formatString(std::string{vkString});
    bool found = findValue(token, prefix, &retVal, start, end);
    if (found) {
        *pValue = retVal;
    }
    return found;
}
)";

constexpr std::string_view publicFuncsStr = R"(
bool vk_serialize(std::string_view vkType, uint32_t vkValue, std::string *pString) {
    if (vkType.empty()) {
        return false;
    }

    std::string_view temp = stripVendor(vkType);
    if (temp.ends_with("FlagBits") || temp.ends_with("Flags")) {
        return serializeBitmask(vkType, vkValue, pString);
    }

    return serializeEnum(vkType, vkValue, pString);
}

bool vk_parse(std::string_view vkType, std::string vkString, uint32_t *pValue) {
    if (vkType.empty()) {
        return false;
    }
    if(vkString.empty()) {
        *pValue = 0;
        return true;
    }

    std::string_view temp = stripVendor(vkType);
    if (temp.ends_with("FlagBits") || temp.ends_with("Flags")) {
        return parseBitmask(vkType, vkString, pValue);
    }

    return parseEnum(vkType, vkString, pValue);
}
)";

constexpr std::string_view helpStr = R"HELP(
This program builds a source/header file for use in C++17 or newer. It lists
contains all Vulkan enum types/flags/values of the indicated Vulkan header spec
version, and can convert to/from strings representing those values. 

Supports both plain enums and the bitmasks.

When converting values to strings, where possible a shorter version of the
enum string is used, where the verbose type prefix is removed:
    VK_IMAGE_LAYOUT_GENERAL => GENERAL
    VK_CULL_MODE_FRONT_BIT | VK_CULL_MODE_BACK_BIT => FRONT | BACK

When converting from strings into values, either the short OR full string can
be used where strings are case insensitive, and underscores can be replaced
with spaces, and addition whitespace can be added to either side of the first/
last alphanumeric character, as these are trimmed off.

For example, all of the following convert to VK_IMAGE_LAYOUT_GENERAL:
`vk imAGE_LayOut GenerAL`, `VK_IMAGE_LAYOUT_GENERAL`,`GENERAL`, `   General `

Also, to assist with forward and backwards compatability, all the vendor tags
are stripped from the typenames and values, since they can be removed in later
versions leading to incompatability issues. For example, the flag for 
VkToolPurposeFlagBitsEXT, `VK_TOOL_PURPOSE_VALIDATION_BIT_EXT`, can will
be stringified as `VALIDATION_BIT`, and can be read similarly, with the above
rules applicable for de-stringifying.

Program Arguments:
    -h, --help  : Help Blurb
    -i, --input : Input vk.xml file to parse. These can be found from the 
                    KhronosGroup, often at this repo:
                    https://github.com/KhronosGroup/Vulkan-Docs
    -d, --dir   : Output directory
    -o, --out   : Output file name (Default: `vk_value_serialization.hpp`)
)HELP";