## WARNING!

This repository has been superseeded by Vulkan-Mini-Libs-2, which compacts all Vulkan versions into single header files instead of the large multitude here! It can be found on GitHub @ https://github.com/StableCoder/vulkan-mini-libs-2

# Vulkan Mini Libs
[![pipeline status](https://git.stabletec.com/utilities/vulkan-mini-libs/badges/main/pipeline.svg)](https://git.stabletec.com/utilities/vulkan-mini-libs/commits/main)
[![coverage report](https://git.stabletec.com/utilities/vulkan-mini-libs/badges/main/coverage.svg)](https://git.stabletec.com/utilities/vulkan-mini-libs/commits/main)
[![license](https://img.shields.io/badge/license-Apache%202.0-blue.svg)](https://git.stabletec.com/utilities/vulkan-mini-libs/blob/main/LICENSE)

A set of small header-only libraries that are of limited scope each to perform a very specific task.


## Vulkan Error Code

Header files for C++. Contains the implementation details that allow the use of VkResult values with std::error_code and std::error_category.

### Header Usage

To use, include the header where the error code is being used.

On *ONE* compilation unit, include the definition of `#define VK_ERROR_CODE_CONFIG_MAIN` so that the definitions are compiled somewhere following the one definition rule.

### VkEqualityChecks header-generation program arguments
#### -h, --help
Help blurb
#### -i, --input <file>
Input vk.xml file to parse. These can be found from the KhronosGroup, often at this repo: [https://github.com/KhronosGroup/Vulkan-Docs](https://github.com/KhronosGroup/Vulkan-Docs)
#### -d, --dir <dir>
Output directory
#### -o, --out <name>
Output file name (Default: `vk_error_code.hpp`)

## Vulkan Value Serialization

This program builds header files for use in C++17 or newer. It
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

Also, to assist with forward and backwards compatability, all the vendor tags are 
stripped from the typenames and values, since they can be removed in later versions 
leading to incompatability issues. For example, the flag for VkToolPurposeFlagBitsEXT, 
`VK_TOOL_PURPOSE_VALIDATION_BIT_EXT`, can will be output as `VALIDATION_BIT`, and can 
be read similarly, with the above rules applicable for parsing.

### Header Usage

To use, include the header where the declarations for the boolean checks are required.

On *ONE* compilation unit, include the definition of `#define VK_VALUE_SERIALIZATION_CONFIG_MAIN` so that the definitions are compiled somewhere following the one definition rule.

### VkValueSerialization header-generation program arguments
#### -h, --help
Help blurb
#### -i, --input <file>
Input vk.xml file to parse. These can be found from the KhronosGroup, often at this repo: [https://github.com/KhronosGroup/Vulkan-Docs](https://github.com/KhronosGroup/Vulkan-Docs)
#### -d, --dir <dir>
Output directory
#### -o, --out <name>
Output file name (Default: `vk_value_serialization.hpp`)

## Vulkan Equality Checks

Header files for C++. Contains `operator==` and `operator!=` functions for a large set of the available Vulkan struct types, checking just the objects for equality. These do *NOT* peroform a deep comparison, such as any objects pointed to by `pNext` or any other pointed-to objects.

### Header Usage

To use, include the header where the declarations for the boolean checks are required.

On *ONE* compilation unit, include the definition of `#define VK_EQUALITY_CHECK_CONFIG_MAIN` so that the definitions are compiled somewhere following the one definition rule.

### VkEqualityChecks header-generation program arguments
#### -h, --help
Help blurb
#### -i, --input <file>
Input vk.xml file to parse. These can be found from the KhronosGroup, often at this repo: [https://github.com/KhronosGroup/Vulkan-Docs](https://github.com/KhronosGroup/Vulkan-Docs)
#### -d, --dir <dir>
Output directory
#### -o, --out <name>
Output file name (Default: `vk_equality_checks.hpp`)

## Generating Header Mini-Libs

In the root of the repository is a shell script, `generate.sh` that, when the programs are built from cmake and also placed in the root, automatically generates header mini-libs. The desired range can also be specified with this.

### Possible Arguments

#### -s, --start <INT>
The starting version of Vulkan to generate for (default: 72)

#### -e, --end <INT>
The ending version of Vulkan to generate for (default: none)

## Utilizes RapidXML from http://rapidxml.sourceforge.net/