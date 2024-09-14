#include "acpi.h"
#include "bootloader.h"
#include "file.h"
#include "font.h"
#include "gop.h"
#include "kernel_loader.h"
#include "kernel_starter.h"
#include "utils.h"
#include <efi.h>
#include <efilib.h>
#include <elf.h>

EFI_STATUS efi_main(EFI_HANDLE image_handle, EFI_SYSTEM_TABLE *system_table) {
	InitializeLib(image_handle, system_table);

	Print(L"[*] Starting bootloader... \n\r");

	EFI_FILE *root_dir	  = NULL;
	EFI_FILE *kernel_file = load_file(root_dir, L"kernel.elf", image_handle);
	if (kernel_file == NULL) {
		print_err(L"Loading kernel file error");
		return EFI_LOAD_ERROR;
	}
	Print(L"[+] Kernel file has been loaded successfully \n\r");

	void *kernel_addr = load_kernel_into_memory(kernel_file);
	if (kernel_addr == NULL) {
		print_err(L"Loading kernel into memory error");
		return EFI_LOAD_ERROR;
	}
	Print(L"[+] Kernel has been successfully loaded into memory \n\r");

	Psf1_font *psf_font = load_psf_font(root_dir, L"zap-light16.psf", image_handle);
	if (psf_font == NULL) {
		print_err(L"Psf font loading error");
		return EFI_LOAD_ERROR;
	}
	Print(L"[+] Psf font loaded successfully \n\r");

	Framebuffer *framebuffer = get_framebuffer();
	if (framebuffer == NULL) {
		print_err(L"Framebuffer initialization error");
		return EFI_LOAD_ERROR;
	}
	Print(L"[+] Framebuffer initialized successfully \n\r");

	framebuffer->double_buffer = allocate_double_framebuffer(framebuffer->buffer_size);
	if (framebuffer->double_buffer == NULL) {
		print_err(L"Double framebuffer allocation error");
		return EFI_LOAD_ERROR;
	}
	Print(L"[+] Double framebuffer allocated successfully \n\r");

	void *acpi_rsdp = get_rsdp();
	if (acpi_rsdp == NULL) {
		print_err(L"Obtaining RSDP of ACPI failed");
		return EFI_LOAD_ERROR;
	}
	Print(L"[+] RSDP of ACPI obtained successfully \n\r");

	Print(L"[*] Starting kernel... \n\r");
	start_kernel(image_handle, framebuffer, psf_font, kernel_addr, acpi_rsdp);

	return EFI_SUCCESS;
}
