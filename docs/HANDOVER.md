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
