#include <kutils/bitmap.hpp>
#include <kutils/bits.hpp>
#include <kutils/console.hpp>
#include <libc/stdlib.hpp>

Bitmap::Bitmap(void *__buf, size __size_in_bits) : __buf(__buf) {
	// Clear memory for the bitmap
	this->__size = (__size_in_bits + 7) / 8;
	memset(__buf, 0x00000000, this->__size);
};

bool Bitmap::get(size idx) {
	// Get bitmap's entry (bit)
	auto rest = idx % 8;
	auto base = (idx - rest) / 8;
	auto result = *(static_cast<u8 *>(this->__buf) + base);

	return result & (0x1 << rest);
}

void Bitmap::set(size idx) {
	// Set to 1 bitmap's entry (bit)
	auto rest = idx % 8;
	auto base = (idx - rest) / 8;
	auto addr = static_cast<u8 *>(this->__buf) + base;

	*addr = set_bit(*addr, rest);
}

void Bitmap::unset(size idx) {
	// Set to 0 bitmap's entry (bit)
	auto rest = idx % 8;
	auto base = (idx - rest) / 8;
	auto addr = static_cast<u8 *>(this->__buf) + base;

	*addr = clear_bit(*addr, rest);
}

size Bitmap::get_mem_size() {
	// Return memory size of the bitmap in bytes
	return this->__size;
}