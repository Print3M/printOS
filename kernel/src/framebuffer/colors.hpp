#pragma once
#include <libc/stdint.hpp>

constexpr u64 rgb_to_int(u8 red, u8 green, u8 blue) {
	u64 result = 0;
	result	   = blue;
	result += green << 8;
	result += red << 16;

	return result;
}

constexpr u64 BLACK = rgb_to_int(0, 0, 0);
constexpr u64 WHITE = rgb_to_int(255, 255, 255);
constexpr u64 RED	= rgb_to_int(255, 0, 0);
constexpr u64 GREEN = rgb_to_int(0, 255, 0);
constexpr u64 BLUE	= rgb_to_int(0, 0, 255);