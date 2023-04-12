[bits 64]

section .text:
    global enable_cpu_4_level_paging

enable_cpu_4_level_paging:
    ; void enable_cpu_4_level_paging()
    
    ; cr0.PG = 1
    ; Enable paging
    mov rax, cr0
    or  rax, (1 << 31)
    mov cr0, rax  

    ; cr4.PAE = 1
    mov rax, cr4
    or  rax, (1 << 5)
    mov cr4, rax

    ; cr4.LA57 = 0
    ; Disable 57-bit linear addressing mode (5-level paging).
    mov rax, cr4
    and rax, ~(1 << 57)
    mov cr4, rax

    xor rax, rax
    ret
