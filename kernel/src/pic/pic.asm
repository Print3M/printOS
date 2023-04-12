[bits 64]

section .text:
    global __disable_pic

__disable_pic:
    ; Source: https://wiki.osdev.org/8259_PIC
    mov al, 0xff
    out 0xa1, al
    out 0x21, al
    ret
