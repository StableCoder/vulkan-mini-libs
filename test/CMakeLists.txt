include(prepare_catch.cmake)
include(code_coverage.cmake)

prepare_catch(COMPILED_CATCH)

find_package(Vulkan REQUIRED)

add_executable(
  VkEnumStringifierTests ../pre-generated/vk_enum_stringifier_v133.cpp
                         to_string.cpp from_string.cpp)
target_include_directories(VkEnumStringifierTests
                           PRIVATE ../include ${Vulkan_INCLUDE_DIRS})
target_link_libraries(VkEnumStringifierTests PRIVATE catch ${Vulkan_LIBRARY})
target_code_coverage(VkEnumStringifierTests EXCLUDE ".*/test/.*")

add_test(NAME VkEnumStringifier-Tests COMMAND VkEnumStringifierTests)
