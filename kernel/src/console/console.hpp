#pragma once
#include <framebuffer/colors.hpp>
#include <framebuffer/framebuffer.hpp>
#include <libc/stdint.hpp>

// TODO: Remove
#define CURSOR_MAX_LINES   37
#define CURSOR_MAX_COLUMNS 100

struct __attribute__((packed)) PsfHeader {
	u8 magic[2];
	u8 mode;
	u8 charsize;
};

struct ConsoleFont {
	PsfHeader *psf_header;
	u8 glyph_height;
	u8 glyph_width;
	void *glyph_buffer;
};

enum ConsoleDefaults {
	max_col = 100,
	max_row = 37,
	tab_spaces = 4,
	fg = WHITE,
	bg = BLACK,
};

class ConsoleCursor {
  private:
	u16 _row = 0;
	u16 _col = 0;
	u64 _fg = ConsoleDefaults::fg;
	u64 _bg = ConsoleDefaults::bg;
	u16 _max_col = ConsoleDefaults::max_col;
	u16 _max_row = ConsoleDefaults::max_row;

  public:
	const u16 &row = _row;
	const u16 &col = _col;
	const u64 &fg = _fg;
	const u64 &bg = _bg;

  public:
	ConsoleCursor();
	void next();
	void gotoxy(u16 x, u16 y);
	void cr(); // Carriege return
	void lf(); // Line feed
	void set_fg(u64 fg);
	void set_bg(u64 bg);
	void reset();
};

class Console {
  private:
	ConsoleFont &_font;
	Framebuffer &_framebuffer;

  public:
	ConsoleCursor cursor = ConsoleCursor();

  public:
	Console(ConsoleFont &font, Framebuffer &framebuffer);
	void print(char *str);
	void print(char chr);
	void clear();
};