# OpenRGB Apex Pro Mini

An independent [OpenRGB](https://openrgb.org/) plugin for controlling per-key RGB lighting on the **wired SteelSeries Apex Pro Mini** under Linux.

The plugin exposes the keyboard as a virtual OpenRGB device and provides a keyboard-shaped editor, software lighting effects, reactive keypress effects, and locally saved lighting profiles.

> **Compatibility:** This project currently targets the wired device with USB vendor/product ID **`1038:161e`** and **OpenRGB Plugin API v5**. Development and on-device testing have primarily used **Arch Linux with a locally built Qt 5 OpenRGB**. Other keyboard revisions, wireless models, OpenRGB binaries, and Linux distributions have not been established as compatible.

This is a community project. It is **not affiliated with, endorsed by, or an official implementation from SteelSeries or OpenRGB**.

## Features

- **Direct and Onboard modes.** Direct mode sends per-key RGB updates; Onboard releases software control back to the keyboard's firmware.
- **Visual key editor.** A 61-key ANSI-style layout supports individual clicks, rectangle selection, Ctrl-click/Ctrl-drag multi-selection, and applying colors to selected keys. It previews colors currently held by OpenRGB.
- **Software effects.** Rainbow, Color Wave, Breathing, Gradient, Color Shift, Comet, Sparkle, Solid, Ripple, Reactive, Rainbow Split, Prismatic Sweep, Heatmap, and Reactive Line, plus Off.
- **Prism-style presets.** 31 historical preset entries and 36 regional palette records reconstructed from independently researched parameters. They are **not exact reproductions of SteelSeries GG effects**.
- **Effect controls.** Speed, brightness, reverse motion where applicable, Base and Accent colors, and an optional custom-color override for preset palettes.
- **Local profiles.** Save and load named profiles containing the selected effect, controls, and a snapshot of all 68 addressable RGB values.
- **Physical-keypress reactions (Linux).** Optional, off-by-default reading of this keyboard's Linux evdev key-down events for reactive effects. Normal typing is not intercepted or blocked.
- **Resizable settings interface.** Vertical scrolling for settings and horizontal scrolling for the keyboard when space is limited.

Software animation frames run on a roughly **20 Hz** update timer. The plugin does not configure actuation points or other keyboard firmware settings.

## Compatibility and requirements

| Component | Requirement |
| --- | --- |
| Keyboard | Wired SteelSeries Apex Pro Mini, USB ID `1038:161e` |
| Operating system | Linux; tested primarily on Arch Linux x86_64 |
| OpenRGB | An OpenRGB build supporting **Plugin API v5** and virtual RGB controllers |
| Qt | The plugin and the running OpenRGB executable must use the **same Qt major version** (Qt 5 or Qt 6) |
| Build dependencies | C++17 compiler, CMake 3.16+, Qt Core/Widgets development files, pkg-config, `hidapi-hidraw` development files |
| USB permissions | The current user needs access to the keyboard's vendor-defined HID interface |
| Reactive effects | Optional read access to the matching `/dev/input/eventN` node |

A plugin compiled against one OpenRGB checkout should **not** be assumed binary-compatible with another version or differently configured build. In particular, an Arch x86_64 build is not a universal Linux binary.

Do not run this plugin alongside another controller that is simultaneously writing lighting commands to the same keyboard.

## Installation

### Prebuilt Linux plugin

If a compatible binary is available on the [Releases page](https://github.com/newnetmp3/OpenRGB-ApexProMini/releases), download the `OpenRGB-ApexProMini.so` asset for your platform and OpenRGB/Qt build.

1. **Exit OpenRGB completely.**
2. Install the plugin in your user plugin directory:

   ```bash
   install -Dm755 OpenRGB-ApexProMini.so \
     "${XDG_CONFIG_HOME:-$HOME/.config}/OpenRGB/plugins/OpenRGB-ApexProMini.so"
   ```

3. Restart OpenRGB. The plugin's **Apex Pro Mini** page should appear under **Settings**, and the keyboard should appear in **Devices** when detected.

If OpenRGB fails to load the plugin, verify its Plugin API, Qt version, and shared-library dependencies before assuming a hardware problem.

### Build from source

Clone this repository and use a matching local checkout of the OpenRGB source tree. Example for **Qt 5**:

```bash
git clone https://github.com/newnetmp3/OpenRGB-ApexProMini.git
cd OpenRGB-ApexProMini

OPENRGB_QT_MAJOR=5 \
OPENRGB_SOURCE_DIR="$HOME/local-git/OpenRGB" \
  bash scripts/check-build.sh --plugin
```

The script runs the standalone protocol/effects tests, builds the plugin against `OPENRGB_SOURCE_DIR`, runs the plugin-related tests, and checks the compiled Qt plugin metadata. Use `OPENRGB_QT_MAJOR=6` only if the OpenRGB executable you intend to run uses Qt 6.

After a successful build, exit OpenRGB and install the output:

```bash
install -Dm755 build-plugin/OpenRGB-ApexProMini.so \
  "${XDG_CONFIG_HOME:-$HOME/.config}/OpenRGB/plugins/OpenRGB-ApexProMini.so"
```

The target OpenRGB checkout must provide `OpenRGBPluginInterface.h` and the RGB controller headers used by its Plugin API. Merely compiling successfully does not guarantee compatibility with a particular OpenRGB executable.

## Using the plugin

Open **Settings → Apex Pro Mini** for the visual lighting editor.

**Direct mode:** Choose **Direct** for software effects or editing individual key colors. Select a keycap with a click, drag across multiple keycaps for rectangular selection, or use Ctrl-click/Ctrl-drag to modify a selection. Choose **Set selected keys color…** to apply a static color.

**Animations:** Select an effect from **Animation**. Adjust speed and brightness, reverse moving effects where meaningful, and use **Stop effect** to stop the animation. Use **Base color**, **Accent color**, and **Use custom colors** to recolor supported animations and presets. Turning off custom colors returns reconstructed presets to their built-in palettes.

**Physical keypress effects:** The highlighted **Enable physical keypress reactions** control appears above the Animation selector. Turn it on to react to actual key presses on the wired keyboard. The status shows whether the matching Linux input event device was opened successfully. When disabled, clicking keys inside the editor can still demonstrate reactive behavior. This setting is intentionally **not enabled automatically by loading a profile**.

**Profiles:** Use **Save as…** and **Load** to manage locally stored lighting profiles. Saving and loading require Direct mode. Profiles include per-key RGB values and software settings; they do **not** write SteelSeries onboard profiles or store your typing history.

**Onboard mode:** Select **Onboard** in OpenRGB's device controls to release software RGB control to the keyboard. OpenRGB cannot read back the firmware's current onboard lighting pattern, so the editor is not a live display of onboard effects.

## Linux input permissions

The physical-keypress feature reads only the evdev interface associated with the matching wired USB keyboard. It processes key-down events in memory to drive animation; it does **not** grab the input device, record typed text, save keystroke history, or transmit keystrokes.

If the editor reports a permission error, use the **exact event-device path shown in the status**:

```bash
ls -l /dev/input/eventN
getfacl /dev/input/eventN
```

Replace `eventN` with the actual event number. Your desktop session's seat/udev permissions must grant read access. Avoid running OpenRGB as root or making input event nodes world-readable.

Unplugging the keyboard may require disabling and re-enabling physical-keypress monitoring after reconnecting; automatic input-device reconnection is not yet implemented.

## OpenRGB virtual-controller shutdown compatibility

Some OpenRGB source revisions create virtual RGB controllers without explicitly stopping their device worker thread before destruction. This can cause a thread-active warning or a shutdown crash.

This repository includes a **guarded, optional source patch** for affected OpenRGB checkouts:

```bash
python3 scripts/patch-openrgb-virtual-shutdown.py /path/to/OpenRGB
cmake --build /path/to/OpenRGB/build --parallel
```

The script changes the **local OpenRGB source tree**, not the plugin, and creates a backup. It refuses to modify a source file that does not match the expected destructor. **Do not apply it blindly to other OpenRGB versions.** Prefer a host build that already fixes the virtual-controller shutdown lifecycle. This plugin deliberately does not call `RGBController::Shutdown()` directly because that symbol may not be exported to plugins.

## Troubleshooting

| Problem | What to check |
| --- | --- |
| Plugin does not appear | Verify the `.so` location, matching Qt major version, and Plugin API v5 compatibility. |
| `does not have a MetaData field` | Often indicates mismatched Qt/plugin loading. Rebuild against the Qt major version used by the running OpenRGB. |
| Missing symbol or shared library | Check `ldd OpenRGB-ApexProMini.so` and rebuild against your installed OpenRGB source/runtime. |
| Keyboard not detected | Confirm the **wired** USB ID is `1038:161e` and that the vendor HID interface is accessible. |
| Effects do not update the keyboard | Select Direct mode; ensure no other RGB controller is writing to the same device. |
| Physical typing does not trigger reactive effects | Enable physical-keypress reactions; check the displayed connection status and event-node permissions. |
| Crash or thread warning when closing OpenRGB | See the virtual-controller shutdown compatibility section above. |

You can inspect Qt plugin metadata after building using:

```bash
./build-plugin/apex_plugin_metadata_check build-plugin/OpenRGB-ApexProMini.so
```

## Scope and limitations

- **Wired Apex Pro Mini only**; wireless models and other SteelSeries keyboards are outside the verified scope.
- The editor displays **61 physical ANSI-style keycaps**, while the device protocol addresses **68 HID usages**. Seven additional regional/firmware usages are exposed in OpenRGB's LED list, not as extra physical keycaps.
- Regional keyboard positions and the SteelSeries-logo-key mapping are partly provisional.
- Prism-style animations are independent, approximate renderings derived from documented parameter records, **not** SteelSeries GG code or guaranteed visual matches.
- Software profiles do not program the keyboard's onboard firmware.
- There is no automatic evdev reconnection or comprehensive multi-keyboard/hotplug support at this stage.
- Owner-reported hardware successes include Direct/Onboard lighting, standard animations, and physical-keypress reactive effects on an Arch/Qt5 setup. This is not a compatibility certification for other host builds.

## Development and acknowledgments

Core protocol serialization, effects, profile encoding, and GUI layout tests are available under `tests/`. Run the offline tests with `bash scripts/check-build.sh`, or the full plugin check with `--plugin` and a matching OpenRGB source tree.

Implementation notes and known development issues are tracked in [`docs/HANDOVER.md`](docs/HANDOVER.md) and [`docs/ROADMAP.md`](docs/ROADMAP.md).

Protocol exploration and historical preset parameters: [ApexProMini_gg_rgb](https://github.com/newnetmp3/ApexProMini_gg_rgb). OpenRGB is developed by the [OpenRGB project](https://github.com/CalcProgrammer1/OpenRGB).

SteelSeries, Apex Pro Mini, and SteelSeries GG are trademarks of their respective owners.
