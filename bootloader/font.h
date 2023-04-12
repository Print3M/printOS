#pragma once
#include "file.h"
#include <efi.h>

#define GLYPH_WIDTH 8
#define GLYPH_HEIGHT 16

#define PSF1_MAGIC 0x3604
#define PSF1_MODE512 0x01

typedef struct {
	UINT16 magic;
	UINT8 mode;
	UINT8 charsize;
} Psf1_header;

typedef struct {
	Psf1_header *psf_header;
	UINT8 glyph_height;
	UINT8 glyph_width;
	void *glyph_buffer;
} Psf1_font;

Psf1_font *load_psf_font(EFI_FILE *directory, CHAR16 *path, EFI_HANDLE image_handle);