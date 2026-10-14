# OpenRGB Apex Pro Mini Plugin

Standalone OpenRGB plugin project for the **wired** SteelSeries Apex Pro Mini (USB VID:PID `1038:161e`). The goal is to avoid modifying OpenRGB's built-in SteelSeries controller.

**Status:** Qt/OpenRGB plugin source and optional Linux module build target implemented, **not yet compiled or runtime-validated**. No release artifact is available.

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
bash scripts/check-build.sh
```

## Experimental plugin build (OpenRGB API v5 / Qt6)\n\nRequires a matching OpenRGB 1.0 source checkout, Qt6 development packages, and hidapi-hidraw. The source is not yet compile-verified.\n\n```sh\ncmake -S . -B build-plugin -DBUILD_OPENRGB_PLUGIN=ON -DOPENRGB_SOURCE_DIR=/path/to/OpenRGB\ncmake --build build-plugin\n```\n\nThe plugin registers a virtual keyboard and writes to the wired Mini's vendor HID control interface. **Do not run the original native-driver fork build at the same time**, or two controllers may target the same hardware.\n\n## Next milestones

1. Confirm the OpenRGB plugin SDK's virtual device registration and shutdown APIs for the target OpenRGB release.
2. Implement plugin entrypoint, HID discovery and ownership, RGB controller lifecycle and OpenRGB keyboard matrix mapping.
3. Add a packaged Linux `.so` and installation instructions.
4. Test against OpenRGB on Arch Linux, including collision avoidance with any native device detector.
5. Port GG-inspired software effects separately; do not describe offline reconstructions as vendor-exact.

## Verification notes

- A local standalone C++ sanity check confirmed the 68 HID IDs are unique and 68 four-byte entries fit the 513-byte report. This is **not** a full build of the repository or an on-device test.
- The full Qt6/HIDAPI plugin build has not run successfully yet; development packages are unavailable in the current execution environment.
- Plugin API 5 is the target. Match the OpenRGB executable and SDK version before installing.

## Local OpenRGB plugin loading

OpenRGB 1.0's PluginManager requires a Qt `MetaData` object containing `OpenRGBPluginAPIVersion` and a unique `Id`. This project embeds `src/plugin-metadata.json` with `Q_PLUGIN_METADATA(FILE ...)`; metadata must be present for the manager to try loading the plugin. An older OpenRGB 0.9 plugin built for a different API may still be skipped independently.

After building the plugin against a compatible local OpenRGB source tree, stop OpenRGB before updating the shared library:

```sh
install -Dm755 build-plugin/OpenRGB-ApexProMini.so "${XDG_CONFIG_HOME:-$HOME/.config}/OpenRGB/plugins/OpenRGB-ApexProMini.so"
```

Restart the locally compiled OpenRGB and inspect its plugin log. Successful metadata recognition does **not** prove keyboard detection or lighting writes; check those separately.
