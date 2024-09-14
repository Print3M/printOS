#include <console/console.hpp>
#include <kernel.hpp>
#include <libc/stdarg.hpp>
#include <libc/stdint.hpp>

size_t str_len(char *str) {
	// Returns string length without '\0' char
	size_t count = 0;

	while (str[count] != '\0') {
		count++;
	}

	return count;
}

char *str_reverse(char *str, int len) {
	char temp;

	for (int i = 0; i < (len / 2); i++) {
		// Switch two chars
		temp			 = str[i];
		str[i]			 = str[len - i - 1];
		str[len - i - 1] = temp;
	}

	return str;
}

void *memcpy(void *dest, void *src, size_t n) {
	volatile uint8_t *d = (uint8_t *) dest;
	volatile uint8_t *s = (uint8_t *) src;

	while (n--) {
		*d++ = *s++;
	}

	return dest;
}

void *memset(void *ptr, u8 v, size n) {
	u8 *p = (u8 *) ptr;

	while (n--) {
		*p++ = v;
	}

	return ptr;
}

char *itoa(u64 num, char *buf, u8 base) {
	// This simple implementation has been copied from:
	//      https://www.geeksforgeeks.org/implement-itoa/
	//
	// How it works:
	// 		123 => "123\0"

	int i = 0;

	/* Handle 0 explicitely, otherwise empty string is printed for 0 */
	if (num == 0) {
		buf[i++] = '0';
		buf[i]	 = '\0';
		return buf;
	}

	// Process individual digits
	while (num != 0) {
		int rem	 = num % base;
		buf[i++] = (rem > 9) ? (rem - 10) + 'a' : rem + '0';
		num		 = num / base;
	}

	buf[i] = '\0'; // Append string terminator

	// Reverse the string
	buf = str_reverse(buf, i);

	return buf;
}

uint8_t strcmp(const char *str1, const char *str2) {
	while (*str1) {
		if (*str1 != *str2) {
			break;
		}

		str1++;
		str2++;
	}

	// return the ASCII difference
	return *(const uint8_t *) str1 - *(const uint8_t *) str2;
}

uint8_t strncmp(const char *str1, const char *str2, size_t n) {
	while (n && *str1 && (*str1 == *str2)) {
		++str1;
		++str2;
		--n;
	}

	if (n == 0) {
		return 0;
	} else {
		return *(uint8_t *) str1 - *(uint8_t *) str2;
	}
}