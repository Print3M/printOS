[bits 64]

section .text:
    global get_cpuid
    global write_msr
    global read_msr

get_cpuid:
    ; void cpuid(uint32_t cpuid_eax, uint32_t *eax, uint32_t *ebx, uint32_t *ecx, uint32_t *edx);
    
    ; Save arguments
    mov r10, rsi ; :eax
    mov r11, rdx ; :ebx
    mov r12, rcx ; :ecx
    mov r13, r8  ; :edx
    
    mov eax, edi
    cpuid

    .check_eax_param:
        cmp r10, 0x0 ; If :eax is not NULL ptr
        jne .ret_eax_param
    .check_ebx_param:
        cmp r11, 0x0 ; If :ebx is not NULL ptr
        jne .ret_ebx_param
    .check_ecx_param:
        cmp r12, 0x0 ; If :ecx is not NULL ptr
        jne .ret_ecx_param
    .check_edx_param:
        cmp r13, 0x0 ;If :edx is not NULL ptr
        jne .ret_edx_param

    .return:
    ret

    .ret_eax_param:
        mov dword [r10], eax ; Return argument :eax (esi)
        jmp .check_ebx_param          
    .ret_ebx_param:
        mov dword [r11], ebx ; Return argument :ebx (edx)
        jmp .check_ecx_param
    .ret_ecx_param:
        mov dword [r12], ecx ; Return argument :ecx (ecx)
        jmp .check_edx_param
    .ret_edx_param:
        mov dword [r13], edx  ; Return argument :edx (r8)
        jmp .return

write_msr:
    ; void write_msr(uint64_t msr_no, uint64_t value);    
    ; value: higher 32 bits -> edx && lower 32 bits -> eax
    mov ecx, edi   ; ecx = msr_no
    mov eax, esi   ; lower bits -> eax
    
    ; higher bits -> edx
    mov rdx, rsi
    shr rdx, 32

    wrmsr
    ret

read_msr:
    ; uint64_t read_msr(uint64_t msr_no);
    mov ecx, edi   ; ecx = msr_no
    rdmsr          ; returns 64-bit value stored in edx:eax (both 32-bit)

    shl rdx, 32    ; shift edx to higher bits (0:31 -> 32:64)
    
    ; add edx to higher bits of rax
    or rax, rdx
    ret