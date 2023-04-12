#include "font.h"
#include "file.h"
#include "utils.h"
#include <efi.h>
#include <efilib.h>
#include <elf.h>

EFI_STATUS get_psf_header(EFI_FILE *psf_file, Psf1_header **psf_header) {
	EFI_STATUS status = EFI_SUCCESS;
	UINTN psf_header_sz = sizeof(Psf1_header);

	status = ST->BootServices->AllocatePool(EfiLoaderData, psf_header_sz, (void **) psf_header);
	if (status != EFI_SUCCESS) {
		print_efi_err(L"Allocating memory for PSF header failed", status);
		return status;
	}
	status = psf_file->Read(psf_file, &psf_header_sz, (void *) *psf_header);
	if (status != EFI_SUCCESS) {
		print_efi_err(L"Reading PSF header failed", status);
	}
	return status;
}

BOOLEAN verify_psf_header(Psf1_header *psf_header) { return psf_header->magic == PSF1_MAGIC; }

EFI_STATUS get_psf_glyphs(EFI_FILE *psf_file, UINTN glyphs_buf_sz, void **glyphs_buf) {
	EFI_STATUS status = EFI_SUCCESS;

	status = psf_file->SetPosition(psf_file, sizeof(Psf1_header));
	if (status != EFI_SUCCESS) {
		print_efi_err(L"Jumping at PSF glyphs buffer failed", status);
		return status;
	}
	status = ST->BootServices->AllocatePool(EfiLoaderData, glyphs_buf_sz, (void **) glyphs_buf);
	if (status != EFI_SUCCESS) {
		print_efi_err(L"Allocating memory for PSF glyphs failed", status);
		return status;
	}
	status = psf_file->Read(psf_file, &glyphs_buf_sz, (void *) *glyphs_buf);
	if (status != EFI_SUCCESS) {
		print_efi_err(L"Reading PSF glyphs failed", status);
	}
	return status;
}

Psf1_font *
load_psf_font(EFI_FILE *dir, CHAR16 *path, EFI_HANDLE image_handle) {
	/*
		Read more about PSF fonts:
			https://www.win.tue.nl/~aeb/linux/kbd/font-formats-1.html
	*/

	// Load .psf font file
	EFI_FILE *psf_file = load_file(dir, path, image_handle);
	if (psf_file == NULL) {
		print_err(L"Loading PSF file failed");
		return NULL;
	}

	// Get .psf file's header
	Psf1_header *psf_header = NULL;
	if (get_psf_header(psf_file, &psf_header) != EFI_SUCCESS) {
		return NULL;
	}

	// Verify .psf format header
	if (verify_psf_header(psf_header)) {
		print_err(L"Invalid PSF file header");
		return NULL;
	}

	// Buffer size for all glyphs (check if 512-mode is enabled)
	UINTN glyphs_buf_sz;
	if (psf_header->mode == PSF1_MODE512) {
		glyphs_buf_sz = psf_header->charsize * 512;
	} else {
		glyphs_buf_sz = psf_header->charsize * 256;
	}

	// Allocate memory and load entire glyphs buffer of psf file
	void *glyphs_buf = NULL;
	if (get_psf_glyphs(psf_file, glyphs_buf_sz, &glyphs_buf) != EFI_SUCCESS) {
		return NULL;
	}

	// Allocate memory for psf_font struct
	Psf1_font *psf_font = NULL;
	EFI_STATUS status =
		ST->BootServices->AllocatePool(EfiLoaderData, sizeof(Psf1_font), (void **) &psf_font);
	if (status != EFI_SUCCESS) {
		print_efi_err(L"Allocating memory for PSF font struct failed", status);
		return NULL;
	}

	psf_font->psf_header = psf_header;
	psf_font->glyph_buffer = glyphs_buf;
	psf_font->glyph_height = GLYPH_HEIGHT;
	psf_font->glyph_width = GLYPH_WIDTH;

	return psf_font;
}