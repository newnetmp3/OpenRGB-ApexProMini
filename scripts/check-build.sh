#!/usr/bin/env bash
set -euo pipefail

# Offline checks do not require the keyboard or OpenRGB SDK.
cmake -S . -B build -DCMAKE_BUILD_TYPE=Debug
cmake --build build --parallel
ctest --test-dir build --output-on-failure

if [[ "${1:-}" == "--plugin" ]]; then
    if [[ -z "${OPENRGB_SOURCE_DIR:-}" ]]; then
        echo "Set OPENRGB_SOURCE_DIR to the matching OpenRGB source checkout" >&2
        exit 2
    fi
    cmake -S . -B build-plugin -DBUILD_OPENRGB_PLUGIN=ON \
        -DOPENRGB_SOURCE_DIR="$OPENRGB_SOURCE_DIR" \
        -DCMAKE_BUILD_TYPE=Debug
    cmake --build build-plugin --parallel
    ctest --test-dir build-plugin --output-on-failure
    ./build-plugin/apex_plugin_metadata_check "$(realpath build-plugin/OpenRGB-ApexProMini.so)"
fi
