#!/usr/bin/env sh

set -e

# First, clone out the Vulkan-Docs repository
if ! [ -d auto-gen ]; then
    git clone https://github.com/KhronosGroup/Vulkan-Docs auto-gen
fi

cd auto-gen
git fetch -p

for TAG in $(git tag | grep -e "^v[0-9]*\.[0-9]*\.[0-9]*$"); do
    VER=$(echo $TAG | cut -d'.' -f3)
    if [[ $VER -lt 72 ]]; then
        # Prior to v72, vk.xml was not published
        continue
    fi
    git checkout $TAG

    # Generate file
    VkEnumStringifier -i xml/vk.xml -d ../pre-generated -o vk_enum_stringifier_v$VER.cpp

    # Format it
    clang-format -i ../pre-generated/vk_enum_stringifier_v$VER.cpp
done
