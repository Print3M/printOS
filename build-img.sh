#!/usr/bin/env bash

set -ueo pipefail

SCRIPT=$(dirname "$0")
BOOTLOADER="$SCRIPT/bootloader/bin/main.efi"
KERNEL="$SCRIPT/kernel/bin/kernel.elf"
STARTUP="$SCRIPT/bootloader/startup.nsh"
KERNEL_FONT="$SCRIPT/kernel/fonts/zap-light16.psf"
OUTPUT_IMG="$SCRIPT/os-image.hdd"

function build_final_img() {
    # Create empty image - sector=512 bytes, sectors number=93750
    dd if=/dev/zero of="$OUTPUT_IMG" bs=512 count=93750

    # Create MS-DOS partition - FAT32 filesystem
    mformat -i "$OUTPUT_IMG" ::

    # Create subdirs for default EFI boot place
    mmd -i "$OUTPUT_IMG" ::/EFI
    mmd -i "$OUTPUT_IMG" ::/EFI/BOOT

    # Copy compiled EFI bootloader to default boot place
    mcopy -i "$OUTPUT_IMG" "$BOOTLOADER" ::/EFI/BOOT

    # Copy startup file into root EFI dir
    mcopy -i "$OUTPUT_IMG" "$STARTUP" ::

    # Copy compiled kernel executable into EFI root dir
    mcopy -i "$OUTPUT_IMG" "$KERNEL" ::

    # Copy psf font file into EFI root dir
    mcopy -i "$OUTPUT_IMG" "$KERNEL_FONT" ::
}

rm -rf "$OUTPUT_IMG"
build_final_img

echo -e "\n[+] Final OS image built successfully!"
echo "    Path: $OUTPUT_IMG"
