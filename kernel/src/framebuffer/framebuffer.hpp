#pragma once
#include <libc/stdint.hpp>

struct __attribute__((packed)) FramebufferData {
	void *addr;
	size b_size; // Size in bytes
	u16 width;
	u16 height;
	u16 px_per_scan;
	u8 b_per_px;
};

struct Glyph {
	char *bitmap;
	u16 x_off;
	u16 y_off;
	u8 height;
	u8 width;
	u32 fg_color;
	u32 bg_color;
};

class Framebuffer {
  private:
	void *_addr;	  // Base memory address of framebuffer
	size _size;		  // Size of framebuffer in bytes
	u16 _width = 30;  // Width in pixels
	u16 _height;	  // Height in pixels
	u16 _px_per_scan; // Pixels per scanline
	u8 _b_per_px;	  // Bytes per pixels

  public:
	Framebuffer(FramebufferData &data);
	void set_pixel(u32 x, u32 y, u64 color);
	void set_all_pixels(u64 color);
	void print_glyph(Glyph &glyph);
	void *get_address();
	size get_size();
};
