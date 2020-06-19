#!/usr/bin/env sh
set -e

# Clone/update the Vulkan-Docs repository
if ! [ -d Vulkan-Docs ]; then
    git clone https://github.com/KhronosGroup/Vulkan-Docs
fi
cd Vulkan-Docs
git fetch -p

# Prepare the top-level headers
mkdir -p ../include/vk_mini_libs_detail/
cat ../scripts/equality_check_start.txt >../include/vk_equality_checks.hpp
cat ../scripts/vulkan_string_parsing_start.txt >../include/vk_string_parsing.hpp

# Generate the per-version files
for TAG in $(git tag | grep -e "^v[0-9]*\.[0-9]*\.[0-9]*$"); do
    VER=$(echo $TAG | cut -d'.' -f3)
    if [[ $VER -lt 72 ]]; then
        # Prior to v72, vk.xml was not published
        continue
    fi
    git checkout $TAG

    # Generate stringifier
    ../build/VkStringifier -i xml/vk.xml -d ../include/vk_mini_libs_detail/ -o vk_string_parsing_v$VER.hpp

    cat >>../include/vk_string_parsing.hpp <<EOL
#if VK_HEADER_VERSION == ${VER}
    #include "vk_mini_libs_detail/vk_string_parsing_v${VER}.hpp"
#endif
EOL

    # Generate equality checks
    ../build/VkEqualityCheck -i xml/vk.xml -d ../include/vk_mini_libs_detail/ -o vk_equality_checks_v$VER.hpp

    cat >>../include/vk_equality_checks.hpp <<EOL
#if VK_HEADER_VERSION == ${VER}
    #include "vk_mini_libs_detail/vk_equality_checks_v${VER}.hpp"
#endif
EOL

done

# Complete the top-level headers
cat ../scripts/equality_check_end.txt >>../include/vk_equality_checks.hpp
cat ../scripts/vulkan_string_parsing_end.txt >>../include/vk_string_parsing.hpp
