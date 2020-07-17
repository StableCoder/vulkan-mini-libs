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

#ifndef VK_ERROR_CODE_HPP
#define VK_ERROR_CODE_HPP

/*  USAGE
    To use, include this header where the declarations for the boolean checks are required.

    On *ONE* compilation unit, include the definition of `#define VK_ERROR_CODE_CONFIG_MAIN`
    so that the definitions are compiled somewhere following the one definition rule.
*/

#include <vulkan/vulkan.h>

// Delegate to header specific to the local Vulkan header version
#if VK_HEADER_VERSION == 147
    #include "detail_error_code/vk_error_code_v147.hpp"
#endif
#if VK_HEADER_VERSION == 146
    #include "detail_error_code/vk_error_code_v146.hpp"
#endif
#if VK_HEADER_VERSION == 145
    #include "detail_error_code/vk_error_code_v145.hpp"
#endif
#if VK_HEADER_VERSION == 144
    #include "detail_error_code/vk_error_code_v144.hpp"
#endif
#if VK_HEADER_VERSION == 143
    #include "detail_error_code/vk_error_code_v143.hpp"
#endif
#if VK_HEADER_VERSION == 142
    #include "detail_error_code/vk_error_code_v142.hpp"
#endif
#if VK_HEADER_VERSION == 141
    #include "detail_error_code/vk_error_code_v141.hpp"
#endif
#if VK_HEADER_VERSION == 140
    #include "detail_error_code/vk_error_code_v140.hpp"
#endif
#if VK_HEADER_VERSION == 139
    #include "detail_error_code/vk_error_code_v139.hpp"
#endif
#if VK_HEADER_VERSION == 138
    #include "detail_error_code/vk_error_code_v138.hpp"
#endif
#if VK_HEADER_VERSION == 137
    #include "detail_error_code/vk_error_code_v137.hpp"
#endif
#if VK_HEADER_VERSION == 136
    #include "detail_error_code/vk_error_code_v136.hpp"
#endif
#if VK_HEADER_VERSION == 135
    #include "detail_error_code/vk_error_code_v135.hpp"
#endif
#if VK_HEADER_VERSION == 134
    #include "detail_error_code/vk_error_code_v134.hpp"
#endif
#if VK_HEADER_VERSION == 133
    #include "detail_error_code/vk_error_code_v133.hpp"
#endif
#if VK_HEADER_VERSION == 132
    #include "detail_error_code/vk_error_code_v132.hpp"
#endif
#if VK_HEADER_VERSION == 131
    #include "detail_error_code/vk_error_code_v131.hpp"
#endif
#if VK_HEADER_VERSION == 130
    #include "detail_error_code/vk_error_code_v130.hpp"
#endif
#if VK_HEADER_VERSION == 129
    #include "detail_error_code/vk_error_code_v129.hpp"
#endif
#if VK_HEADER_VERSION == 128
    #include "detail_error_code/vk_error_code_v128.hpp"
#endif
#if VK_HEADER_VERSION == 127
    #include "detail_error_code/vk_error_code_v127.hpp"
#endif
#if VK_HEADER_VERSION == 126
    #include "detail_error_code/vk_error_code_v126.hpp"
#endif
#if VK_HEADER_VERSION == 125
    #include "detail_error_code/vk_error_code_v125.hpp"
#endif
#if VK_HEADER_VERSION == 124
    #include "detail_error_code/vk_error_code_v124.hpp"
#endif
#if VK_HEADER_VERSION == 123
    #include "detail_error_code/vk_error_code_v123.hpp"
#endif
#if VK_HEADER_VERSION == 122
    #include "detail_error_code/vk_error_code_v122.hpp"
#endif
#if VK_HEADER_VERSION == 121
    #include "detail_error_code/vk_error_code_v121.hpp"
#endif
#if VK_HEADER_VERSION == 120
    #include "detail_error_code/vk_error_code_v120.hpp"
#endif
#if VK_HEADER_VERSION == 119
    #include "detail_error_code/vk_error_code_v119.hpp"
#endif
#if VK_HEADER_VERSION == 118
    #include "detail_error_code/vk_error_code_v118.hpp"
#endif
#if VK_HEADER_VERSION == 117
    #include "detail_error_code/vk_error_code_v117.hpp"
#endif
#if VK_HEADER_VERSION == 116
    #include "detail_error_code/vk_error_code_v116.hpp"
#endif
#if VK_HEADER_VERSION == 115
    #include "detail_error_code/vk_error_code_v115.hpp"
#endif
#if VK_HEADER_VERSION == 114
    #include "detail_error_code/vk_error_code_v114.hpp"
#endif
#if VK_HEADER_VERSION == 113
    #include "detail_error_code/vk_error_code_v113.hpp"
#endif
#if VK_HEADER_VERSION == 112
    #include "detail_error_code/vk_error_code_v112.hpp"
#endif
#if VK_HEADER_VERSION == 111
    #include "detail_error_code/vk_error_code_v111.hpp"
#endif
#if VK_HEADER_VERSION == 110
    #include "detail_error_code/vk_error_code_v110.hpp"
#endif
#if VK_HEADER_VERSION == 109
    #include "detail_error_code/vk_error_code_v109.hpp"
#endif
#if VK_HEADER_VERSION == 108
    #include "detail_error_code/vk_error_code_v108.hpp"
#endif
#if VK_HEADER_VERSION == 107
    #include "detail_error_code/vk_error_code_v107.hpp"
#endif
#if VK_HEADER_VERSION == 106
    #include "detail_error_code/vk_error_code_v106.hpp"
#endif
#if VK_HEADER_VERSION == 105
    #include "detail_error_code/vk_error_code_v105.hpp"
#endif
#if VK_HEADER_VERSION == 104
    #include "detail_error_code/vk_error_code_v104.hpp"
#endif
#if VK_HEADER_VERSION == 103
    #include "detail_error_code/vk_error_code_v103.hpp"
#endif
#if VK_HEADER_VERSION == 102
    #include "detail_error_code/vk_error_code_v102.hpp"
#endif
#if VK_HEADER_VERSION == 101
    #include "detail_error_code/vk_error_code_v101.hpp"
#endif
#if VK_HEADER_VERSION == 100
    #include "detail_error_code/vk_error_code_v100.hpp"
#endif
#if VK_HEADER_VERSION == 99
    #include "detail_error_code/vk_error_code_v99.hpp"
#endif
#if VK_HEADER_VERSION == 98
    #include "detail_error_code/vk_error_code_v98.hpp"
#endif
#if VK_HEADER_VERSION == 97
    #include "detail_error_code/vk_error_code_v97.hpp"
#endif
#if VK_HEADER_VERSION == 96
    #include "detail_error_code/vk_error_code_v96.hpp"
#endif
#if VK_HEADER_VERSION == 95
    #include "detail_error_code/vk_error_code_v95.hpp"
#endif
#if VK_HEADER_VERSION == 94
    #include "detail_error_code/vk_error_code_v94.hpp"
#endif
#if VK_HEADER_VERSION == 93
    #include "detail_error_code/vk_error_code_v93.hpp"
#endif
#if VK_HEADER_VERSION == 92
    #include "detail_error_code/vk_error_code_v92.hpp"
#endif
#if VK_HEADER_VERSION == 91
    #include "detail_error_code/vk_error_code_v91.hpp"
#endif
#if VK_HEADER_VERSION == 90
    #include "detail_error_code/vk_error_code_v90.hpp"
#endif
#if VK_HEADER_VERSION == 89
    #include "detail_error_code/vk_error_code_v89.hpp"
#endif
#if VK_HEADER_VERSION == 88
    #include "detail_error_code/vk_error_code_v88.hpp"
#endif
#if VK_HEADER_VERSION == 87
    #include "detail_error_code/vk_error_code_v87.hpp"
#endif
#if VK_HEADER_VERSION == 86
    #include "detail_error_code/vk_error_code_v86.hpp"
#endif
#if VK_HEADER_VERSION == 85
    #include "detail_error_code/vk_error_code_v85.hpp"
#endif
#if VK_HEADER_VERSION == 84
    #include "detail_error_code/vk_error_code_v84.hpp"
#endif
#if VK_HEADER_VERSION == 83
    #include "detail_error_code/vk_error_code_v83.hpp"
#endif
#if VK_HEADER_VERSION == 82
    #include "detail_error_code/vk_error_code_v82.hpp"
#endif
#if VK_HEADER_VERSION == 81
    #include "detail_error_code/vk_error_code_v81.hpp"
#endif
#if VK_HEADER_VERSION == 80
    #include "detail_error_code/vk_error_code_v80.hpp"
#endif
#if VK_HEADER_VERSION == 79
    #include "detail_error_code/vk_error_code_v79.hpp"
#endif
#if VK_HEADER_VERSION == 78
    #include "detail_error_code/vk_error_code_v78.hpp"
#endif
#if VK_HEADER_VERSION == 77
    #include "detail_error_code/vk_error_code_v77.hpp"
#endif
#if VK_HEADER_VERSION == 76
    #include "detail_error_code/vk_error_code_v76.hpp"
#endif
#if VK_HEADER_VERSION == 75
    #include "detail_error_code/vk_error_code_v75.hpp"
#endif
#if VK_HEADER_VERSION == 74
    #include "detail_error_code/vk_error_code_v74.hpp"
#endif
#if VK_HEADER_VERSION == 73
    #include "detail_error_code/vk_error_code_v73.hpp"
#endif
#if VK_HEADER_VERSION == 72
    #include "detail_error_code/vk_error_code_v72.hpp"
#endif

#endif // VK_ERROR_CODE_HPP