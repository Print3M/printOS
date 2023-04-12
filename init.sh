#!/usr/bin/env bash

set -ueo pipefail

SCRIPT=$(dirname "$0")

function init_all() {
    make -C "$SCRIPT/kernel" init
    make -C "$SCRIPT/bootloader" init
}

init_all

echo -e "\n[+] Kernel & bootloader initialized successfully!"