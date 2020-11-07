/*
    Copyright (C) 2020 George Cave - gcave@stablecoder.ca

    Licensed under the Apache License, Version 2.0 (the "License");
    you may not use this file except in compliance with the License.
    You may obtain a copy of the License at

       http://www.apache.org/licenses/LICENSE-2.0

    Unless CHECKd by applicable law or agreed to in writing, software
    distributed under the License is distributed on an "AS IS" BASIS,
    WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
    See the License for the specific language governing permissions and
    limitations under the License.
*/

#include <catch.hpp>
#include <vulkan/vulkan.h>

#define VK_EQUALITY_CHECK_CONFIG_MAIN
#include "vk_equality_checks.hpp"

TEST_CASE("VkApplicationInfo - strcmp for null-terminated data") {
    SECTION("Empty nullptr data") {
        VkApplicationInfo test1{};
        VkApplicationInfo test2{};

        REQUIRE(test1 == test2);
        REQUIRE_FALSE(test1 != test2);
    }
    SECTION("One having real data") {
        VkApplicationInfo test1{};
        VkApplicationInfo test2{
            .pApplicationName = "lol",
        };

        REQUIRE_FALSE(test1 == test2);
        REQUIRE(test1 != test2);
    }
    SECTION("Both pointing to the same pointer/data") {
        char const *pStr = "lol";
        VkApplicationInfo test1{
            .pApplicationName = pStr,
        };
        VkApplicationInfo test2{
            .pApplicationName = pStr,
        };

        REQUIRE(test1 == test2);
        REQUIRE_FALSE(test1 != test2);
    }

    SECTION("Both pointing to the same data string but at different pointers") {
        std::string str1 = "lol";
        std::string str2 = "lol";
        VkApplicationInfo test1{
            .pApplicationName = str1.data(),
        };
        VkApplicationInfo test2{
            .pApplicationName = str2.data(),
        };

        REQUIRE(test1 == test2);
        REQUIRE_FALSE(test1 != test2);
    }
}

TEST_CASE("VkSpecializationInfo - memcmp for raw data") {
    std::string str1 = "lol";
    std::string str2 = "lol";
    std::string str3 = "lel";

    SECTION("Both nullptr") {
        VkSpecializationInfo test1{};
        VkSpecializationInfo test2{};

        REQUIRE(test1 == test2);
        REQUIRE_FALSE(test1 != test2);
    }
    SECTION("Different data sizes") {
        VkSpecializationInfo test1{.dataSize = 3};
        VkSpecializationInfo test2{.dataSize = 4};

        REQUIRE_FALSE(test1 == test2);
        REQUIRE(test1 != test2);
    }
    SECTION("Same data / pointer") {
        VkSpecializationInfo test1{.dataSize = str1.size(), .pData = str1.data()};
        VkSpecializationInfo test2{.dataSize = str1.size(), .pData = str1.data()};

        REQUIRE(test1 == test2);
        REQUIRE_FALSE(test1 != test2);
    }
    SECTION("Same data / different pointers") {
        VkSpecializationInfo test1{.dataSize = str1.size(), .pData = str1.data()};
        VkSpecializationInfo test2{.dataSize = str2.size(), .pData = str2.data()};

        REQUIRE(test1 == test2);
        REQUIRE_FALSE(test1 != test2);
    }
    SECTION("Different data / different pointers") {
        VkSpecializationInfo test1{.dataSize = str1.size(), .pData = str1.data()};
        VkSpecializationInfo test2{.dataSize = str3.size(), .pData = str3.data()};

        REQUIRE_FALSE(test1 == test2);
        REQUIRE(test1 != test2);
    }
}

TEST_CASE("VkPipelineViewportStateCreateInfo - Checking variable data arrays") {
    std::array<VkViewport, 2> data1{VkViewport{.x = 1}, VkViewport{.x = 3}};
    std::array<VkViewport, 2> data2{VkViewport{.x = 1}, VkViewport{.x = 3}};
    std::array<VkViewport, 3> data3{VkViewport{.x = 2}, VkViewport{.x = 3}};

    SECTION("Zero-sized arrays") {
        VkPipelineViewportStateCreateInfo test1{};
        VkPipelineViewportStateCreateInfo test2{};

        REQUIRE(test1 == test2);
        REQUIRE_FALSE(test1 != test2);
    }
    SECTION("Differently-sized arrays") {
        VkPipelineViewportStateCreateInfo test1{.viewportCount = 1};
        VkPipelineViewportStateCreateInfo test2{.viewportCount = 2};

        REQUIRE_FALSE(test1 == test2);
        REQUIRE(test1 != test2);
    }
    SECTION("Same data") {
        VkPipelineViewportStateCreateInfo test1{.viewportCount = data1.size(),
                                                .pViewports = data1.data()};
        VkPipelineViewportStateCreateInfo test2{.viewportCount = data2.size(),
                                                .pViewports = data2.data()};

        REQUIRE(test1 == test2);
        REQUIRE_FALSE(test1 != test2);
    }

    SECTION("Different data") {
        VkPipelineViewportStateCreateInfo test1{.viewportCount = data1.size(),
                                                .pViewports = data1.data()};
        VkPipelineViewportStateCreateInfo test2{.viewportCount = data3.size(),
                                                .pViewports = data3.data()};

        REQUIRE_FALSE(test1 == test2);
        REQUIRE(test1 != test2);
    }
}