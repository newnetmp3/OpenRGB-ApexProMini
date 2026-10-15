# GG effect fidelity port — work in progress

Source reference: `newnetmp3/ApexProMini_gg_rgb`, particularly `src/compositor.rs`, `src/graphics.rs`, `src/reactive_factory.rs` and `src/layout.rs`.

## Stage 1 — committed on this branch
- Added `include/apex_pro_mini_scene.h`: standalone C++17 Normal/Add/Multiply/Screen blend operations, opacity and soft radial aperture masks.
- Added `tests/scene_compositor_test.cpp`: offline regression cases for blending, opacity clamping and masks.
- No existing animation dispatch, HID report, evdev handling, Qt editor or profile code changed.
- Test source is **not yet wired into CMake/CTest or independently compiled**. No hardware or GUI validation claimed.

## Next integration steps
1. Register the standalone test in the existing CMake test harness and run offline compilation/CTest.
2. Port ordered scene recipes for `westCoast`, `haze` and `warpDrive` from the Rust implementation into C++17, keeping the existing factory IDs, palettes and speed semantics stable.
3. Add snapshot tests across phases and key positions, comparing both implementations using generated offline frames.
4. Port Rust's adaptive factory reactive durations (250/500/1250 ms at nominal speed) and overlapping hit handling without regressing the owner-confirmed evdev path.
5. Improve width-aware physical geometry and regional masks; preserve all 68 HID usages and 61 visible ANSI keycaps.
6. Expose only meaningful per-effect controls in the Qt UI; validate scrolling/resizing and profiles.
7. Run the complete plugin build and physical QA when the owner has the keyboard.

These are independent reconstructions from recovered parameters, **not exact proprietary GG shaders**. Do not claim parity without comparisons.
