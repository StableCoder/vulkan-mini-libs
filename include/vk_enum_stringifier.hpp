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

#ifndef VK_ENUM_STRINGIFIER_HPP
#define VK_ENUM_STRINGIFIER_HPP

#include <cstdint>
#include <optional>
#include <string>
#include <string_view>

namespace vkEnum {

// Returns the Vulkan header version the data was generated from
uint32_t vulkanHeaderVersion();

/**
 * @brief Parses a given enum type/value for an enum type
 * @param enumType Vulkan enum typename as a string
 * @param value String of the value to parse
 * @return The value if the type/values are found. Returns nullopt is something can't be found.
 *
 * Formats the string and attempts to find a match, and returns it. Enums can only match one.
 */
std::optional<uint32_t> parseEnum(std::string_view enumType, std::string value);

/**
 * @brief Parses a given enum type/value(s) for a mask type
 * @param enumType Vulkan enum typename as a string
 * @param value String of the value(s) to parse.
 * @return The value if the type/values are found. Returns nullopt is something can't be found.
 *
 * Tokenizes the given value string, delimited by '|', and for each token attempts to find the value
 * in the provided enum value set, and if found, OR's it witht eh current return value.
 */
std::optional<uint32_t> parseBitmask(std::string_view enumType, std::string value);

/**
 * @brief Stringifies a given Vk type/value for an enum
 * @param enumType Vulkan enum typename as a string
 * @param enumValue Value to convert into a string
 * @return The value if the type/values are found. Returns nullopt is something can't be found.
 */
std::optional<std::string> stringifyEnum(std::string_view enumType, uint32_t enumValue);

/**
 * @brief Stringifies a given Vk type/value for a bitmask
 * @param enumType Vulkan enum typename as a string
 * @param enumValue Value to convert into a string
 * @return Short string representing the bitmask values delimited by the '|' symbol, nullopt if the
 * type or a value is not found.
 */
std::optional<std::string> stringifyBitmask(std::string_view enumType, uint32_t enumValue);

} // namespace vkEnum

#endif // VK_ENUM_STRINGIFIER_HPP
