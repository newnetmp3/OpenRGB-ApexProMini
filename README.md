# OpenRGB Apex Pro Mini Plugin

Standalone OpenRGB plugin project for the **wired** SteelSeries Apex Pro Mini (USB VID:PID `1038:161e`). The goal is to avoid modifying OpenRGB's built-in SteelSeries controller.

**Status:** Initial protocol library and development scaffold. This is **not yet a loadable OpenRGB plugin**. The OpenRGB plugin device-registration interface, build integration, and runtime lifecycle still require implementation and testing.

## Protocol

- HID interface 1, usage page `0xFFC0`, usage 1, report ID 0.
- Direct lighting: 513-byte **feature** report: `00 40 NN` followed by up to 68 entries of `[HID usage, R, G, B]`, zero-padded.
- Release direct lighting: 65-byte **output** report beginning `00 41`.
- The firmware's 70 indexed slots are **not** interchangeable with the 68 supported HID usage codes.
- No onboard profile or actuation writes are needed.

Protocol research: [ApexProMini_gg_rgb](https://github.com/newnetmp3/ApexProMini_gg_rgb). The earlier native OpenRGB prototype is preserved in [draft PR #1](https://github.com/newnetmp3/OpenRGB/pull/1).

## Development

The `include/apex_pro_mini_protocol.h` header provides dependency-free packet serialization. It performs no HID I/O and does not claim to be a plugin entrypoint. This keeps transport encoding testable independently of OpenRGB and the keyboard.

```sh
cmake -S . -B build
cmake --build build
ctest --test-dir build --output-on-failure
```

## Next milestones

1. Confirm the OpenRGB plugin SDK's virtual device registration and shutdown APIs for the target OpenRGB release.
2. Implement plugin entrypoint, HID discovery and ownership, RGB controller lifecycle and OpenRGB keyboard matrix mapping.
3. Add a packaged Linux `.so` and installation instructions.
4. Test against OpenRGB on Arch Linux, including collision avoidance with any native device detector.
5. Port GG-inspired software effects separately; do not describe offline reconstructions as vendor-exact.
