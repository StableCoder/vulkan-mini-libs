cmake_minimum_required(VERSION 3.2)
project(VkEnumStringifier)

option(BUILD_TESTS "" OFF)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_executable(VkEnumStringifier src/vk_enum_stringifier.cpp)
target_include_directories(VkEnumStringifier PRIVATE external)

if(BUILD_TESTS)
  enable_testing()
  add_subdirectory(test)
endif()
