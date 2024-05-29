#pragma once
#include <libc/stdint.hpp>

enum AllocedFlag {
	FREE	  = 0b00000000,
	ALLOCATED = 0b00000001,
};

typedef struct BlockHeader {
	BlockHeader *prev;
	BlockHeader *next;
	bool is_free;
	size bytes;
};

typedef struct HeapStats {
	size all_blocks;
	size free_blocks;
	size allocated_blocks;
};

class Heap {
  private:
	// Double-linked-list of allocated blocks
	BlockHeader *__blocks = nullptr;

  public:
	Heap();
	void *malloc(size bytes);
	void free(void *ptr);

	HeapStats get_heap_stats();

  private:
	BlockHeader *__find_free_block(size bytes);
	void __allocate_block(BlockHeader *block);
	void __free_block();
};