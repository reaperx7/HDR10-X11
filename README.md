# HDR10-X11
Non-functional implementation work-in-progress framework code for getting HDR10 working under X11.

The goal of this project is to provider reference code for usage by xlibre-xserver to
incorporate HDR10 support under X11 with automatic detection via boolean toggles.

This code is non-functional in stand-alone and is provided as a reference only. This project
makes no claims of viability towards working code, but aims to provide a framework from
which to work from.

This code is licensed under BSDL 3 Clause license and is provided as-is with no support.

-------------------------------------------------------------------------------------------------------

Current goals:

- Provides HDR10 support for XLibre, enabling 10-bit color and HDR metadata.

Requests:
  SetHDRMetadata(window, primaries, eotf, max_cll, min_lum, max_lum)
  Sets HDR metadata for a window (Rec.2020, PQ, CTA-861-H).

- QueryHDRCapabilities(screen) -> (supported, max_depth, eotfs)
- Queries HDR support for a screen.

Configuration:
- Option "Enable_HDR10" "on|off" (default: off)
- Enables HDR10 in the modesetting driver.

Possible limitations of this code framework:

- Vulkan Dependency: Assumes amdvlk or Mesa 25+ with VK_EXT_hdr_metadata. Older Mesa versions lack
  HDR support.

- Simplified Metadata: The code simplifies Rec.2020 primaries handling; a full implementation needs
  precise CIE coordinates.

- Single Driver: Only modesetting is supported; other drivers (e.g., NVIDIA) need separate patches.

- Protocol Compatibility: The XHDR extension breaks X11’s 8-bit color model, requiring client updates.

Possible needed steps afterwards:

- Validate Vulkan Integration: Test with an HDR10-capable display and GPU.

- Expand to Other Drivers: Port HDR support to xf86-video-amdgpu or xf86-video-intel if needed.

