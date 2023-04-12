#pragma once
#include <libc/stdint.hpp>

namespace mmio {

static inline void write8(void *p, u8 data) {
	*(volatile u8 *) (p) = data;
}

static inline u8 read8(void *p) {
	return *(volatile u8 *) (p);
}

static inline void write16(void *p, u16 data) {
	*(volatile u16 *) (p) = data;
}

static inline u16 read16(void *p) {
	return *(volatile u16 *) (p);
}

static inline void write32(void *p, u32 data) {
	*(volatile u32 *) (p) = data;
}

static inline u32 read32(void *p) {
	return *(volatile u32 *) (p);
}

static inline void write64(void *p, u64 data) {
	*(volatile u64 *) (p) = data;
}

static inline u64 read64(void *p) {
	return *(volatile u64 *) (p);
}

static inline void readn(void *dst, const volatile void *src, size bytes) {
	volatile u8 *s = (volatile u8 *) src;
	u8 *d = (u8 *) dst;

	while (bytes > 0) {
		*d = *s;
		++s;
		++d;
		--bytes;
	}
}

}; // namespace mmio