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

### Diagnosing “does not have a MetaData field”

The plugin build now creates `build-plugin/apex_plugin_metadata_check` and runs it against the compiled `.so`. It uses Qt's `QPluginLoader::metaData()` and checks the same required fields as OpenRGB's PluginManager. A successful C++ compilation alone is not enough.

When updating the installed plugin, first **close OpenRGB completely** and verify the module you installed is byte-for-byte the freshly built file:

```sh
OPENRGB_SOURCE_DIR="$HOME/local-git/OpenRGB" bash scripts/check-build.sh --plugin
install -Dm755 build-plugin/OpenRGB-ApexProMini.so "$HOME/.config/OpenRGB/plugins/OpenRGB-ApexProMini.so"
sha256sum build-plugin/OpenRGB-ApexProMini.so "$HOME/.config/OpenRGB/plugins/OpenRGB-ApexProMini.so"
./build-plugin/apex_plugin_metadata_check "$HOME/.config/OpenRGB/plugins/OpenRGB-ApexProMini.so"
```

Both SHA-256 values must be identical, and both metadata checks must succeed. If either check fails, do not start OpenRGB yet. The two older `0.9_Bookworm` plugins are unrelated and may continue producing their own warnings.

## Qt runtime compatibility

OpenRGB may be compiled with Qt5 or Qt6. The plugin must use the **same Qt major version as the OpenRGB executable**. A Qt6 plugin can pass Qt6's standalone metadata inspector yet be rejected by a Qt5 OpenRGB loader with `does not have a MetaData field`.

Inspect the executable directly: `ldd /absolute/path/to/OpenRGB | grep -E 'libQt[56](Core|Widgets)'`. Build the plugin with `OPENRGB_QT_MAJOR=5` or `OPENRGB_QT_MAJOR=6` according to that output. The plugin build defaults to 5 for locally compiled qmake installations; rebuild from a clean `build-plugin` directory when changing majors. For Arch Linux Qt5 builds, install `qt5-base` if it is missing.

## Startup crash fixed (2026-10-14)

The native Qt5 OpenRGB runtime crashed in `QWidget::setParent` through `OpenRGBPluginContainer::OpenRGBPluginContainer` because `GetWidget()` returned `nullptr`. OpenRGB unconditionally reparents plugin widgets, including Settings-tab plugins. The implementation now returns a real `QLabel` tracked with `QPointer` to avoid a dangling widget pointer if Qt deletes it. **Requires an on-device runtime retest after rebuilding**; do not treat a successful compile as proof of safe startup.
