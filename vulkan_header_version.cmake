# Copyright (C) 2020 George Cave - gcave@stablecoder.ca
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#   http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.

# Searches for the 'vulkan_core.h' header, and if found, searches it and
# retrieves the corresponding header version in the ${VULKAN_HEADER_VERSION}
# variable to the calling scope.
function(get_vulkan_header_version)
  find_path(VULKAN_CORE_INCLUDE_DIR NAMES vulkan/vulkan_core.h)

  file(READ "${VULKAN_CORE_INCLUDE_DIR}/vulkan/vulkan_core.h"
       VULKAN_HEADER_DATA)
  string(REGEX MATCH "#define VK_HEADER_VERSION ([0-9]*)" _
               ${VULKAN_HEADER_DATA})

  set(VULKAN_HEADER_VERSION
      ${CMAKE_MATCH_1}
      PARENT_SCOPE)
endfunction()
