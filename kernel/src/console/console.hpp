#pragma once
#include <framebuffer/colors.hpp>
#include <framebuffer/framebuffer.hpp>
#include <libc/stdint.hpp>

struct __attribute__((packed)) PsfHeader {
	u8 magic[2];
	u8 mode;
	u8 charsize;
};

constexpr u8 ASCII_PRINTABLE_MIN = 32;
constexpr u8 ASCII_PRINTABLE_MAX = 127;

struct ConsoleFont {
	PsfHeader *psf_header;
	u8 glyph_height;
	u8 glyph_width;
	void *glyph_buffer;
};

enum class ConsoleDefault {
	max_col	   = 100,
	max_row	   = 37,
	tab_spaces = 4,
	fg		   = WHITE,
	bg		   = BLACK,
};

class ConsoleCursor {
  private:
	u16 _row	 = 0;
	u16 _col	 = 0;
	u64 _fg		 = (u64) ConsoleDefault::fg;
	u64 _bg		 = (u64) ConsoleDefault::bg;
	u16 _max_col = (u16) ConsoleDefault::max_col;
	u16 _max_row = (u16) ConsoleDefault::max_row;

  public:
	const u16 &row	   = _row;
	const u16 &col	   = _col;
	const u64 &fg	   = _fg;
	const u64 &bg	   = _bg;
	const u16 &max_col = _max_col;
	const u16 &max_row = _max_row;

  public:
	ConsoleCursor();
	void next();
	void back();
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

  private:
	void _print_char(char chr);
	void _scroll_down();

  public:
	/*
		All public methods of Console class should execute
		framebuffer.draw() method implicitly. It's not
		guaranteed in case of private methods!
	*/
	Console(ConsoleFont &font, Framebuffer &framebuffer);
	void print(char *str);
	void print(char chr);
	void clear_line(u16 line);
	void clear();
};