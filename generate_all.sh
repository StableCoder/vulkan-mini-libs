#!/usr/bin/env sh

set -e

# Clone/update the Vulkan-Docs repository
if ! [ -d Vulkan-Docs ]; then
    git clone https://github.com/KhronosGroup/Vulkan-Docs
fi
cd Vulkan-Docs
git fetch -p

# Prepare the top-level headers
cat ../scripts/equality_check_start.txt >../generated_include/vk_equality_checks.hpp

# Generate the per-version files
for TAG in $(git tag | grep -e "^v[0-9]*\.[0-9]*\.[0-9]*$"); do
    VER=$(echo $TAG | cut -d'.' -f3)
    if [[ $VER -lt 72 ]]; then
        # Prior to v72, vk.xml was not published
        continue
    fi
    git checkout $TAG

    # Generate stringifier
    ../build/VkStringifier -i xml/vk.xml -d ../pre-generated/value_stringifier/src/ -o vk_value_stringifier_v$VER.cpp

    # Format it
    clang-format -i ../pre-generated/value_stringifier/src/vk_value_stringifier_v$VER.cpp

    # Generate equality checks
    ../build/VkEqualityCheck -i xml/vk.xml -d ../generated_include/equality_check_detail/ -o vk_equality_checks_v$VER.hpp

    cat >>../generated_include/vk_equality_checks.hpp <<EOL
#if VK_HEADER_VERSION == ${VER}
    #include <equality_check_detail/vk_equality_checks_v${VER}.hpp>
#endif
EOL

done

# Complete the top-level headers
cat ../scripts/equality_check_end.txt >>../generated_include/vk_equality_checks.hpp
