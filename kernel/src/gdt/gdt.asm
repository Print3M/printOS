[bits 64]

section .text:
    global __load_gdt

__load_gdt:
    lgdt [rdi] ; Load GDT

    ; Set segment registers to kernel data segment
    ; (2nd position in GDT = 0x10) 
    mov ax, 0x10
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    mov ss, ax

    ; Far return to segment:offset (from stack)
    ; in order to reload segment registers.
    ; Put on stack 0x08:ret_addr:
    pop rdi
    mov rax, 0x08
    push rax
    push rdi

    ; Call qword far return:
    ;   It gets two values from stack and loads 
    ;   them into CS (Code Segment) and IP registers.
    retfq 