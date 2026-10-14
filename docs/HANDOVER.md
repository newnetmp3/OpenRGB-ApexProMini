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
