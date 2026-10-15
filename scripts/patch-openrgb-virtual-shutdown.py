#!/usr/bin/env python3
"""Guarded local OpenRGB source patch for virtual-controller worker shutdown.

OpenRGB creates RGBController_Virtual on behalf of plugins. Some revisions
leave its destructor empty; RGBController::~RGBController then sees a live
worker and can race plugin teardown. Shutdown() is not exported from OpenRGB
to dynamically loaded modules, so this call belongs *inside the host*.
"""
import pathlib
import sys

if len(sys.argv) != 2:
    sys.exit("Usage: python3 scripts/patch-openrgb-virtual-shutdown.py ~/local-git/OpenRGB")
root = pathlib.Path(sys.argv[1]).expanduser().resolve()
path = root / "RGBController" / "RGBController_Virtual.cpp"
if not path.is_file():
    sys.exit(f"Not found: {path}")
raw = path.read_text(encoding="utf-8")
original = """RGBController_Virtual::~RGBController_Virtual()
{

}"""
replacement = """RGBController_Virtual::~RGBController_Virtual()
{
    // The host owns the worker thread; stop it before destroying the derived
    // virtual controller and before the plugin's HID callbacks are unloaded.
    Shutdown();
}"""
if replacement in raw:
    print(f"Already patched: {path}")
    sys.exit(0)
if raw.count(original) != 1:
    sys.exit("OpenRGB source differs from the verified upstream revision; NOT patching. Inspect RGBController_Virtual::~RGBController_Virtual() manually.")
backup = path.with_suffix(path.suffix + ".before-apex-mini-shutdown.bak")
if not backup.exists():
    backup.write_text(raw, encoding="utf-8")
path.write_text(raw.replace(original, replacement, 1), encoding="utf-8")
print(f"Patched: {path}")
print(f"Backup: {backup}")
print("Now rebuild OpenRGB itself (cmake --build <OpenRGB-build-dir> --parallel).")
