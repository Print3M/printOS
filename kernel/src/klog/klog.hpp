/*
	Kernel logging system.
*/
#pragma once
#include <libc/stdint.hpp>

enum KLogType { info, debug, error, panic };

class KLog {
  private:
	size _num_of_entries;
	void *_buff;
	char *_free_entry; // Next free entry to write

  public:
	KLog();
	const char *get_entry(size id);
	void add_entry(KLogType type, char *msg);
}