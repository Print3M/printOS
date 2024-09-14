#include <framebuffer/framebuffer.hpp>
#include <kernel.hpp>
#include <kutils/assertions.hpp>
#include <libc/stdint.hpp>
#include <libc/stdlib.hpp>

Framebuffer::Framebuffer(FramebufferData &data) {
	this->__fb_addr		   = data.base_addr;
	this->__double_fb_addr = data.double_buffer;
	this->_height		   = data.height;
	this->_width		   = data.width;
	this->_b_size		   = data.b_size;
	this->_px_per_scan	   = data.px_per_scan;
	this->_b_per_px		   = data.b_per_px;

	ASSERT(this->__fb_addr != nullptr);
}

void Framebuffer::set_pixel(u32 x, u32 y, u64 color) {
	// Write :color into calculated framebuffer address
	// address = framebuffer's base address + y offset (from base adress) + x offset (from y)
	u64 addr = reinterpret_cast<u64>(this->__double_fb_addr) +
			   (y * this->_px_per_scan * this->_b_per_px) + (x * this->_b_per_px);
	u64 *pixel = reinterpret_cast<u64 *>(addr);
	*pixel	   = color;
}

void Framebuffer::set_all_pixels(u64 color) {
	// Set solid color for entire framebuffer
	for (u16 x = 0; x < this->_width; x++) {
		for (u16 y = 0; y < this->_height; y++) {
			this->set_pixel(x, y, color);
		}
	}

	this->draw();
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

void *Framebuffer::get_fb_addr() {
	// Return base address of the framebuffer
	return this->__fb_addr;
}

void *Framebuffer::get_double_fb_addr() {
	// Return base address of the double framebuffer
	return this->__double_fb_addr;
}

void Framebuffer::draw() {
	/*
		If a double-buffer is initialized, copy its memory to the framebuffer.
		Otherwise, the framebuffer was used directly and do nothing here.
	*/

	// TODO: It's slow :/ Why? Maybe inefficient memcpy implementation
	memcpy(this->__fb_addr, this->__double_fb_addr, this->_b_size);
}