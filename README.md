# OpenRGB Apex Pro Mini Plugin

Standalone OpenRGB plugin project for the **wired** SteelSeries Apex Pro Mini (USB VID:PID `1038:161e`). The goal is to avoid modifying OpenRGB's built-in SteelSeries controller.

**Status:** Built and hardware-tested on Arch Linux using locally compiled Qt5 OpenRGB. The wired keyboard is detected, Direct RGB changes physical lighting, and switching to Onboard restores internal lighting. A packaged release and effects port are not yet complete.

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

## Owner hardware verification — 2026-10-14

On Arch Linux with a locally compiled Qt5 OpenRGB, the user confirmed the plugin loads, the Apex Pro Mini appears as an OpenRGB device, Direct mode successfully changes physical keyboard RGB colors, and Onboard restores the internal lighting. This confirms the initial device discovery, virtual-controller registration, RGB HID transport and release workflow on the owner's wired keyboard. This does **not** yet verify per-key visual matrix coordinates, all software effects, multiple attached keyboards, hotplug, or packaging.

## Provisional keyboard matrix (2026-10-14)

The device now exposes a 5-row by 32-column OpenRGB matrix with human-readable key names instead of a 68-LED linear zone. The map references the original 68 HID IDs by LED index, and encoding order is unchanged. Placement is an **ANSI-like approximation** with extra positions for firmware and ISO variants, not a validated manufacturer's geometry. Protocol tests verify matrix coverage, bounds, and uniqueness. Visual key placement and all RGB effects remain to be validated on the owner's device.

## Native matrix visual cleanup (2026-10-14)

The first hardware screenshot showed a sparse 5×32 checkerboard of uniformly sized cells. A follow-up change packs the same 68 addressable HID usages into a 5×16 matrix, with a center-positioned Space LED. This is a **visual compactness fix**, not a true keycap-shaped rendering: OpenRGB's native LED grid still draws each key as an equal-sized rectangle, and variant usages are still included. Custom keyboard-shaped Qt graphics are a separate milestone. A real OpenRGB render of the new matrix is pending.

## Photograph-matched physical keycaps (2026-10-14)

The owner supplied a clear overhead photo of their wired ANSI Apex Pro Mini. The native OpenRGB visualization has been corrected to include **61 physical keycaps**, in rows of **14 / 14 / 13 / 12 / 8**. The bottom row is Ctrl, Windows, Alt, Space, Alt, Windows, SteelSeries-logo key, Ctrl. The firmware exposes 68 HID addresses; seven extra slots (137, 50, 100, 135, 139, 138, 136) remain controllable through the OpenRGB LED list / transport, but are not displayed as imaginary physical keys. The SteelSeries-logo key uses provisional HID 240 pending owner confirmation. The grid remains square-cell based: exact widths and offsets require the planned custom Qt keyboard widget. **The new grid and individual key mapping are not yet tested on hardware.**

## Interactive keycap editor (development)

The Settings-tab placeholder widget has been replaced with a standalone Qt5/Qt6-compatible 61-key ANSI-like editor based on the owner's keyboard photograph. Key widths approximate real keycap widths, including Backspace, Tab, Caps, Enter, both Shifts, and Space. Click a key, then **Set selected key color…** to pick a color. This calls the virtual OpenRGB controller's `SetColor` and `UpdateLEDs` only in Direct mode, retaining the hardware-verified HID output/release commands and the standard OpenRGB Devices tab. Onboard mode deliberately refuses software LED writes. The SteelSeries-logo key's HID 240 remains provisional. The editor does not yet provide live color previews, multi-key selection, effects, presets or a complete alternative to OpenRGB's standard UI.

**Not yet compile- or hardware-verified:** build the updated plugin and confirm the Settings-tab editor opens and individual key assignments correspond to the physical keyboard; the previous working plugin binary can be restored if the new build fails. Keep OpenRGB closed when replacing its `.so`.

## Drag multi-key selection (2026-10-14)

The Settings-tab Qt keycap editor now supports click for one key, **left-button rectangle drag** across multiple keys, Ctrl-click toggle, Ctrl-drag additive selection, Select All and Clear Selection. A semi-transparent Qt rubber-band marks the drag rectangle, and selected keys get a blue outline. Apply-color now sets each selected HID's controller color and calls `UpdateLEDs()` **once** per operation; it refuses writes in Onboard mode. These changes do not alter the hardware-tested HID encoding or 0x41 release behavior.

**Still to validate on the owner's Qt5 desktop:** Compile the new editor and test drag selection between rows, Ctrl-add, and that a bulk color changes precisely the selected physical keys. The callback reports successful scheduling of `UpdateLEDs`, not confirmation that hidapi reported a successful USB transfer.

## Live keycap color preview (2026-10-14)

The 61-key Settings-tab editor now displays OpenRGB's current per-key RGB values as the backgrounds of the corresponding keycaps. The selection is drawn with an independent cyan border so selecting keys does not obscure their colors. Contrasting label colors improve readability on light or dark RGB fills; key tooltips show the HID usage and hex color. A 150 ms Qt GUI timer polls **only while the editor is visible**, reading `RGBControllerInterface::GetColor` without performing new USB writes. Colors selected with the editor are reflected immediately after `UpdateLEDs`, and color changes made in OpenRGB's Devices tab are picked up during the next refresh. The color dialog opens at the first selected key's current value.

The software cannot query the hardware's onboard animated lighting, so Onboard mode retains the last known software preview rather than claiming to show the physical LEDs. HID protocol and device lifecycle are unchanged. **This version still requires local Qt5 compilation and owner visual testing.**

## Software animation foundation (development)

First pass adds three locally generated effects in the Apex Pro Mini Settings editor: **Rainbow**, **Color Wave**, and **Breathing**. The speed slider spans 0.25x–3x; Stop effect returns to static Direct colors. A Qt timer requests an approximately **20 Hz** frame rate (50 ms), using the already-working 68-usage `SetColor`/`UpdateLEDs` path and the live keycap preview; no new device commands are introduced. Switching to Onboard stops effect updates on the next timer tick instead of fighting internal lighting. Manual color edits stop the animation before applying their selection. The extra firmware slots remain addressable as part of all 68-key frames, with fallback geometry for unverified regional usages. Each frame is updated in a single controller call.

CMake now contains `apex_pro_mini_effects_test` for deterministic rainbow/wave/breathing frame generation, timing variation and preserved HID order. **Animation effects, Qt integration, USB frame-rate stability and Onboard transitions still require compilation and owner hardware testing.** These are independent approximations, not vendor-exact GG presets. The remaining effect catalogue and saved presets remain on the roadmap.
