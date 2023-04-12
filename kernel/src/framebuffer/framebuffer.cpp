#include <framebuffer/framebuffer.hpp>
#include <kernel.hpp>
#include <libc/stdint.hpp>

Framebuffer::Framebuffer(FramebufferData &data) {
	this->_addr = data.addr;
	this->_height = data.height;
	this->_width = data.width;
	this->_size = data.b_size;
	this->_px_per_scan = data.px_per_scan;
	this->_b_per_px = data.b_per_px;
}

void Framebuffer::set_pixel(u32 x, u32 y, u64 color) {
	// Write :color into calculated framebuffer address
	// address = framebuffer's base address + y offset (from base adress) + x offset (from y)
	u64 addr = reinterpret_cast<u64>(this->_addr) + (y * this->_px_per_scan * this->_b_per_px) +
			   (x * this->_b_per_px);
	u64 *pixel = reinterpret_cast<u64 *>(addr);
	*pixel = color;
}

void Framebuffer::set_all_pixels(u64 color) {
	// Set solid color for entire framebuffer
	for (u16 x = 0; x < this->_width; x++) {
		for (u16 y = 0; y < this->_height; y++) {
			this->set_pixel(x, y, color);
		}
	}
}

void Framebuffer::print_glyph(Glyph &glyph) {
	// Iterate through glyph pixels in glyph bitmap
	for (u16 y = 0; y < glyph.height; y++) {
		for (u16 x = 0; x < glyph.width; x++) {
			// Print bitmap pixels
			u64 color;
			if ((*glyph.bitmap & ((1 << 7) >> x)) > 0) {
				// Glyph color
				color = glyph.fg_color;
			} else {
				// Background color
				color = glyph.bg_color;
			}
			this->set_pixel(x + glyph.x_off, y + glyph.y_off, color);
		}
		glyph.bitmap++;
	}
}

void *Framebuffer::get_address() {
	// Return base memory address of framebuffer
	return this->_addr;
}

size Framebuffer::get_size() {
	// Return size of framebuffer in memory (in bytes)
	return this->_size;
}