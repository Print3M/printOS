#include <drivers/keyboard.hpp>
#include <drivers/ps2.hpp>
#include <libc/stdint.hpp>

char Keyboard::read_key() {
	auto code = ps2::controller::read_data();

	// Map scan code to ASCII
	switch (code) {
		case (0x2):
			return this->_mod_special('1', '!');
		case (0x3):
			return this->_mod_special('2', '@');
		case (0x4):
			return this->_mod_special('3', '#');
		case (0x5):
			return this->_mod_special('4', '$');
		case (0x6):
			return this->_mod_special('5', '%');
		case (0x7):
			return this->_mod_special('6', '^');
		case (0x8):
			return this->_mod_special('7', '&');
		case (0x9):
			return this->_mod_special('8', '*');
		case (0xA):
			return this->_mod_special('9', '(');
		case (0xB):
			return this->_mod_special('0', ')');
		case (0xC):
			return this->_mod_special('-', '_');
		case (0xD):
			return this->_mod_special('=', '+');
		case (0x10):
			return this->_mod_letter('q', 'Q');
		case (0x11):
			return this->_mod_letter('w', 'W');
		case (0x12):
			return this->_mod_letter('e', 'E');
		case (0x13):
			return this->_mod_letter('r', 'R');
		case (0x14):
			return this->_mod_letter('t', 'T');
		case (0x15):
			return this->_mod_letter('y', 'Y');
		case (0x16):
			return this->_mod_letter('u', 'U');
		case (0x17):
			return this->_mod_letter('i', 'I');
		case (0x18):
			return this->_mod_letter('o', 'O');
		case (0x19):
			return this->_mod_letter('p', 'P');
		case (0x1A):
			return this->_mod_special('[', '{');
		case (0x1B):
			return this->_mod_special(']', '}');
		case (0x1E):
			return this->_mod_letter('a', 'A');
		case (0x1F):
			return this->_mod_letter('s', 'S');
		case (0x20):
			return this->_mod_letter('d', 'D');
		case (0x21):
			return this->_mod_letter('f', 'F');
		case (0x22):
			return this->_mod_letter('g', 'G');
		case (0x23):
			return this->_mod_letter('h', 'H');
		case (0x24):
			return this->_mod_letter('j', 'J');
		case (0x25):
			return this->_mod_letter('k', 'K');
		case (0x26):
			return this->_mod_letter('l', 'L');
		case (0x27):
			return this->_mod_special(';', ':');
		case (0x28):
			return this->_mod_special('\'', '"');
		case (0x29):
			return this->_mod_special('`', '~');
		case (0x2B):
			return this->_mod_special('\\', '|');
		case (0x2C):
			return this->_mod_letter('z', 'Z');
		case (0x2D):
			return this->_mod_letter('x', 'X');
		case (0x2E):
			return this->_mod_letter('c', 'C');
		case (0x2F):
			return this->_mod_letter('v', 'V');
		case (0x30):
			return this->_mod_letter('b', 'B');
		case (0x31):
			return this->_mod_letter('n', 'N');
		case (0x32):
			return this->_mod_letter('m', 'M');
		case (0x33):
			return this->_mod_special(',', '<');
		case (0x34):
			return this->_mod_special('.', '>');
		case (0x35):
			return this->_mod_special('/', '?');
		case (ScanCode::ENTER_IN):
			return ASCII::LINE_FEED;
		case (ScanCode::TAB_IN):
			return ASCII::HORIZONTAL_TAB;
		case (ScanCode::BACKSPACE_IN):
			return ASCII::BACKSPACE;
		case (ScanCode::ESCAPE_IN):
			return ASCII::ESCAPE;
		case (ScanCode::SPACE_IN):
			return ASCII::SPACE;
		case (ScanCode::LEFT_SHIFT_IN):
		case (ScanCode::RIGHT_SHIFT_IN):
			this->shift = true;
			return ASCII::SHIFT_IN;
		case (ScanCode::LEFT_SHIFT_OUT):
		case (ScanCode::RIGHT_SHIFT_OUT):
			this->shift = false;
			return ASCII::SHIFT_OUT;
		case (ScanCode::CAPSLOCK_IN):
			this->capslock = !this->capslock;
			break;
		default:
			return ASCII::NULL_CHAR;
	}

	return ASCII::NULL_CHAR;
}

inline char Keyboard::_mod_letter(char chr, char mod_chr) {
	if (this->capslock) {
		if (this->shift) {
			return chr;
		} else {
			return mod_chr;
		}
	} else {
		if (this->shift) {
			return mod_chr;
		} else {
			return chr;
		}
	}

	return chr;
}

inline char Keyboard::_mod_special(char chr, char mod_chr) {
	if (this->shift) {
		return mod_chr;
	} else {
		return chr;
	}
}
