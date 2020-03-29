cmake_minimum_required(VERSION 3.2)
project(vulkan-stringifier)

include(../vulkan_header_version.cmake)

get_vulkan_header_version()

add_library(
  vk_value_stringifier
  value_stringifier/src/vk_value_stringifier_v${VULKAN_HEADER_VERSION}.cpp)
target_include_directories(vk_value_stringifier
                           PUBLIC value_stringifier/include)
