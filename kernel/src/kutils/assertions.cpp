#include <kutils/console.hpp>
#include <libc/stdint.hpp>

[[noreturn]] void
__assertion_failed(char const *msg, char const *file, size line, char const *func) {

	// Call PANIC here
	kpanic(msg, file, line, func);

	for (;;)
		;
}