include(prepare_catch.cmake)
include(code-coverage.cmake)

prepare_catch(COMPILED_CATCH)

find_package(Vulkan REQUIRED)

# Error Code
add_executable(VkErrorCodeTests error_code.cpp)
target_link_libraries(VkErrorCodeTests PRIVATE catch Vulkan::Vulkan)
target_code_coverage(VkErrorCodeTests EXCLUDE ".*/test/.*")

add_test(NAME VkErrorCodeTests-Tests COMMAND VkErrorCodeTests)

# Serialization
add_executable(VkSerializationTests parsing.cpp serialization.cpp)
target_link_libraries(VkSerializationTests PRIVATE catch Vulkan::Vulkan)
target_code_coverage(VkSerializationTests EXCLUDE ".*/test/.*")

add_test(NAME VkSerializationTests-Tests COMMAND VkSerializationTests)
