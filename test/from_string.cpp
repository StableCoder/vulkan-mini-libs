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

TEST_CASE("De-stringify: Failure Cases") {
    SECTION("Garbage type returns nothing") {
        REQUIRE(0 == parseEnum("VkGarbagio", "2D"));
        REQUIRE(0 == parseBitmask("VkGarbagio", "2D"));
    }
    SECTION("Garbage values return nothing") {
        REQUIRE(0 == parseEnum("VkImageType", "6D"));
        REQUIRE(0 == parseBitmask("VkResolveModeFlagBits", "NOT_EXIST"));
    }
    SECTION("Attempting to do a bitmask for an enum returns nothing") {
        REQUIRE(0 == parseEnum("VkImageType", "2D | 3D"));
    }
}

TEST_CASE(
    "De-stringify: Checking enum conversions from strings to the values from the  actual header") {
    SECTION("With original shortened strings") {
        REQUIRE(VK_IMAGE_LAYOUT_UNDEFINED == parseEnum("VkImageLayout", "UNDEFINED"));
        REQUIRE(VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL ==
                parseEnum("VkImageLayout", "TRANSFER_DST_OPTIMAL"));

        REQUIRE(VK_IMAGE_TYPE_2D == parseEnum("VkImageType", "2D"));
    }
    SECTION("With extra spaces around the type") {
        REQUIRE(VK_IMAGE_LAYOUT_UNDEFINED ==
                parseEnum("VkImageLayout", "    VK_IMAGE_LAYOUT_UNDEFINED    "));
    }
    SECTION("With mixed capitalization and mixture of underscores/spaces") {
        REQUIRE(VK_IMAGE_LAYOUT_UNDEFINED ==
                parseEnum("VkImageLayout", "    vK IMgeE_LAyOUt UNDEFIned    "));
    }
    SECTION("With full strings") {
        REQUIRE(VK_IMAGE_LAYOUT_UNDEFINED ==
                parseEnum("VkImageLayout", "VK_IMAGE_LAYOUT_UNDEFINED"));
        REQUIRE(VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL ==
                parseEnum("VkImageLayout", "VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL"));

        REQUIRE(VK_IMAGE_TYPE_2D == parseEnum("VkImageType", "VK_IMAGE_TYPE_2D"));
    }

    SECTION("With Vendor Tags") {
        REQUIRE(VK_PRESENT_MODE_IMMEDIATE_KHR ==
                parseEnum("VkPresentModeKHR", "VK_PRESENT_MODE_IMMEDIATE_KHR"));
        REQUIRE(VK_PRESENT_MODE_IMMEDIATE_KHR == parseEnum("VkPresentModeKHR", "IMMEDIATE_KHR"));

        REQUIRE(VK_PRESENT_MODE_IMMEDIATE_KHR ==
                parseEnum("VkPresentModeKHR", "VK_PRESENT_MODE_IMMEDIATE"));
        REQUIRE(VK_PRESENT_MODE_IMMEDIATE_KHR == parseEnum("VkPresentModeKHR", "IMMEDIATE"));
    }
}

TEST_CASE("De-stringify: Checking bitmask conversions from string to bitmask values") {
    SECTION("With original shortened strings") {
        REQUIRE(VK_RESOLVE_MODE_SAMPLE_ZERO_BIT ==
                parseBitmask("VkResolveModeFlagBits", "SAMPLE_ZERO_BIT"));
        REQUIRE(VK_RESOLVE_MODE_MAX_BIT == parseBitmask("VkResolveModeFlagBits", "MAX_BIT"));
        REQUIRE(VK_RESOLVE_MODE_MIN_BIT == parseBitmask("VkResolveModeFlagBits", "MIN_BIT"));

        REQUIRE((VK_RESOLVE_MODE_MAX_BIT | VK_RESOLVE_MODE_MIN_BIT) ==
                parseBitmask("VkResolveModeFlagBits", "MAX_BIT | MIN_BIT"));
    }
    SECTION("With full strings") {
        REQUIRE(VK_RESOLVE_MODE_SAMPLE_ZERO_BIT ==
                parseBitmask("VkResolveModeFlagBits", "VK_RESOLVE_MODE_SAMPLE_ZERO_BIT"));
        REQUIRE(VK_RESOLVE_MODE_MAX_BIT ==
                parseBitmask("VkResolveModeFlagBits", "VK_RESOLVE_MODE_MAX_BIT"));
        REQUIRE(VK_RESOLVE_MODE_MIN_BIT ==
                parseBitmask("VkResolveModeFlagBits", "VK_RESOLVE_MODE_MIN_BIT"));

        REQUIRE((VK_RESOLVE_MODE_MAX_BIT | VK_RESOLVE_MODE_MIN_BIT) ==
                parseBitmask("VkResolveModeFlagBits",
                             "VK_RESOLVE_MODE_MAX_BIT | VK_RESOLVE_MODE_MIN_BIT"));
        REQUIRE((VK_RESOLVE_MODE_MAX_BIT | VK_RESOLVE_MODE_MIN_BIT) ==
                parseBitmask("VkResolveModeFlagBits",
                             "VK_RESOLVE_MODE_MIN_BIT | VK_RESOLVE_MODE_MAX_BIT"));
    }
    SECTION("With mixed short/full strings") {
        REQUIRE((VK_RESOLVE_MODE_MAX_BIT | VK_RESOLVE_MODE_MIN_BIT) ==
                parseBitmask("VkResolveModeFlagBits", "MAX_BIT | "
                                                      "VK_RESOLVE_MODE_MIN_BIT"));
        REQUIRE((VK_RESOLVE_MODE_MAX_BIT | VK_RESOLVE_MODE_MIN_BIT) ==
                parseBitmask("VkResolveModeFlagBits", "VK_RESOLVE_MODE_MIN_BIT | MAX_BIT"));
    }
}