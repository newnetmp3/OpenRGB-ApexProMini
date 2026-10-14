# Development handover — 2026-10-14

## Status
- Owner hardware-verified wired SteelSeries Apex Pro Mini (VID:PID 1038:161e) with native Qt5 OpenRGB 1.0 plugin.
- Qt metadata and runtime loading work; RGB Direct lighting and 0x41 Onboard restoration are confirmed.
- 61-key custom keyboard Qt Settings-tab editor based on owner photo supports click, drag rectangle, Ctrl-click, Ctrl-drag, multi-key color, and live OpenRGB preview. User reported the editor works prior to live-preview enhancement and asked to keep going.
- Most recent unverified change: animated Rainbow, Color Wave, Breathing (Qt 50 ms timer, speed 0.25–3.0x), effects_test.cpp.
- The standard OpenRGB native LED view displays a simplified five-row matrix; seven extra HID usages are still available in standard LED list but not represented as physical keys.
- The SteelSeries logo key mapping to HID 240 is provisional.
- No verified compile/test result yet for the newest animation additions. An attempted external clone for tests failed because github.com DNS was unavailable in the execution container.

## Next tasks
1. Ask owner to run `OPENRGB_QT_MAJOR=5 OPENRGB_SOURCE_DIR="$HOME/local-git/OpenRGB" bash scripts/check-build.sh --plugin`. Resolve compiler/errors from their output.
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
