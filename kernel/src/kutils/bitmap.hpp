#pragma once
#include <libc/stdint.hpp>

class Bitmap {
  private:
	void *__buf;
	size __size; // In bytes

  public:
	const void *buf = &__buf; // TODO: Does it work?

  public:
	Bitmap(void *__buf, size __size);
	bool get(size idx);
	void set(size idx);
	void unset(size idx);
	size get_mem_size();
};