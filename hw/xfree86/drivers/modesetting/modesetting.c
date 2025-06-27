#BSD 3-Clause License
#
#Copyright (c) 2025, James Powell
#
#Redistribution and use in source and binary forms, with or without
#modification, are permitted provided that the following conditions are met:
#
#1. Redistributions of source code must retain the above copyright notice, this
#   list of conditions and the following disclaimer.
#
#2. Redistributions in binary form must reproduce the above copyright notice,
#   this list of conditions and the following disclaimer in the documentation
#   and/or other materials provided with the distribution.
#
#3. Neither the name of the copyright holder nor the names of its
#   contributors may be used to endorse or promote products #derived from
#   this software without specific prior written permission.
#
#THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
#AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
#IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
#DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
#FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
#DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
#SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
#CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
#OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
#OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

# This file appends into modesetting.c for the following reason:
# The modesetting driver uses Mesa/Vulkan for rendering. Add HDR10 support via Vulkan’s VK_EXT_hdr_metadata
# Initialize Vulkan in modesetting.c:PreInit.
# Call SetupVulkanHDR during driver initialization.
# Hook ApplyHDRMetadata into the rendering pipeline (e.g., BlockHandler or Composite).



#include <vulkan/vulkan.h>
#include "XHDR.h"

typedef struct _ModesettingRec {
    // Existing fields...
    VkPhysicalDevice physicalDevice;
    VkDevice device;
    VkBool32 hdrCapable;
} ModesettingRec;

static Bool
SetupVulkanHDR(ScrnInfoPtr scrn)
{
    ModesettingPtr ms = ModesettingGetPtr(scrn);
    VkPhysicalDeviceProperties props;
    vkGetPhysicalDeviceProperties(ms–

>physicalDevice, &props);
    ms->hdrCapable = VK_FALSE;

    // Check for HDR extensions
    uint32_t extCount;
    vkEnumerateDeviceExtensionProperties(ms->physicalDevice, NULL, &extCount, NULL);
    VkExtensionProperties *exts = malloc(extCount * sizeof(VkExtensionProperties));
    vkEnumerateDeviceExtensionProperties(ms->physicalDevice, NULL, &extCount, exts);
    for (uint32_t i = 0; i < extCount; i++) {
        if (strcmp(exts[i].extensionName, "VK_EXT_hdr_metadata") == 0) {
            ms->hdrCapable = VK_TRUE;
            break;
        }
    }
    free(exts);

    LogMessage(X_DEBUG, "Modesetting: HDR capable=%d\n", ms->hdrCapable);
    return ms->hdrCapable;
}

static void
ApplyHDRMetadata(ScrnInfoPtr scrn, WindowPtr pWin)
{
    if (!pWin->hdrMetadata)
        return;

    ModesettingPtr ms = ModesettingGetPtr(scrn);
    if (!ms->hdrCapable)
        return;

    VkHdrMetadataEXT metadata = {0};
    metadata.sType = VK_STRUCTURE_TYPE_HDR_METADATA_EXT;
    metadata.displayPrimaryRed.x = pWin->hdrMetadata->primaries & 0xFFFF; // Simplified
    metadata.displayPrimaryRed.y = (pWin->hdrMetadata->primaries >> 16) & 0xFFFF;
    // Set other primaries, luminance, MaxCLL (simplified)
    metadata.maxContentLightLevel = pWin->hdrMetadata->max_cll;
    metadata.maxFrameAverageLightLevel = pWin->hdrMetadata->max_lum;

    LogMessage(X_DEBUG, "Modesetting: Applying HDR metadata for window %ld\n", pWin->drawable.id);

    // Apply metadata to Vulkan swapchain (implementation-specific)
    // Assume Vulkan swapchain is set up
    vkSetHdrMetadataEXT(ms->device, 1, &ms->swapchain, &metadata);
}
