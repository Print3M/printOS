#include <kernel.hpp>
#include <libc/stdarg.hpp>
#include <libc/stdint.hpp>
#include <libc/stdlib.hpp>

void __printf(const char *str, va_list args) {
	// Dynamic list of arguments
	char buf[4096]; // Buffer for result string
	memset(buf, 0, 4096);
	char temp_buf[64]; // 8*8 = 64 bits + 9th byte for '\0';
	memset(temp_buf, 0, 64);

	size len = str_len((char *) str);
	size temp_len = 0;
	bool is_percent = false; // %x notation detector

	u16 j = 0;
	for (u16 i = 0; i < len; i++) {
		if (is_percent) {
			switch (str[i]) {
				case 's': {
					// If %s
					// Get next argument
					char *arg = va_arg(args, char *);
					temp_len = str_len(arg);

					// Add string to result buffer
					memcpy((void *) (buf + j), (void *) arg, temp_len);

					j += temp_len;
					break;
				}
				// case 'c': {
				// 	// If %c
				// 	uint8_t arg = va_arg(args, uint8_t);
				// 	buf[j] = arg;
				// 	j += 1;
				// 	break;
				// }
				case 'd': {
					// If %d
					u64 arg = va_arg(args, u64);
					itoa(arg, temp_buf, 10);
					temp_len = str_len(temp_buf);
					memcpy((void *) (buf + j), (void *) temp_buf, temp_len);
					j += temp_len;
					break;
				}
				case 'p': {
					// If %p
					u64 arg = va_arg(args, u64);
					itoa(arg, temp_buf, 16);
					temp_len = str_len(temp_buf);
					memcpy((void *) (buf + j), (void *) temp_buf, temp_len);
					j += temp_len;
					break;
				}
			}

			is_percent = false;
			continue;
		} else {
			if (str[i] == '%') {
				is_percent = true;
				continue;
			} else {
				// Add new char
				buf[j] = str[i];
				j++;
				continue;
			}
		}
	}

	buf[j] = '\0';

	kernel.console->print(buf);
}

void kprintf(const char *fmt, ...) {
	va_list args;
	va_start(args, fmt);
	__printf(fmt, args);
	va_end(args);
}

void kprint_err(const char *msg) {
	kprintf("[!] Kernel error: %s \n", msg);
}

void kprint_succ(const char *msg) {
	kprintf("[+] Kernel success: %s \n", msg);
}

void kprint_info(const char *msg) {
	kprintf("[*] Kernel info: %s \n", msg);
}

void kprint_pmem_info(void) {
	kprint_info("physical memory info (provided by the UEFI Memory Map):");
	kprintf("\t all: frames = %d, memory = %dMB (holes and MMIO included) \n",
			kernel.pmem->frames,
			kernel.pmem->frames * PMemConsts::FRAME_SZ / 1024 / 1024);
	kprintf("\t free: frames = %d, memory = %dMB \n",
			kernel.pmem->free_frames,
			kernel.pmem->free_frames * PMemConsts::FRAME_SZ / 1024 / 1024);
	kprintf("\t locked: frames = %d, memory = %dMB \n",
			kernel.pmem->locked_frames,
			kernel.pmem->locked_frames * PMemConsts::FRAME_SZ / 1024 / 1024);
}

void kprint_cpu_info(void) {
	kprint_info("CPU info:");
	kprintf("\t - manufacturer:   %s \n", devices.cpu.manufacturer);
	kprintf("\t - stepping id:    %d \n", devices.cpu.stepping_id);
	kprintf("\t - family id:      %d \n", devices.cpu.family_id);
	kprintf("\t - model id:       %d \n", devices.cpu.model_id);
	kprintf("\t - type:           %d \n", devices.cpu.type);
	kprintf("\t - brand:          %s \n", devices.cpu.brand);
	kprintf("\t - logical cores:  %d \n", devices.cpu.logical_cores);
	kprintf("\t - physical cores: %d \n", devices.cpu.physical_cores);
	kprintf("\t - hyperthreading: %d \n", devices.cpu.hyperthreading);
}

void kpanic(const char *msg) {
	kernel.framebuffer->set_all_pixels(BLUE);
	kernel.console->cursor.gotoxy(0, 0);
	kernel.console->cursor.set_bg(BLUE);
	kernel.console->cursor.set_fg(WHITE);

	kprintf("Kernel panic!!! \n");
	kprintf("Error: %s", msg);
}