include(prepare_catch.cmake)
include(code_coverage.cmake)

prepare_catch(COMPILED_CATCH)

find_package(Vulkan REQUIRED)

add_executable(VkStringifierTests to_string.cpp from_string.cpp)
target_link_libraries(VkStringifierTests PRIVATE catch vk_value_stringifier
                                                 Vulkan::Vulkan)
target_code_coverage(VkStringifierTests EXCLUDE ".*/test/.*")

add_test(NAME VkStringifier-Tests COMMAND VkStringifierTests)
