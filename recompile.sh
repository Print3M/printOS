#!/usr/bin/env bash

set -ueo pipefail

SCRIPT=$(dirname "$0")

function compile_all() {
    make -C "$SCRIPT/kernel"
    make -C "$SCRIPT/bootloader"
}

function clean_all() {
    make -C "$SCRIPT/kernel" clean
    make -C "$SCRIPT/bootloader" clean
}

clean_all
compile_all

echo -e "\n[+] Kernel & bootloader recompiled successfully!"