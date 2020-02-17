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
#include <vk_enum_stringifier.hpp>
#include <vulkan/vulkan.h>

using namespace vkEnum;

TEST_CASE("De-stringify: Failure Cases") {
    SECTION("Garbage type returns nullopt") {
        REQUIRE_FALSE(parseEnum("VkGarbagio", "2D").has_value());
        REQUIRE_FALSE(parseBitmask("VkGarbagio", "2D").has_value());
    }
    SECTION("Garbage values return nothing") {
        REQUIRE_FALSE(parseEnum("VkImageType", "6D").has_value());
        REQUIRE_FALSE(parseBitmask("VkDebugReportFlagBitsEXT", "NOT_EXIST").has_value());
    }
    SECTION("Attempting to do a bitmask for an enum returns nothing") {
        REQUIRE_FALSE(parseEnum("VkImageType", "2D | 3D").has_value());
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
                parseEnum("VkImageLayout", "    vK IMagE_LAyOUt UNDEFIned    "));
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
        REQUIRE(VK_DEBUG_REPORT_PERFORMANCE_WARNING_BIT_EXT ==
                parseBitmask("VkDebugReportFlagBitsEXT", "PERFORMANCE_WARNING_BIT"));
        REQUIRE(VK_DEBUG_REPORT_PERFORMANCE_WARNING_BIT_EXT ==
                parseBitmask("VkDebugReportFlagBitsEXT", "PERFORMANCE_WARNING"));

        REQUIRE(VK_DEBUG_REPORT_DEBUG_BIT_EXT ==
                parseBitmask("VkDebugReportFlagBitsEXT", "DEBUG_BIT"));
        REQUIRE(VK_DEBUG_REPORT_DEBUG_BIT_EXT == parseBitmask("VkDebugReportFlagBitsEXT", "DEBUG"));
        REQUIRE(VK_DEBUG_REPORT_ERROR_BIT_EXT ==
                parseBitmask("VkDebugReportFlagBitsEXT", "ERROR_BIT"));
        REQUIRE(VK_DEBUG_REPORT_ERROR_BIT_EXT == parseBitmask("VkDebugReportFlagBitsEXT", "ERROR"));

        REQUIRE((VK_DEBUG_REPORT_DEBUG_BIT_EXT | VK_DEBUG_REPORT_ERROR_BIT_EXT) ==
                parseBitmask("VkDebugReportFlagBitsEXT", "DEBUG_BIT | ERROR_BIT"));
        REQUIRE((VK_DEBUG_REPORT_DEBUG_BIT_EXT | VK_DEBUG_REPORT_ERROR_BIT_EXT) ==
                parseBitmask("VkDebugReportFlagBitsEXT", "DEBUG | ERROR"));
    }
    SECTION("With full strings") {
        REQUIRE(
            VK_DEBUG_REPORT_PERFORMANCE_WARNING_BIT_EXT ==
            parseBitmask("VkDebugReportFlagBitsEXT", "VK_DEBUG_REPORT_PERFORMANCE_WARNING_BIT"));
        REQUIRE(VK_DEBUG_REPORT_PERFORMANCE_WARNING_BIT_EXT ==
                parseBitmask("VkDebugReportFlagBitsEXT", "VK_DEBUG_REPORT_PERFORMANCE_WARNING"));

        REQUIRE(VK_DEBUG_REPORT_DEBUG_BIT_EXT ==
                parseBitmask("VkDebugReportFlagBitsEXT", "VK_DEBUG_REPORT_DEBUG_BIT"));
        REQUIRE(VK_DEBUG_REPORT_DEBUG_BIT_EXT ==
                parseBitmask("VkDebugReportFlagBitsEXT", "VK_DEBUG_REPORT_DEBUG"));
        REQUIRE(VK_DEBUG_REPORT_ERROR_BIT_EXT ==
                parseBitmask("VkDebugReportFlagBitsEXT", "VK_DEBUG_REPORT_ERROR_BIT"));
        REQUIRE(VK_DEBUG_REPORT_ERROR_BIT_EXT ==
                parseBitmask("VkDebugReportFlagBitsEXT", "VK_DEBUG_REPORT_ERROR"));

        REQUIRE((VK_DEBUG_REPORT_DEBUG_BIT_EXT | VK_DEBUG_REPORT_ERROR_BIT_EXT) ==
                parseBitmask("VkDebugReportFlagBitsEXT",
                             "VK_DEBUG_REPORT_DEBUG_BIT | VK_DEBUG_REPORT_ERROR_BIT"));
        REQUIRE((VK_DEBUG_REPORT_DEBUG_BIT_EXT | VK_DEBUG_REPORT_ERROR_BIT_EXT) ==
                parseBitmask("VkDebugReportFlagBitsEXT",
                             "VK_DEBUG_REPORT_ERROR | VK_DEBUG_REPORT_DEBUG"));
    }
    SECTION("With mixed short/full strings") {
        REQUIRE((VK_DEBUG_REPORT_DEBUG_BIT_EXT | VK_DEBUG_REPORT_ERROR_BIT_EXT) ==
                parseBitmask("VkDebugReportFlagBitsEXT", "DEBUG_BIT | "
                                                         "VK_DEBUG_REPORT_ERROR_BIT"));
        REQUIRE((VK_DEBUG_REPORT_DEBUG_BIT_EXT | VK_DEBUG_REPORT_ERROR_BIT_EXT) ==
                parseBitmask("VkDebugReportFlagBitsEXT", "VK_DEBUG_REPORT_ERROR_BIT | DEBUG_BIT"));
    }
}