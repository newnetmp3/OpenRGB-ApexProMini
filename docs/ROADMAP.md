# Apex Pro Mini OpenRGB plugin roadmap

- [x] Device HID protocol serializer and offline unit test
- [x] Qt5 native OpenRGB API 5 plugin build, metadata and device registration (owner verified)
- [x] Direct full-color lighting and Onboard restoration (owner verified)
- [x] Physical-photo-inspired keycap UI, click/multi-select and bulk editing (owner verified for early editor)
- [x] Live preview implementation, current OpenRGB colors (owner feedback: awesome; no independent detailed runtime logs)
- [x] Initial Rainbow, Wave and Breathing animations (**owner reports working on keyboard**)
- [x] Gradient, Color Shift, Comet, Sparkle algorithms and UI entries committed (**pending owner build/hardware test**)
- [ ] Validate effects build/behavior, frame pacing and transition edge cases
- [x] Implement all 14 core effects (including click-driven Ripple, Reactive, Heatmap and Reactive Line) — new modes pending owner compile/hardware QA
- [x] Port 31 historical Prism-style preset entries (30 unique base records plus redPulse/faze rename) and 36 source-derived regional variants — independent approximations, not GG-parity certified
- [x] Implement opt-in physical evdev keypress capture for verified wired Mini input node (pending owner build/runtime validation)
- [ ] Validate physical typing reactions and permissions on owner's Arch Linux Wayland desktop
- [ ] Add hotplug reconnect and physical-event input status recovery
- [ ] Validate effects and regional masks against physical layout and owner hardware
- [x] Label all reconstructed Prism-style presets and preserve provenance; visual parity unverified
- [x] Add named JSON profile saving/loading and effect controls (brightness, reverse, base/accent; pending owner build/UI validation)
- [ ] Polish effect-specific parameter presentation and save/load integration testing
- [ ] Improve physical key geometry/regions; verify SteelSeries-key HID mapping
- [ ] Hotplug/reconnect handling, error reports, collision avoidance and recovery
- [ ] Native Arch Linux packaging, release CI and installation instructions

Preserve `68 HID addresses` vs `61 photographed physical keycaps` distinction.

- [x] Address reported Prism-reactive Heatmap regression: unified factory reactive routing, cumulative heat, dedicated frame tests (pending owner Qt5 hardware validation)

- [x] Owner reports Prism-style physical keypress reactive effects now work after Heatmap/dispatch fix (2026-10-14)

- [x] Fix catalog test confusing uniform Color Shift (ID 5) with spatial Gradient (ID 4); pending owner rerun of offline and Qt5 builds
