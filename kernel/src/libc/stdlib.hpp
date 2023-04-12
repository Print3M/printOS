#pragma once
#include <libc/stdint.hpp>

// String operations
size_t str_len(char *str);
char *str_reverse(char *str, int len);
uint8_t strcmp(const char *str1, const char *str2);
uint8_t strncmp(const char *str1, const char *str2, const size_t n);

// Conversions
char *itoa(uint64_t num, char *buf, uint8_t base);

// Memory operations
void *memset(void *ptr, uint8_t v, size_t n);
void *memcpy(void *dest, void *src, size_t n);