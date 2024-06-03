#pragma once
#include <libc/stdint.hpp>

enum AllocedFlag {
	FREE	  = 0b00000000,
	ALLOCATED = 0b00000001,
};

struct BlockHeader {
	BlockHeader *prev;
	BlockHeader *next;
	bool is_free;
	size bytes;
};

struct HeapStats {
	size all_blocks;
	size free_blocks;
	size allocated_blocks;
};

class Heap {
  private:
	// Double-linked-list of allocated blocks
	BlockHeader *_blocks = nullptr;

  public:
	Heap();
	void *malloc(size bytes);
	void free(void *ptr);
	HeapStats get_heap_stats();

  private:
	void _free_block();
	BlockHeader *_find_free_block(size bytes);
	BlockHeader *_find_block_close_before_addr(void *addr);
};