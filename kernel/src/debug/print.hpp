#pragma once
#include <libc/stdint.hpp>

void kprint_mem_hex(uint8_t *ptr, uint16_t n_bytes);
void kprint_mem_bin(uint8_t *ptr, uint16_t n_bytes);
void kprint_mem_ascii(uint8_t *ptr, uint16_t n_bytes);
