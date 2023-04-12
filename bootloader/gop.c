#include "gop.h"
#include "utils.h"
#include <efi.h>
#include <efilib.h>

EFI_STATUS locate_gop(EFI_GRAPHICS_OUTPUT_PROTOCOL **gop) {
	EFI_STATUS status = EFI_SUCCESS;

	status = BS->LocateProtocol(&gEfiGraphicsOutputProtocolGuid, NULL, (void **) gop);
	if (status != EFI_SUCCESS) {
		print_efi_err(L"Locate GOP failed", status);
	}

	return status;
}

Framebuffer *get_framebuffer() {
	EFI_GRAPHICS_OUTPUT_PROTOCOL *gop = NULL;
	if (locate_gop(&gop) != EFI_SUCCESS) {
		return NULL;
	}

	// Allocate framebuffer struct
	Framebuffer *framebuffer = NULL;
	EFI_STATUS status =
		BS->AllocatePool(EfiLoaderData, sizeof(Framebuffer), (void **) &framebuffer);
	if (status != EFI_SUCCESS) {
		print_efi_err(L"Allocating memory for framebuffer struct failed", status);
		return NULL;
	}

	framebuffer->base_address = (void *) gop->Mode->FrameBufferBase;
	framebuffer->buffer_size = gop->Mode->FrameBufferSize;
	framebuffer->width = gop->Mode->Info->HorizontalResolution;
	framebuffer->height = gop->Mode->Info->VerticalResolution;
	framebuffer->pixels_per_scanline = gop->Mode->Info->PixelsPerScanLine;
	framebuffer->bytes_per_pixel = BYTES_PER_PIXEL;

	return framebuffer;
}
