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
#include <vk_enum_stringifier_v132.hpp>
#include <vulkan/vulkan.h>

using namespace vkEnum;

TEST_CASE("Stringify: Enum") {
    SECTION("Failure case where a bad type is given") {
        REQUIRE(stringifyEnum("VkGarbagio", VK_IMAGE_TYPE_3D).empty());
    }
    SECTION("Success cases") {
        REQUIRE(stringifyEnum("VkImageType", VK_IMAGE_TYPE_3D) == "3D");
        REQUIRE(stringifyEnum("VkImageType", VK_IMAGE_TYPE_2D) == "2D");
    }

    SECTION("Vendor Tag Success") {
        REQUIRE(stringifyEnum("VkPresentModeKHR", VK_PRESENT_MODE_IMMEDIATE_KHR) == "IMMEDIATE");
    }
}

TEST_CASE("Stringify: Bitmask") {
    SECTION("Failure case where a bad type is given") {
        REQUIRE(stringifyBitmask("VkGarbagio", VK_CULL_MODE_BACK_BIT).empty());
    }
    SECTION("Regular success cases") {
        REQUIRE(stringifyBitmask("VkResolveModeFlagBits", VK_RESOLVE_MODE_MIN_BIT) == "MIN_BIT");
        REQUIRE(stringifyBitmask("VkResolveModeFlagBits",
                                 VK_RESOLVE_MODE_MAX_BIT | VK_RESOLVE_MODE_MIN_BIT) ==
                "MAX_BIT | MIN_BIT");
    }
    SECTION("Combined bitmask will use larger items first") {
        REQUIRE(stringifyBitmask("VkCullModeFlagBits",
                                 VK_CULL_MODE_BACK_BIT | VK_CULL_MODE_FRONT_BIT) ==
                "FRONT_AND_BACK");
    }
}