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
- [ ] Capture real hardware keypress events for reactive effects (currently triggered only by clicking keys in plugin editor)
- [ ] Validate effects and regional masks against physical layout and owner hardware
- [x] Label all reconstructed Prism-style presets and preserve provenance; visual parity unverified
- [ ] Add preset saving/loading and per-effect controls
- [ ] Improve physical key geometry/regions; verify SteelSeries-key HID mapping
- [ ] Hotplug/reconnect handling, error reports, collision avoidance and recovery
- [ ] Native Arch Linux packaging, release CI and installation instructions

Preserve `68 HID addresses` vs `61 photographed physical keycaps` distinction.
