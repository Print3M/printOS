#pragma once
#include <libc/stdint.hpp>

[[noreturn]] void
__assertion_failed(char const *msg, char const *file, size line, char const *func);

#define ASSERT(expr)                                                            \
	do {                                                                        \
		if (!static_cast<bool>(expr)) [[unlikely]]                              \
			__assertion_failed(#expr, __FILE__, __LINE__, __PRETTY_FUNCTION__); \
	} while (0)
