# Apex Pro Mini OpenRGB plugin roadmap

- [x] Device HID protocol serializer and offline unit test
- [x] Qt5 native OpenRGB API 5 plugin build, metadata and device registration (owner verified)
- [x] Direct full-color lighting and Onboard restoration (owner verified)
- [x] Physical-photo-inspired keycap UI, click/multi-select and bulk editing (owner verified for early editor)
- [x] Live preview implementation, current OpenRGB colors (owner feedback: awesome; no independent detailed runtime logs)
- [x] Initial Rainbow, Wave and Breathing animations (**owner reports working on keyboard**)
- [x] Gradient, Color Shift, Comet, Sparkle algorithms and UI entries committed (**pending owner build/hardware test**)
- [ ] Validate effects build/behavior, frame pacing and transition edge cases
- [ ] Complete remaining software effects, input-reactive effects and region masks
- [ ] Port Prism-inspired presets with provenance and non-vendor-exact labels
- [ ] Add preset saving/loading and per-effect controls
- [ ] Improve physical key geometry/regions; verify SteelSeries-key HID mapping
- [ ] Hotplug/reconnect handling, error reports, collision avoidance and recovery
- [ ] Native Arch Linux packaging, release CI and installation instructions

Preserve `68 HID addresses` vs `61 photographed physical keycaps` distinction.
