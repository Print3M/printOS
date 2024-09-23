#pragma once
#include <libc/stdint.hpp>

namespace ScanCode {

constexpr u8 ESCAPE_IN		 = 0x01;
constexpr u8 BACKSPACE_IN	 = 0x0e;
constexpr u8 TAB_IN			 = 0x0f;
constexpr u8 ENTER_IN		 = 0x1C;
constexpr u8 LEFT_SHIFT_IN	 = 0x2a;
constexpr u8 RIGHT_SHIFT_IN	 = 0x36;
constexpr u8 SPACE_IN		 = 0x39;
constexpr u8 CAPSLOCK_IN	 = 0x3a;
constexpr u8 LEFT_SHIFT_OUT	 = 0xaa;
constexpr u8 RIGHT_SHIFT_OUT = 0xb6;

} // namespace ScanCode

namespace ASCII {

constexpr u8 NULL_CHAR		 = 0x00;
constexpr u8 BACKSPACE		 = 0x08;
constexpr u8 HORIZONTAL_TAB	 = 0x09;
constexpr u8 LINE_FEED		 = 0x0a;
constexpr u8 CARRIAGE_RETURN = 0x0d;
constexpr u8 SHIFT_OUT		 = 0x0e;
constexpr u8 SHIFT_IN		 = 0x0f;
constexpr u8 ESCAPE			 = 0x1b;
constexpr u8 SPACE			 = 0x20;

} // namespace ASCII

class Keyboard {
  private:
	inline char _mod_letter(char chr, char mod_chr);
	inline char _mod_special(char chr, char mod_chr);

  public:
	bool shift	  = false;
	bool capslock = false;

  public:
	char read_key();
	bool is_mod();
};