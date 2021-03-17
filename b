#!/bin/sh

_clean() {
    [ build ] && rm -rf build
}

_build() {
    meson build
}

_make() {
    ninja -C build
}

main() {
    _clean
    _build
    _make
}

main "${@}"
