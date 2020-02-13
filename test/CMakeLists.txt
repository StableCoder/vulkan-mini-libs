include(prepare_catch.cmake)
include(code_coverage.cmake)

prepare_catch(COMPILED_CATCH)

add_executable(VkEnumConverterTests ../pre-generated/vk_enum_converter_v132.cpp
                                    to_string.cpp from_string.cpp)
target_include_directories(VkEnumConverterTests PRIVATE ../pre-generated)
target_link_libraries(VkEnumConverterTests PRIVATE catch)
target_code_coverage(VkEnumConverterTests EXCLUDE ".*/test/.*")

add_test(NAME VkEnumConverter-Tests COMMAND VkEnumConverterTests)
