#include <console/console.hpp>
#include <framebuffer/colors.hpp>
#include <framebuffer/framebuffer.hpp>
#include <kernel.hpp>
#include <libc/stdarg.hpp>
#include <libc/stdint.hpp>
#include <libc/stdlib.hpp>

ConsoleCursor::ConsoleCursor() {
	// Initialize state
	this->reset();
}

void ConsoleCursor::next() {
	if (this->_col + 1 < this->_max_col) {
		this->_col += 1;
	} else {
		this->_col = 0;
		this->_row += 1;
	}
}

void ConsoleCursor::cr() {
	// Carrieg return
	this->_col = 0;
}

void ConsoleCursor::lf() {
	// Line feed
	this->_row += 1;
}

void ConsoleCursor::gotoxy(u16 x, u16 y) {
	this->_col = x;
	this->_row = y;
}

void ConsoleCursor::reset() {
	this->_col	   = 0;
	this->_row	   = 0;
	this->_bg	   = ConsoleDefaults::bg;
	this->_fg	   = ConsoleDefaults::fg;
	this->_max_col = ConsoleDefaults::max_col;
	this->_max_row = ConsoleDefaults::max_row;
}

void ConsoleCursor::set_bg(u64 color) {
	// Set background
	this->_bg = color;
}

void ConsoleCursor::set_fg(u64 color) {
	// Set foreground
	this->_fg = color;
}

Console::Console(ConsoleFont &font, Framebuffer &framebuffer)
	: _font(font), _framebuffer(framebuffer) {
}

void Console::clear() {
	// Clear console and reset cursor
	this->_framebuffer.set_all_pixels(0x0);
	this->cursor.reset();
}

void Console::print(char chr) {
	switch (chr) {
		case '\n': {
			this->cursor.lf();
			this->cursor.cr();
			break;
		}
		case '\t': {
			this->cursor.next();
			this->cursor.next();
			this->cursor.next();
			break;
		}
		case '\r': {
			this->cursor.cr();
			break;
		}
		default: {
			// Get glyph bitmap ptr
			int offset		   = chr * this->_font.psf_header->charsize;
			char *glyph_bitmap = reinterpret_cast<char *>((size) this->_font.glyph_buffer + offset);

			Glyph glyph = {
				.bitmap	  = glyph_bitmap,
				.x_off	  = this->cursor.col * this->_font.glyph_width,
				.y_off	  = this->cursor.row * this->_font.glyph_height,
				.height	  = this->_font.glyph_height,
				.width	  = this->_font.glyph_width,
				.fg_color = this->cursor.fg,
				.bg_color = this->cursor.bg,
			};
			this->_framebuffer.print_glyph(glyph);
			this->cursor.next();
		}
	}
}

void Console::print(char *str) {
	while (*str != '\0') {
		this->print(*str);
		str++;
	}
}