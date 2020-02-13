cmake_minimum_required(VERSION 3.2)
project(VkEnumConverter)

option(BUILD_TESTS "" OFF)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_executable(VkEnumConverter src/vk_enum_converter.cpp)
target_include_directories(VkEnumConverter PRIVATE external)

if(BUILD_TESTS)
  enable_testing()
  add_subdirectory(test)
endif()
