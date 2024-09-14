#pragma once
#include <libc/stdint.hpp>

struct __attribute__((packed)) FramebufferData {
	void *base_addr;
	void *double_buffer;
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
	size _b_size;	  // Size of framebuffer in bytes
	u16 _width;		  // Width in pixels
	u16 _height;	  // Height in pixels
	u16 _px_per_scan; // Pixels per scanline
	u8 _b_per_px;	  // Bytes per pixels

	void *__fb_addr;				  // Base memory address of framebuffer
	void *__double_fb_addr = nullptr; // Double framebuffer

  public:
	const u16 &width   = _width;
	const u16 &height  = _height;
	const size &b_size = _b_size;
	const u8 &b_per_px = _b_per_px;

  public:
	Framebuffer(FramebufferData &data);
	void set_pixel(u32 x, u32 y, u64 color);
	void set_all_pixels(u64 color);
	void print_glyph(Glyph &glyph);
	void *get_fb_addr();
	void *get_double_fb_addr();
	void draw();
};
