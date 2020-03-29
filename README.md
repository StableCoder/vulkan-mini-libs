# Vk Stringifier

[![pipeline status](https://git.stabletec.com/utilities/vulkan-stringifier/badges/master/pipeline.svg)](https://git.stabletec.com/utilities/vulkan-stringifier/commits/master)
[![coverage report](https://git.stabletec.com/utilities/vulkan-stringifier/badges/master/coverage.svg)](https://git.stabletec.com/utilities/vulkan-stringifier/commits/master)
[![license](https://img.shields.io/badge/license-Apache%202.0-blue.svg)](https://git.stabletec.com/utilities/vulkan-stringifier/blob/master/LICENSE)

This program builds a source/header file for use in C++17 or newer. It lists
contains all Vulkan enum types/flags/values of the indicated Vulkan header spec
version, and can convert to/from strings representing those values. 

Supports both plain enums and the bitmasks.

When converting values to strings, where possible a shorter version of the
enum string is used, where the verbose type prefix is removed:
- VK_IMAGE_LAYOUT_GENERAL => GENERAL
- VK_CULL_MODE_FRONT_BIT | VK_CULL_MODE_BACK_BIT => FRONT | BACK

When converting from strings into values, either the short OR full string can
be used where strings are case insensitive, and underscores can be replaced
with spaces, and addition whitespace can be added to either side of the first/
last alphanumeric character, as these are trimmed off.

For example, all of the following convert to VK_IMAGE_LAYOUT_GENERAL:
`vk imAGE_LayOut GenerAL`, `VK_IMAGE_LAYOUT_GENERAL`,`GENERAL`, `   General `

Also, to assist with forward and backwards compatability, all the vendor tags are stripped from the typenames and values, since they can be removed in later versions leading to incompatability issues. For example, the flag
for VkToolPurposeFlagBitsEXT, `VK_TOOL_PURPOSE_VALIDATION_BIT_EXT`, can will
be output as `VALIDATION_BIT`, and can be read similarly, with the above
rules applicable for de-stringifying.

## Program Arguments
### -h, --help
Help blurb
### -i, --input <file>
Input vk.xml file to parse. These can be found from the KhronosGroup, often at this repo: [https://github.com/KhronosGroup/Vulkan-Docs](https://github.com/KhronosGroup/Vulkan-Docs)
### -d, --dir <dir>
Output directory
### -o, --out <name>
Output file name, note that there will be both a .cpp and .hpp variant generated. (Default: `vk_enum_stringifier`)


## Generated Usage

Quite simply, use the version of the source that works best (can choose from pre-generated items from the pre-generated/ directory) and pair it with the common header file in the include/ directory into your application.


## Uses RapidXML from http://rapidxml.sourceforge.net/