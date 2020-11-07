cmake_minimum_required(VERSION 3.2)
project(VkEnumStringifier)

option(BUILD_EXAMPLES "Build examples" OFF)
option(BUILD_TESTS "Build tests" OFF)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_executable(VkErrorCode src/error_code.cpp)
target_include_directories(VkErrorCode PRIVATE external)

add_executable(VkValueSerialization src/value_serialization.cpp)
target_include_directories(VkValueSerialization PRIVATE external)

# add_executable(VkYamlGenerator src/yaml_generator.cpp)
# target_include_directories(VkYamlGenerator PRIVATE external)

add_executable(VkEqualityCheck src/equality_check.cpp)
target_include_directories(VkEqualityCheck PRIVATE external)

if(BUILD_EXAMPLES)
  add_subdirectory(example)
endif()
if(BUILD_TESTS)
  enable_testing()
  add_subdirectory(test)
endif()
