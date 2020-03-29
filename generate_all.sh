#!/usr/bin/env sh

set -e

# First, clone out the Vulkan-Docs repository
if ! [ -d Vulkan-Docs ]; then
    git clone https://github.com/KhronosGroup/Vulkan-Docs
fi

cd Vulkan-Docs
git fetch -p

for TAG in $(git tag | grep -e "^v[0-9]*\.[0-9]*\.[0-9]*$"); do
    VER=$(echo $TAG | cut -d'.' -f3)
    if [[ $VER -lt 72 ]]; then
        # Prior to v72, vk.xml was not published
        continue
    fi
    git checkout $TAG

    # Generate file
    ../build/VkStringifier -i xml/vk.xml -d ../pre-generated/value_stringifier/src/ -o vk_value_stringifier_v$VER.cpp

    # Format it
    clang-format -i ../pre-generated/value_stringifier/src/vk_value_stringifier_v$VER.cpp
done
