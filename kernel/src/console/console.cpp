#include <console/console.hpp>
#include <framebuffer/colors.hpp>
#include <framebuffer/framebuffer.hpp>
#include <kernel.hpp>
#include <kutils/console.hpp>
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

void ConsoleCursor::back() {
	if (this->_col > 0) {
		this->_col -= 1;
	} else {
		this->_col = this->_max_col - 1;
		this->_row -= 1;
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
	this->_bg	   = (u64) ConsoleDefault::bg;
	this->_fg	   = (u64) ConsoleDefault::fg;
	this->_max_col = (u16) ConsoleDefault::max_col;
	this->_max_row = (u16) ConsoleDefault::max_row;
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
	this->_framebuffer.set_all_pixels(BLACK);
	this->cursor.reset();

	this->_framebuffer.draw();
}

void Console::clear_line(u16 line) {
	// Indexing from 0
	for (size x = 0; x < this->_framebuffer.width; x++) {
		for (size y = 0; y < this->_font.glyph_height; y++) {
			this->_framebuffer.set_pixel(x, line * this->_font.glyph_height + y, BLACK);
		}
	}

	this->_framebuffer.draw();
}

void Console::_print_char(char chr) {
	if (this->cursor.max_row == this->cursor.row) {
		this->_scroll_down();
	}

	if (chr >= ASCII_PRINTABLE_MIN && chr <= ASCII_PRINTABLE_MAX) [[likely]] {
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

		return;
	}

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
		case '\b': {
			this->cursor.back();
			this->_print_char(' ');
			this->cursor.back();
			break;
		}
	}
}

void Console::print(char chr) {
	this->_print_char(chr);
	this->_framebuffer.draw();
}

void Console::print(char *str) {
	while (*str != '\0') {
		this->_print_char(*str);
		str++;
	}

	this->_framebuffer.draw();
}

void Console::_scroll_down() {
	auto fb	   = this->_framebuffer.get_double_fb_addr();
	auto start = this->_framebuffer.get_pixel_addr(0, this->_font.glyph_height);
	auto size  = reinterpret_cast<u64>(start) - reinterpret_cast<u64>(fb);
	memcpy(fb, start, this->_framebuffer.b_size - size);

	this->cursor.gotoxy(this->cursor.col, this->cursor.row - 1);
	this->clear_line(this->cursor.max_row - 1);
}
