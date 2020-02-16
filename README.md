# Vk Enum Stringifier

This program builds a source/header file for use in C++17 or newer. It lists
contains all Vulkan enum types/flags/values of the indicated Vulkan header spec
version, and can convert to/from strings representing those values. 

Supports both plain enums and the bitmasks.

When converting values to strings, where possible a shorter version of the
enum string is used, where the verbose type prefix is removed:
    VK_IMAGE_LAYOUT_GENERAL => GENERAL
    VK_CULL_MODE_FRONT_BIT | VK_CULL_MODE_BACK_BIT => FRONT | BACK

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


## Uses RapidXML from http://rapidxml.sourceforge.net/