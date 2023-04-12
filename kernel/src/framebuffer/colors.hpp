#pragma once
#include <libc/stdint.hpp>

#define rgb_to_int(r, g, b) (uint64_t)((b & 255) | (g & 255) << 8 | (r & 255) << 16)

#define BLACK rgb_to_int(0, 0, 0)
#define WHITE rgb_to_int(255, 255, 255)
#define RED	  rgb_to_int(255, 0, 0)
#define GREEN rgb_to_int(0, 255, 0)
#define BLUE  rgb_to_int(0, 0, 255)