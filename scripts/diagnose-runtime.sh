#!/usr/bin/env bash
set -uo pipefail

plugin="${XDG_CONFIG_HOME:-$HOME/.config}/OpenRGB/plugins/OpenRGB-ApexProMini.so"
built="build-plugin/OpenRGB-ApexProMini.so"

echo "== Executables on PATH =="
type -a openrgb 2>&1 || true
echo "== OpenRGB processes =="
ps -eo pid,ppid,args | grep -Ei '[o]penrgb|[f]latpak.*openrgb' || true
for pid in $(pgrep -fi '(^|/)(openrgb)( |$)|org.openrgb.OpenRGB' || true); do
    if [[ -e "/proc/$pid/exe" ]]; then
        printf 'PID %s executable: ' "$pid"
        readlink -f "/proc/$pid/exe" 2>/dev/null || true
    fi
done
echo "== Flatpak installation =="
flatpak list --app --columns=application,version 2>/dev/null | grep -i openrgb || true
echo "== Git HEAD =="
git log -1 --format='%h %s' 2>/dev/null || true
echo "== Binary hashes =="
for f in "$built" "$plugin"; do
    if [[ -f "$f" ]]; then sha256sum "$f"; else echo "Missing: $f"; fi
done
echo "== Installed plugin metadata =="
if [[ -x build-plugin/apex_plugin_metadata_check && -f "$plugin" ]]; then
    build-plugin/apex_plugin_metadata_check "$plugin" || true
else
    echo "Metadata checker or installed plugin not found"
fi
echo "== Qt dependencies =="
if [[ -f "$plugin" ]]; then
    ldd "$plugin" | grep -E '(Qt[56]|not found|hidapi)' || true
fi
echo "== Runtime check suggestion =="
echo 'Exit all OpenRGB instances, then run the compiled OpenRGB binary by its absolute path.'
echo 'Capture its complete new warnings and errors, not historical logs.'
