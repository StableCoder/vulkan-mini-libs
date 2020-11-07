include(prepare_catch.cmake)
include(code-coverage.cmake)

prepare_catch(COMPILED_CATCH)

find_package(Vulkan REQUIRED)

include_directories(../include)
link_libraries(catch Vulkan::Vulkan)

# Equality Checks
add_executable(VkEqualityCheckTests equality_checks.cpp)

add_test(NAME VkEqualityCheckTests-Tests COMMAND VkEqualityCheckTests)

# Error Code
add_executable(VkErrorCodeTests error_code.cpp)
target_code_coverage(VkErrorCodeTests EXCLUDE ".*/test/.*")

add_test(NAME VkErrorCodeTests-Tests COMMAND VkErrorCodeTests)

# Serialization
add_executable(VkSerializationTests parsing.cpp serialization.cpp)
target_code_coverage(VkSerializationTests EXCLUDE ".*/test/.*")

add_test(NAME VkSerializationTests-Tests COMMAND VkSerializationTests)
