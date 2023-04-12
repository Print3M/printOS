#include <libc/stdint.hpp>

u64 rgb_to_int(u8 red, u8 green, u8 blue) {
	u64 result = 0;
	result = blue;
	result += green << 8;
	result += red << 16;

	return result;
}

