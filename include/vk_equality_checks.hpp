/*
    Copyright (C) 2020 George Cave - gcave@stablecoder.ca

    Licensed under the Apache License, Version 2.0 (the "License");
    you may not use this file except in compliance with the License.
    You may obtain a copy of the License at

       http://www.apache.org/licenses/LICENSE-2.0

    Unless required by applicable law or agreed to in writing, software
    distributed under the License is distributed on an "AS IS" BASIS,
    WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
    See the License for the specific language governing permissions and
    limitations under the License.
*/

/*
    This file was auto-generated by the Vulkan mini-libs utility can be found at
    https://github.com/stablecoder/vulkan-mini-libs.git
    or
    https://git.stabletec.com/utilities/vulkan-mini-libs.git

    Check for an updated version anytime, or state concerns/bugs.
*/

#ifndef VK_EQUALITY_CHECKS_HPP
#define VK_EQUALITY_CHECKS_HPP

/*  USAGE
    To use, include this header where the declarations for the boolean checks are required.

    On *ONE* compilation unit, include the definition of `#define VK_EQUALITY_CHECK_CONFIG_MAIN`
    so that the definitions are compiled somewhere following the one definition rule.
*/

#include <vulkan/vulkan.h>

// Delegate to header specific to the local Vulkan header version
#if VK_HEADER_VERSION == 146
    #include "vk_mini_libs_detail/vk_equality_checks_v146.hpp"
#endif
#if VK_HEADER_VERSION == 145
    #include "vk_mini_libs_detail/vk_equality_checks_v145.hpp"
#endif
#if VK_HEADER_VERSION == 144
    #include "vk_mini_libs_detail/vk_equality_checks_v144.hpp"
#endif
#if VK_HEADER_VERSION == 143
    #include "vk_mini_libs_detail/vk_equality_checks_v143.hpp"
#endif
#if VK_HEADER_VERSION == 142
    #include "vk_mini_libs_detail/vk_equality_checks_v142.hpp"
#endif
#if VK_HEADER_VERSION == 141
    #include "vk_mini_libs_detail/vk_equality_checks_v141.hpp"
#endif
#if VK_HEADER_VERSION == 140
    #include "vk_mini_libs_detail/vk_equality_checks_v140.hpp"
#endif
#if VK_HEADER_VERSION == 139
    #include "vk_mini_libs_detail/vk_equality_checks_v139.hpp"
#endif
#if VK_HEADER_VERSION == 138
    #include "vk_mini_libs_detail/vk_equality_checks_v138.hpp"
#endif
#if VK_HEADER_VERSION == 137
    #include "vk_mini_libs_detail/vk_equality_checks_v137.hpp"
#endif
#if VK_HEADER_VERSION == 136
    #include "vk_mini_libs_detail/vk_equality_checks_v136.hpp"
#endif
#if VK_HEADER_VERSION == 135
    #include "vk_mini_libs_detail/vk_equality_checks_v135.hpp"
#endif
#if VK_HEADER_VERSION == 134
    #include "vk_mini_libs_detail/vk_equality_checks_v134.hpp"
#endif
#if VK_HEADER_VERSION == 133
    #include "vk_mini_libs_detail/vk_equality_checks_v133.hpp"
#endif
#if VK_HEADER_VERSION == 132
    #include "vk_mini_libs_detail/vk_equality_checks_v132.hpp"
#endif
#if VK_HEADER_VERSION == 131
    #include "vk_mini_libs_detail/vk_equality_checks_v131.hpp"
#endif
#if VK_HEADER_VERSION == 130
    #include "vk_mini_libs_detail/vk_equality_checks_v130.hpp"
#endif
#if VK_HEADER_VERSION == 129
    #include "vk_mini_libs_detail/vk_equality_checks_v129.hpp"
#endif
#if VK_HEADER_VERSION == 128
    #include "vk_mini_libs_detail/vk_equality_checks_v128.hpp"
#endif
#if VK_HEADER_VERSION == 127
    #include "vk_mini_libs_detail/vk_equality_checks_v127.hpp"
#endif
#if VK_HEADER_VERSION == 126
    #include "vk_mini_libs_detail/vk_equality_checks_v126.hpp"
#endif
#if VK_HEADER_VERSION == 125
    #include "vk_mini_libs_detail/vk_equality_checks_v125.hpp"
#endif
#if VK_HEADER_VERSION == 124
    #include "vk_mini_libs_detail/vk_equality_checks_v124.hpp"
#endif
#if VK_HEADER_VERSION == 123
    #include "vk_mini_libs_detail/vk_equality_checks_v123.hpp"
#endif
#if VK_HEADER_VERSION == 122
    #include "vk_mini_libs_detail/vk_equality_checks_v122.hpp"
#endif
#if VK_HEADER_VERSION == 121
    #include "vk_mini_libs_detail/vk_equality_checks_v121.hpp"
#endif
#if VK_HEADER_VERSION == 120
    #include "vk_mini_libs_detail/vk_equality_checks_v120.hpp"
#endif
#if VK_HEADER_VERSION == 119
    #include "vk_mini_libs_detail/vk_equality_checks_v119.hpp"
#endif
#if VK_HEADER_VERSION == 118
    #include "vk_mini_libs_detail/vk_equality_checks_v118.hpp"
#endif
#if VK_HEADER_VERSION == 117
    #include "vk_mini_libs_detail/vk_equality_checks_v117.hpp"
#endif
#if VK_HEADER_VERSION == 116
    #include "vk_mini_libs_detail/vk_equality_checks_v116.hpp"
#endif
#if VK_HEADER_VERSION == 115
    #include "vk_mini_libs_detail/vk_equality_checks_v115.hpp"
#endif
#if VK_HEADER_VERSION == 114
    #include "vk_mini_libs_detail/vk_equality_checks_v114.hpp"
#endif
#if VK_HEADER_VERSION == 113
    #include "vk_mini_libs_detail/vk_equality_checks_v113.hpp"
#endif
#if VK_HEADER_VERSION == 112
    #include "vk_mini_libs_detail/vk_equality_checks_v112.hpp"
#endif
#if VK_HEADER_VERSION == 111
    #include "vk_mini_libs_detail/vk_equality_checks_v111.hpp"
#endif
#if VK_HEADER_VERSION == 110
    #include "vk_mini_libs_detail/vk_equality_checks_v110.hpp"
#endif
#if VK_HEADER_VERSION == 109
    #include "vk_mini_libs_detail/vk_equality_checks_v109.hpp"
#endif
#if VK_HEADER_VERSION == 108
    #include "vk_mini_libs_detail/vk_equality_checks_v108.hpp"
#endif
#if VK_HEADER_VERSION == 107
    #include "vk_mini_libs_detail/vk_equality_checks_v107.hpp"
#endif
#if VK_HEADER_VERSION == 106
    #include "vk_mini_libs_detail/vk_equality_checks_v106.hpp"
#endif
#if VK_HEADER_VERSION == 105
    #include "vk_mini_libs_detail/vk_equality_checks_v105.hpp"
#endif
#if VK_HEADER_VERSION == 104
    #include "vk_mini_libs_detail/vk_equality_checks_v104.hpp"
#endif
#if VK_HEADER_VERSION == 103
    #include "vk_mini_libs_detail/vk_equality_checks_v103.hpp"
#endif
#if VK_HEADER_VERSION == 102
    #include "vk_mini_libs_detail/vk_equality_checks_v102.hpp"
#endif
#if VK_HEADER_VERSION == 101
    #include "vk_mini_libs_detail/vk_equality_checks_v101.hpp"
#endif
#if VK_HEADER_VERSION == 100
    #include "vk_mini_libs_detail/vk_equality_checks_v100.hpp"
#endif
#if VK_HEADER_VERSION == 99
    #include "vk_mini_libs_detail/vk_equality_checks_v99.hpp"
#endif
#if VK_HEADER_VERSION == 98
    #include "vk_mini_libs_detail/vk_equality_checks_v98.hpp"
#endif
#if VK_HEADER_VERSION == 97
    #include "vk_mini_libs_detail/vk_equality_checks_v97.hpp"
#endif
#if VK_HEADER_VERSION == 96
    #include "vk_mini_libs_detail/vk_equality_checks_v96.hpp"
#endif
#if VK_HEADER_VERSION == 95
    #include "vk_mini_libs_detail/vk_equality_checks_v95.hpp"
#endif
#if VK_HEADER_VERSION == 94
    #include "vk_mini_libs_detail/vk_equality_checks_v94.hpp"
#endif
#if VK_HEADER_VERSION == 93
    #include "vk_mini_libs_detail/vk_equality_checks_v93.hpp"
#endif
#if VK_HEADER_VERSION == 92
    #include "vk_mini_libs_detail/vk_equality_checks_v92.hpp"
#endif
#if VK_HEADER_VERSION == 91
    #include "vk_mini_libs_detail/vk_equality_checks_v91.hpp"
#endif
#if VK_HEADER_VERSION == 90
    #include "vk_mini_libs_detail/vk_equality_checks_v90.hpp"
#endif
#if VK_HEADER_VERSION == 89
    #include "vk_mini_libs_detail/vk_equality_checks_v89.hpp"
#endif
#if VK_HEADER_VERSION == 88
    #include "vk_mini_libs_detail/vk_equality_checks_v88.hpp"
#endif
#if VK_HEADER_VERSION == 87
    #include "vk_mini_libs_detail/vk_equality_checks_v87.hpp"
#endif
#if VK_HEADER_VERSION == 86
    #include "vk_mini_libs_detail/vk_equality_checks_v86.hpp"
#endif
#if VK_HEADER_VERSION == 85
    #include "vk_mini_libs_detail/vk_equality_checks_v85.hpp"
#endif
#if VK_HEADER_VERSION == 84
    #include "vk_mini_libs_detail/vk_equality_checks_v84.hpp"
#endif
#if VK_HEADER_VERSION == 83
    #include "vk_mini_libs_detail/vk_equality_checks_v83.hpp"
#endif
#if VK_HEADER_VERSION == 82
    #include "vk_mini_libs_detail/vk_equality_checks_v82.hpp"
#endif
#if VK_HEADER_VERSION == 81
    #include "vk_mini_libs_detail/vk_equality_checks_v81.hpp"
#endif
#if VK_HEADER_VERSION == 80
    #include "vk_mini_libs_detail/vk_equality_checks_v80.hpp"
#endif
#if VK_HEADER_VERSION == 79
    #include "vk_mini_libs_detail/vk_equality_checks_v79.hpp"
#endif
#if VK_HEADER_VERSION == 78
    #include "vk_mini_libs_detail/vk_equality_checks_v78.hpp"
#endif
#if VK_HEADER_VERSION == 77
    #include "vk_mini_libs_detail/vk_equality_checks_v77.hpp"
#endif
#if VK_HEADER_VERSION == 76
    #include "vk_mini_libs_detail/vk_equality_checks_v76.hpp"
#endif
#if VK_HEADER_VERSION == 75
    #include "vk_mini_libs_detail/vk_equality_checks_v75.hpp"
#endif
#if VK_HEADER_VERSION == 74
    #include "vk_mini_libs_detail/vk_equality_checks_v74.hpp"
#endif
#if VK_HEADER_VERSION == 73
    #include "vk_mini_libs_detail/vk_equality_checks_v73.hpp"
#endif
#if VK_HEADER_VERSION == 72
    #include "vk_mini_libs_detail/vk_equality_checks_v72.hpp"
#endif

#endif // VK_EQUALITY_CHECKS_HPP
