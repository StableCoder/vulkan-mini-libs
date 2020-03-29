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

#include <catch.hpp>
#include <vk_value_stringifier.hpp>
#include <vulkan/vulkan.h>

using namespace vkEnum;

TEST_CASE("Stringify: Failure cases") {
    REQUIRE_FALSE(stringifyBitmask("", 0).has_value());
    REQUIRE_FALSE(stringifyBitmask("", 0).has_value());
}

TEST_CASE("Stringify: Enum") {
    SECTION("Failure case where a bad type is given") {
        REQUIRE_FALSE(stringifyEnum("VkGarbagio", VK_IMAGE_TYPE_3D).has_value());
    }
    SECTION("Failure case where bad enum given") {
        stringifyEnum("VkImageType", -1U);
        REQUIRE_FALSE(stringifyEnum("VkImageType", -1U).has_value());
    }
    SECTION("Success cases") {
        REQUIRE(stringifyEnum("VkImageType", VK_IMAGE_TYPE_3D).value() == "3D");
        REQUIRE(stringifyEnum("VkImageType", VK_IMAGE_TYPE_2D).value() == "2D");
    }

    SECTION("Vendor Tag Success") {
        REQUIRE(stringifyEnum("VkPresentModeKHR", VK_PRESENT_MODE_IMMEDIATE_KHR).value() ==
                "IMMEDIATE");
    }
}

TEST_CASE("Stringify: Bitmask") {
    SECTION("Failure case where a bad type is given") {
        REQUIRE_FALSE(stringifyBitmask("VkGarbagio", VK_CULL_MODE_BACK_BIT).has_value());
    }
    SECTION("Regular success cases") {
        REQUIRE(
            stringifyBitmask("VkDebugReportFlagBitsEXT", VK_DEBUG_REPORT_ERROR_BIT_EXT).value() ==
            "ERROR");
        REQUIRE(stringifyBitmask("VkDebugReportFlagBitsEXT",
                                 VK_DEBUG_REPORT_DEBUG_BIT_EXT | VK_DEBUG_REPORT_ERROR_BIT_EXT)
                    .value() == "DEBUG | ERROR");
    }
    SECTION("Combined bitmask will use larger items first") {
        REQUIRE(
            stringifyBitmask("VkCullModeFlagBits", VK_CULL_MODE_BACK_BIT | VK_CULL_MODE_FRONT_BIT)
                .value() == "FRONT_AND_BACK");
    }
}