include(prepare_catch.cmake)
include(code_coverage.cmake)

prepare_catch(COMPILED_CATCH)

add_executable(
  VkEnumStringifierTests ../pre-generated/vk_enum_stringifier_v132.cpp
                         to_string.cpp from_string.cpp)
target_include_directories(VkEnumStringifierTests PRIVATE ../pre-generated)
target_link_libraries(VkEnumStringifierTests PRIVATE catch)
target_code_coverage(VkEnumStringifierTests EXCLUDE ".*/test/.*")

add_test(NAME VkEnumStringifier-Tests COMMAND VkEnumStringifierTests)
