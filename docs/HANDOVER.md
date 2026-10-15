# Development handover — 2026-10-14

## Status
- Owner hardware-verified wired SteelSeries Apex Pro Mini (VID:PID 1038:161e) with native Qt5 OpenRGB 1.0 plugin.
- Qt metadata and runtime loading work; RGB Direct lighting and 0x41 Onboard restoration are confirmed.
- 61-key custom keyboard Qt Settings-tab editor based on owner photo supports click, drag rectangle, Ctrl-click, Ctrl-drag, multi-key color, and live OpenRGB preview. User reported the editor works prior to live-preview enhancement and asked to keep going.
- Owner reports initial Rainbow, Color Wave, and Breathing effects run well on wired keyboard (2026-10-14). Current GUI refreshes at 20 Hz (Qt 50 ms timer), speed 0.25–3.0x.
- New unverified batch: four additional software effects — static Gradient, whole-board Color Shift, moving Comet, deterministic Sparkle. Total 7 effects; existing selector extended and offline deterministic tests expanded.
- The standard OpenRGB native LED view displays a simplified five-row matrix; seven extra HID usages are still available in standard LED list but not represented as physical keys.
- The SteelSeries logo key mapping to HID 240 is provisional.
- The four newest effects have not yet been compiled or hardware-tested in the owner environment. The previous three were reported working; do not conflate status.

## Next tasks
1. Ask owner to run `OPENRGB_QT_MAJOR=5 OPENRGB_SOURCE_DIR="$HOME/local-git/OpenRGB" bash scripts/check-build.sh --plugin`. Resolve compiler/errors from their output. Test newly added Gradient, Color Shift, Comet and Sparkle.
2. Check RGB animations on hardware; verify 20 Hz USB stability, Direct/Onboard transitions, live keycap preview, no leakage when switching to Onboard.
3. Port remaining researched effects (gradient, color-shift, sparkle, ripple, reactive, comet, rainbow-split, prismatic-sweep, heatmap, reactive-line) with strict separation between validated hardware behavior and reconstructed vendor-style effects.
4. Build region/layer/preset controls, persistence, UI quality, tests, release packaging and CI.
5. Ensure `docs/HANDOVER.md` and `docs/ROADMAP.md` get updated with each completed milestone.

## Build/install
```bash
cd ~/local-git/OpenRGB-ApexProMini
git pull --ff-only
OPENRGB_QT_MAJOR=5 OPENRGB_SOURCE_DIR="$HOME/local-git/OpenRGB" bash scripts/check-build.sh --plugin
# Quit OpenRGB fully first:
install -Dm755 build-plugin/OpenRGB-ApexProMini.so ~/.config/OpenRGB/plugins/OpenRGB-ApexProMini.so
../OpenRGB/build/openrgb
```

Avoid running the original standalone Apex RGB service concurrently with the plugin.

## Full catalog port — 2026-10-14

Source: `newnetmp3/ApexProMini_gg_rgb` (Rust `src/effects.rs`, `src/factory.rs`, `src/regions.rs`). New C++ source headers:
- `include/apex_pro_mini_catalog.h`: seven remaining core modes and generic Prism-style palette renderer, plus existing seven animation modes via `apex_pro_mini_effects.h` (14 core modes excluding UI Off).
- `include/apex_pro_mini_factory.h`: all 31 historical named preset entries, including redPulse/faze alias (30 distinct underlying base definitions).
- `include/apex_pro_mini_regions.h`: 36 region palette variations for eight named preset groups.
- `src/ApexMiniKeyboardEditor.h`: effect selector covers all modes and 31 named Prism-style presets. Clicking an editor key while an effect runs records a temporary KeyHit for demos of Ripple, Reactive, Heatmap and Reactive Line. **No evdev or system-wide keyboard event capture** is implemented, so reactive animations do not yet react to ordinary physical typing.
- `tests/catalog_test.cpp`: checks all frame lengths and HID order, encodability, 31 historical names, 36 regional variants and click-hit reactivity.

Latest batch is **NOT COMPILE-VERIFIED**, NOT hardware-verified, and is not a claim of exact vendor GG rendering. Region coordinates are approximate; source migration data summarize parameter palettes/periods but not complete proprietary compositor behavior.

Next: run Qt5 build+CTest and test effects, errors, frame rate, UI selection, Stop, Onboard. Address hardware key event support without grabbing/delaying keyboard input; preferably an optional read-only evdev integration with explicit permissions and user opt-in.

## Optional real-key input integration — 2026-10-14

Owner reports full effect/preset catalog works on the wired keyboard and requests genuine physical keypress reactions.

Implemented Linux-only opt-in read-only evdev support:
- `src/ApexMiniPhysicalInput.h` enumerates `/sys/class/input/event*`, validates ancestor USB VID 1038 / PID 161e and KEY_A capability, then opens only the matching `/dev/input/eventN` node with `O_RDONLY|O_NONBLOCK|O_CLOEXEC`.
- `include/apex_pro_mini_evdev_codes.h` maps Linux key codes to valid Apex USB HID IDs.
- `QSocketNotifier` dispatches only `EV_KEY` with `value==1` to the existing transient `KeyHit` vector. No grabs, keystroke logging, files, remote transmission or background service.
- Settings editor adds **Enable physical keypress reactions** toggle (off by default), with device/permission status. Turning it off closes the evdev file descriptor.
- `tests/evdev_codes_test.cpp` plus CTest entry validate normal key translations. Real event node detection and Qt5 runtime tests still need owner verification.
- Optional read permission may be needed via desktop seat ACL/udev policy. Avoid chmod 666, indiscriminate input group elevation or running OpenRGB as root.
- **Caveat:** real typing is not an independent global system keyboard hook. It is limited to a matching wired keyboard event node, which is intentional. Keycaps clicked within editor remain a functional trigger.
- **Caveat:** hot unplug currently shuts down the notifier, but its UI status may not update until re-enabled; reconnect requires toggling monitoring again. Future improvement: hotplug detection and reporter status.

Build using same standard Qt5 plugin command; no extra third-party libevdev dependency required (Linux input UAPI and Qt Core).

## Prism-reactive regression fix — 2026-10-14

Owner verifies physical Ripple works 100%, but reports Prism-style key-triggered effects such as Heatmap do not operate properly. Thus evdev detection and Linux key mapping are functional; target the Prism renderer rather than reopening input permissions.

- Added explicit `reactive_kind(effect)` routing for factory `reactiveLine` (+5), `reactiveRipple` (+6), `reactiveSingleKey` (+7), and `heatmap` (+29).
- Both core and factory heatmap use cumulative recent-key intensity with exponential falloff, instead of a max-only value.
- Factory presets retain their white palette accents and dark idle background; no HIDs, USB transport or callback behavior changed.
- Added CTest regressions asserting all four factory reactive modes visually change when supplied a real `KeyHit`, plus repeated hits heat up the same key more than one hit.
- Corrected GUI labels to indicate keypress-reactive (physical with opt-in evdev, editor-click fallback).
- This fix is committed but not yet independently Qt5-build or hardware verified; owner should build, test all four with input toggle on, and report any failures with exact preset name.

Next: improve diagnostics for input event count and confirm true hardware keypress response, not merely editor-click effects.

## Owner hardware verification — 2026-10-14

Following the Prism-reactive regression fix (unified reactive dispatch and cumulative Heatmap), the owner reports: **"they work"**. This confirms the previously failing Prism-style keystroke-reactive presets now operate on the wired keyboard under their configured Arch Linux/OpenRGB setup, including the preset family discussed (Heatmap, Reactive Line, Reactive Ripple, Single Key). Keep the evdev opt-in behavior, validated Direct/Onboard handling, live preview and existing 68-HID transport stable. No additional runtime metrics or per-preset screenshots were provided; do not claim exact vendor parity. Next development priorities: profile save/load, optional reconnection/status UI, and improved controls without regressing physical typing reactions.

## Milestone: named presets and per-effect controls — 2026-10-14

User explicitly prioritized “Add preset saving/loading and per-effect controls” over hotplug/reconnection. Implemented:
- `src/ApexMiniProfiles.h`: QtCore-only versioned JSON profile format, atomic `QSaveFile` writes, strict names and 68-HID RGB snapshot validation. Files stored in `QStandardPaths::AppConfigLocation/apex-pro-mini/profiles` (typically `~/.config/<OpenRGB app name>/apex-pro-mini/profiles`); no keys or keypress history stored.
- `src/ApexMiniKeyboardEditor.h`: named profile chooser plus Save as and Load buttons. Captures chosen effect, speed slider, brightness slider, reverse motion, base/accent colors and all 68 current OpenRGB LED colors (requires Direct mode). Loading validates first and restores 68-key snapshot; selecting the saved effect resumes its animation if one is selected. Onboard mode declines load without changing UI state.
- `include/apex_pro_mini_catalog.h`: `EffectControls` adds global software brightness, reverse-time motion, Solid base color, editable palettes for Breathing/Gradient/Color Shift, configurable reactive base/accent. Prism-style palettes retain their historical source colors. This preserves keyboard HID/evdev logic.
- `tests/profiles_test.cpp` and extended catalog tests for JSON roundtrip, name/version/HID restrictions, atomic disk write, brightness blackout and edited colors; Qt profile test added to CMake when building the plugin.
- Existing physical-keypress enable switch is NOT persisted or auto-enabled by profiles for privacy.
- **Pending:** compile Qt5 locally, test saves/loads for core/Prism/keypress-reactive and static per-key colors, and verify new controls in owner’s OpenRGB GUI. No CI success confirmed in this session.

Profiles are a local software format; they do not save or read SteelSeries onboard lighting presets.

## CTest catalog test fix — 2026-10-14

Owner's `bash scripts/check-build.sh --plugin` stopped at the first offline CTest stage: `apex_pro_mini_catalog_test` line 82 asserted `grad[0].color.r != grad[10].color.r`. The test inadvertently invoked effect ID 5, **Color Shift**, which correctly produces uniform spatial color, instead of effect ID 4, **Gradient**. Fixed regression test to explicitly refer to `Effect::Gradient`, compare known Esc/Backspace HIDs for a spatial red/green gradient, and separately assert `Effect::ColorShift` yields equal key colors. No renderer, evdev, plugin transport or UI changes. Owner must re-run build; the script's `set -e` prevents the Qt plugin build from starting until offline CTest passes. The updated test is committed but not yet rerun on the owner's system.

## Fix: Base/Accent ignored by Prism presets — 2026-10-14

Owner reports Base and Accent pickers have no visible effect on many/all historical presets. Root cause: renderer used built-in Prism color stops and ignored editor Base/Accent except on a few native modes.

- New `EffectControls::custom_palette`, default **false** to preserve established source-derived colors.
- GUI adds **Use custom colors** checkbox, automatically enabled upon choosing either base or accent; users can uncheck to restore the original palette.
- `recolor_pixel` maps original per-key brightness/luminance envelope to the custom base/accent colors, preserving black masks and effect timing, motion, heat/keypress response and layout. Applies to Prism factory palette modes, Rainbow, Wave, Comet, Sparkle, Rainbow Split, and Prismatic Sweep. Core native solid/breathing/gradient/color-shift and reactive modes already use their existing base/accent treatment.
- Existing JSON version-1 profiles gain optional `custom_palette` bool, with absence read as false for backward compatibility. Saved custom selection is restored by Load.
- Added tests over all non-disabled historical presets with recent synthetic physical KeyHits, restoring original colors when toggle is false, plus profile roundtrip and old-format decode.
- Status: commits pushed, **not yet owner hardware or Qt5 build verified**. No HID or evdev changes; preserve prior working physical reactive effects. Owner should test e.g. Prism, Pride, Heatmap, reactiveRipple and switching custom colors off.

## Exit segfault / virtual RGBController shutdown — 2026-10-14

Owner reports every OpenRGB exit logs `[RGBController] SteelSeries Apex Pro Mini (Plugin): Device thread still active in base class destructor, ensure Shutdown() was called from inherited destructor` then segfaults. Source inspection of upstream `CalcProgrammer1/OpenRGB` shows `RGBController::Shutdown()` stops/joins the device call thread and drains outstanding update callbacks. Previously `ApexProMiniPlugin::Unload()` directly unregistered and deleted the virtual controller before restoring onboard LEDs and closing hidapi. This was a use-after-free/racy teardown.

Fix: `KeyboardEditor::StopActivity()` stops the animation and live preview Qt timers, closes optional evdev reader and clears callback closures; `ApexProMiniPlugin::Unload()` calls StopActivity, unregisters the virtual controller, invokes `static_cast<RGBController*>(controller_)->Shutdown()`, then deletes controller, sends onboard release, closes HID handle, exits hidapi. `RGBController.h` is provided by the existing OpenRGB source tree include path. No effect, profile, transport or input mapping behavior changed.

**UNVERIFIED:** must compile with owner's matching OpenRGB Qt5 checkout; test repeated exit with animation running, physical reactive input on, input off and Onboard mode; inspect `coredumpctl list` for absence of new crashes. Preserve last working plugin backup. Avoid calling Shutdown more than once; current Unload nulls controller and is idempotent on repeated calls.

## Loader symbol regression and host fix — 2026-10-14

Owner confirmed previous exit segfault no longer reproduced in the immediate test, but launch/exit now aborts with:
`OpenRGB-ApexProMini.so: undefined symbol: _ZN13RGBController8ShutdownEv`

Root cause: plugin linked against OpenRGB's non-exported `RGBController::Shutdown()` (previous patch), so the runtime loader fails to resolve it. Upstream source inspection `CalcProgrammer1/OpenRGB`, `RGBController/RGBController_Virtual.cpp`, confirms `RGBController_Virtual::~RGBController_Virtual()` is empty. `OpenRGBPluginAPI::DeleteVirtualRGBController()` deletes the controller without Shutdown. OpenRGB base class destructor warns when thread is running; host can race plugin callbacks.

Fix in this repo:
1. Remove all direct Shutdown references and `#include <RGBController.h>` from plugin source (no loader lookup).
2. Add `scripts/patch-openrgb-virtual-shutdown.py`, an idempotent guarded patch of the user's **OpenRGB host source**, inserting `Shutdown();` inside `RGBController_Virtual::~RGBController_Virtual()` (where the host binary can link it). Creates a one-time backup; refuses unknown source versions.
3. Keep `KeyboardEditor::StopActivity()` and normal plugin unregister/delete/firmware release/HID close.

**Requires user to rebuild BOTH host and plugin**:
```bash
cd ~/local-git/OpenRGB-ApexProMini
git pull --ff-only
python3 scripts/patch-openrgb-virtual-shutdown.py ~/local-git/OpenRGB
cmake --build ~/local-git/OpenRGB/build --parallel
OPENRGB_QT_MAJOR=5 OPENRGB_SOURCE_DIR="$HOME/local-git/OpenRGB" bash scripts/check-build.sh --plugin
# Quit OpenRGB before copying .so
install -Dm755 build-plugin/OpenRGB-ApexProMini.so ~/.config/OpenRGB/plugins/OpenRGB-ApexProMini.so
../OpenRGB/build/openrgb
```
Test close/restart with animations and physical key input enabled. No verified owner runtime yet. Upstream host fix is a local source modification, and may need reapplying after updating OpenRGB.

## UI emphasis for physical keypress toggle — 2026-10-14

User requests that **Enable physical keypress reactions** stand out and appear above Animation selector. Moved existing Linux-only opt-in evdev controls into an amber-outline **⚠ PHYSICAL KEYPRESS REACTIONS** QGroupBox immediately before the animation selector, with concise instruction: required for real typing responses in Ripple, Reactive, Heatmap and applicable Prism-style effects; not required for standard animations. Increased toggle height and added clear OFF / CONNECTED / UNAVAILABLE state message, retaining device-specific status/error details. No change to evdev capture, opt-in default, effect rendering, saved profiles or host thread cleanup. Build and on-device visual QA pending.

## Physical-keypress emphasis layout QA — 2026-10-14

Owner screenshot showed amber warning panel's title, instructions, status and button overlapping vertically. Root cause is the previous untitled, stylesheet-painted `QGroupBox` receiving inadequate size allocation within compact OpenRGB Settings. Replaced with `QFrame` plus explicit VBox margins, spacing, minimum height (105 px), minimum vertical size policy and clearer two-row layout. Shortened instructional text and status. Moved noncritical seven-extra-HID-address technical footer to the heading tooltip to recover Settings panel height. Physical evdev integration, opt-in toggle and reactive effects unchanged. Commit pending owner visual inspection with same window size; Qt5 compile not independently verified.

## Adaptive/resizable Settings tab — 2026-10-14

User requested Apex plugin Settings content-aware on smaller OpenRGB windows. Removed the hard `setMinimumWidth(820)` constraint. The editor now contains a vertically scrollable `QScrollArea` with widgetResizable page, while the full 61-key virtual keyboard is kept at natural keycap proportions inside a **separate horizontally scrollable viewport**; this prevents keycap compression and oversized host minimum width. The selection/status, physical reactive switch, effect/speed, brightness/palette and profile control rows are now responsive `QBoxLayout` rows that stack vertically when the pane is narrower than 900 px. Existing mouse selection maps coordinates to the editor; keycap positions and HID mappings are unchanged. The highlighted reactive QFrame remains above Animation, preserving explicit opt-in monitoring. Removed only layout constraints, not keyboard logic.

Added `tests/editor_layout_test.cpp` to the Qt plugin CTest suite, using the Qt offscreen backend to exercise page/keyboard scroll policies and viewport geometry at 510/560/730/1080 px widths. Tests are committed but NOT executed in this environment, which lacks Qt5 development headers. Owner must run `bash scripts/check-build.sh --plugin` and manually resize Settings window while trying selection, save/load and animations. 
