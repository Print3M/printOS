/* 
    GOP stands for Graphic Output Protocol (UEFI feature)
*/
#pragma once
#include <efi.h>

#define BYTES_PER_PIXEL 4

typedef struct {
	void *base_address;
	UINT64 buffer_size;
	UINT16 width;
	UINT16 height;
	UINT16 pixels_per_scanline;
	UINT8 bytes_per_pixel;
} Framebuffer;

Framebuffer *get_framebuffer();

