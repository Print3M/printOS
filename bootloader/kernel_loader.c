#include "kernel_loader.h"
#include "file.h"
#include "utils.h"
#include <efi.h>
#include <efilib.h>
#include <elf.h>

BOOLEAN verify_kernel_elf_header(Elf64_Ehdr *e_hdr) {
	return memcmp((void *) (UINTN) e_hdr->e_ident[EI_MAG0], ELFMAG, SELFMAG) == 0 &&
		   e_hdr->e_ident[EI_CLASS] == ELFCLASS64 && e_hdr->e_ident[EI_DATA] == ELFDATA2LSB &&
		   e_hdr->e_type == ET_EXEC && e_hdr->e_machine == EM_X86_64 &&
		   e_hdr->e_version == EV_CURRENT;
}

EFI_STATUS get_elf_header(EFI_FILE *file, Elf64_Ehdr *e_hdr) {
	EFI_STATUS status = EFI_SUCCESS;
	UINTN e_hdr_size  = sizeof(Elf64_Ehdr);

	status = file->SetPosition(file, 0);
	if (status != EFI_SUCCESS) {
		print_efi_err(L"Jumping at the beginning of ELF failed", status);
		return status;
	}

	status = file->Read(file, &e_hdr_size, (void *) e_hdr);
	if (status != EFI_SUCCESS) {
		print_efi_err(L"Reading ELF header failed", status);
	}

	return status;
}

EFI_STATUS get_elf_p_hdrs(EFI_FILE *file, Elf64_Ehdr *e_hdr, Elf64_Phdr **p_hdrs) {
	EFI_STATUS status	= EFI_SUCCESS;
	UINTN all_p_hdrs_sz = e_hdr->e_phentsize * e_hdr->e_phnum; // Size of all program headers

	status = BS->AllocatePool(EfiLoaderData, all_p_hdrs_sz, (void **) p_hdrs);
	if (status != EFI_SUCCESS) {
		print_efi_err(L"Allocating memory for ELF p_hdrs failed", status);
		return status;
	}
	status = file->SetPosition(file, e_hdr->e_phoff);
	if (status != EFI_SUCCESS) {
		print_efi_err(L"Jumping at e_phoff in ELF file failed", status);
		return status;
	}
	status = file->Read(file, &all_p_hdrs_sz, (void *) *p_hdrs);
	if (status != EFI_SUCCESS) {
		print_efi_err(L"Reading ELF program headers failed", status);
		return status;
	}
	return status;
}

Elf64_Phdr *_get_next_p_hdr(Elf64_Phdr *p_hdr, UINTN p_hdr_size) {
	return (Elf64_Phdr *) ((UINTN) p_hdr + p_hdr_size);
}

EFI_STATUS load_elf_segment_by_p_hdr(EFI_FILE *file, Elf64_Phdr *p_hdr) {
	EFI_STATUS status = EFI_SUCCESS;

	// Allocate pages for segment at its physical address
	UINT16 num_of_pages		 = (p_hdr->p_filesz / PAGE_SZ) + 1;
	Elf64_Addr segment_paddr = p_hdr->p_paddr;

	status = BS->AllocatePages(AllocateAddress, EfiLoaderData, num_of_pages, &segment_paddr);
	if (status != EFI_SUCCESS) {
		print_efi_err(L"Memory allocation for ELF segment failed", status);
		return status;
	}

	// Read segment and load into allocated memory
	status = file->SetPosition(file, p_hdr->p_offset);
	if (status != EFI_SUCCESS) {
		print_efi_err(L"Jumping at segment offset", status);
		return status;
	}

	UINTN segment_file_sz = p_hdr->p_filesz;
	status				  = file->Read(file, &segment_file_sz, (void *) segment_paddr);
	if (status != EFI_SUCCESS) {
		print_efi_err(L"Reading one of ELF's PT_LOAD segments failed", status);
	}
	return status;
}

void *load_kernel_into_memory(EFI_FILE *kernel_file) {
	/*
		Returns entry point to the kernel.
		IMPORTANT: Kernel is loaded into EfiLoaderData memory type!
	*/

	// Read ELF file header
	Elf64_Ehdr e_hdr;
	if (get_elf_header(kernel_file, &e_hdr) != EFI_SUCCESS) {
		return NULL;
	}

	// Verify ELF header
	if (verify_kernel_elf_header(&e_hdr)) {
		print_err(L"Kernel ELF format is bad");
		return NULL;
	}

	// Get ELF program headers
	Elf64_Phdr *p_hdrs = NULL;
	if (get_elf_p_hdrs(kernel_file, &e_hdr, &p_hdrs) != EFI_SUCCESS) {
		return NULL;
	}

	// Get first program header
	Elf64_Phdr *p_hdr = p_hdrs;

	// Iterate through ELF program headers and load PT_LOAD segments
	for (int i = 0; i < e_hdr.e_phnum; i++) {
		if (i > 0) {
			p_hdr = _get_next_p_hdr(p_hdr, e_hdr.e_phentsize);
		}

		if (p_hdr->p_type == PT_LOAD) {
			if (load_elf_segment_by_p_hdr(kernel_file, p_hdr) != EFI_SUCCESS) {
				return NULL;
			}
		}
	}

	return (void *) e_hdr.e_entry;
}