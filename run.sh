#!/usr/bin/env bash

SCRIPT=$(dirname "$0")
OVMF_DIR="$SCRIPT/ovmf"

qemu-system-x86_64 \
    -drive format=raw,file=os-image.hdd \
    -m 256M \
    -smp 1,cores=1,threads=1 \
    -drive format=raw,if=pflash,unit=0,file="$OVMF_DIR/OVMF_CODE.fd",readonly=on \
    -drive format=raw,if=pflash,unit=1,file="$OVMF_DIR/OVMF_VARS.fd" \
    -net none \
    -monitor stdio \
    -no-reboot \
    # -d int
