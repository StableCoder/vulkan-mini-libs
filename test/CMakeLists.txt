include(prepare_catch.cmake)
include(code_coverage.cmake)

prepare_catch(COMPILED_CATCH)

find_package(Vulkan REQUIRED)

add_executable(
  VkStringifierTests
  ../pre-generated/value_stringifier/src/vk_value_stringifier_v133.cpp
  to_string.cpp from_string.cpp)
target_include_directories(VkStringifierTests
                           PRIVATE ../pre-generated/value_stringifier/include)
target_link_libraries(VkStringifierTests PRIVATE catch Vulkan::Vulkan)
target_code_coverage(VkStringifierTests EXCLUDE ".*/test/.*")

add_test(NAME VkStringifier-Tests COMMAND VkStringifierTests)
