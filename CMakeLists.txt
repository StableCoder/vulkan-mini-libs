cmake_minimum_required(VERSION 3.2)
project(VkEnumStringifier)

option(BUILD_TESTS "" OFF)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_subdirectory(pre-generated)

add_executable(VkStringifier src/vk_stringifier.cpp)
target_include_directories(VkStringifier PRIVATE external)

add_executable(VkYamlGenerator src/yaml_generator.cpp)
target_include_directories(VkYamlGenerator PRIVATE external)

add_executable(VkEqualityCheck src/equality_check.cpp)
target_include_directories(VkEqualityCheck PRIVATE external)

if(BUILD_TESTS)
  enable_testing()
  add_subdirectory(test)
endif()
