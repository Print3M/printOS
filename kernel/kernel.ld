/*
    Script has been copied from:
        https://github.com/lucic71/lucicOS/blob/master/kernel/arch/i386/linker.ld
*/
OUTPUT_FORMAT(elf64-x86-64)
ENTRY(_start)

SECTIONS {

    /*
     * The code should be loaded at an address greater or equal to 1MB because
     * lower addresses are reserved to GRUB, BIOS and memory mapped I/O.
     *
     */

    . = 0x100000; /* Where the kernel is loaded into physical memory /*

    /* Align all sections from all files to a 4KB boundary. */

    _kernel_start = .;
    _kernel_text_start = .;

    .text ALIGN (0x1000) : {
        *(.multiboot)
        *(.text)
    }

    _kernel_text_end = .;
    _kernel_rodata_start = .;

    .rodata ALIGN (0x1000) : {
        *(.rodata*)
    }

    _kernel_rodata_end = .;
    _kernel_data_start = .;

    .data ALIGN (0x1000) : {
        *(.data)
    }

    _kernel_data_end = .;
    _kernel_bss_start = .;

    .bss ALIGN (0x1000) : {
        *(COMMON)
        *(.bss)
    }

    _kernel_bss_end = .;

    . = ALIGN(4K);
    _kernel_end = .;

    /DISCARD/ : {
        *(.comment)
        *(.note.gnu.build-id)
    }

    _kernel_size = _kernel_end - _kernel_start;

}