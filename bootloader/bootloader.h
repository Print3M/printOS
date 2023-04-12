#pragma once
#include "font.h"
#include "gop.h"
#include "kernel_starter.h"

typedef struct {
	Framebuffer *framebuffer;
	Psf1_font *font;
	MemoryData *memory;
	void *acpi_rsdp;
} BootloaderData;