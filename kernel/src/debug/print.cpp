#include <cpu/mmio.h>
#include <kernel.hpp>
#include <kutils/console.hpp>
#include <libc/stdint.hpp>
#include <libc/stdlib.hpp>

void kprint_mem_hex(u8 *ptr, u16 n_bytes) {
	// Print bytes of memory as hex numbers
	// printf("(first: 0x%p, last: 0x%p, %d bytes):\n", ptr + n_bytes - 1, ptr, n_bytes);
	// TODO: get 4 bytes at once by mmio_read32

	ptr += n_bytes - 1;
	char str[4];
	u8 line_c = 0;

	while (n_bytes > 0) {
		if (line_c == 8 || line_c == 0) {
			line_c = 0;
			kprintf("\n%p-%p:  ", ptr, ptr - 7);
		}

		memset((void *) str, 0x0, 4);
		itoa(mmio::read8((void *) ptr), str, 16);

		if (str_len(str) == 1) {
			// Zero alignment
			kernel.console->print('0');
		}

		kernel.console->print(str);
		kernel.console->print(' ');
		ptr--;
		n_bytes--;
		line_c++;
	}

	kernel.console->print('\n');
}

void kprint_mem_bin(u8 *ptr, u16 n_bytes) {
	// Print bytes of memory as binary numbers
	kprintf("(first: 0x%p, last: 0x%p, %d bytes):\n", ptr + n_bytes - 1, ptr, n_bytes);
	ptr += n_bytes - 1;
	char str[16];

	while (n_bytes > 0) {
		memset((void *) str, 0x0, 16);
		itoa(mmio::read8((void *) ptr), str, 2);

		// Zero alignment
		u8 len = str_len(str);
		while (len++ < 8) {
			kernel.console->print('0');
		}

		kernel.console->print(str);
		kernel.console->print(' ');
		ptr--;
		n_bytes--;
	}

	kernel.console->print('\n');
}

void kprint_mem_ascii(u8 *ptr, u16 n_bytes) {
	// Print bytes of memory as ASCII characters
	kprintf("(first: 0x%p, last: 0x%p, %d bytes):\n", ptr + n_bytes - 1, ptr, n_bytes);
	ptr += n_bytes - 1;

	while (n_bytes > 0) {
		kernel.console->print(mmio::read8((void *) ptr));
		ptr--;
		n_bytes--;
	}

	kernel.console->print('\n');
}