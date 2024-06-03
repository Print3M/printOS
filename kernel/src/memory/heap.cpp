#include <kernel.hpp>
#include <kutils/assertions.hpp>
#include <kutils/bits.hpp>
#include <kutils/console.hpp>
#include <libc/stdint.hpp>
#include <memory/heap.hpp>
#include <memory/paging.hpp>

Heap::Heap() {
	// Initialize memory allocator by allocating one page
	auto frame = kernel.pmem->request_frame();
	ASSERT(frame != nullptr);
	kernel.paging->allocate_page(frame, frame);

	auto block	   = static_cast<BlockHeader *>(frame);
	block->is_free = true;
	block->next	   = nullptr;
	block->prev	   = nullptr;
	block->bytes   = PagingConsts::PAGE_SZ - sizeof(BlockHeader);
	this->_blocks = block;
}

BlockHeader *Heap::_find_free_block(size bytes) {
	auto block = this->_blocks;
	ASSERT(block != nullptr);

	do {
		if (block->is_free && block->bytes >= bytes)
			return block;

		block = block->next;
	} while (block != nullptr);

	return nullptr;
}

inline void *get_block_data_addr(BlockHeader *block) {
	return static_cast<void *>((u8 *) block + sizeof(BlockHeader));
}

inline size get_total_block_size(BlockHeader *block) {
	/*
		Total size = block header + data
	*/
	return sizeof(BlockHeader) + block->bytes;
}

inline bool are_blocks_contiguous(BlockHeader *block, BlockHeader *next_block) {
	if (block == nullptr || next_block == nullptr)
		return false;

	auto addr = reinterpret_cast<size>(block) + get_total_block_size(block);

	return reinterpret_cast<BlockHeader *>(addr) == next_block;
}

BlockHeader *Heap::_find_block_close_before_addr(void *addr) {
	auto block				= this->_blocks;
	size current_block_addr = 0;
	size next_block_addr	= 0;
	size find_block_addr	= reinterpret_cast<size>(addr);

	while (block != nullptr) {

		// Last block, there's no other chance
		if (block->next == nullptr) {
			return block;
		}

		current_block_addr = reinterpret_cast<size>(block);
		next_block_addr	   = reinterpret_cast<size>(block->next);

		// If the wanted address is between two blocks
		if (current_block_addr < find_block_addr && next_block_addr < find_block_addr) {
			return block;
		}

		block = block->next;
	}

	ASSERT(1 == 2);
	return nullptr;
}

void *Heap::malloc(size bytes) {
	ASSERT(bytes > 0);
	auto block = this->_find_free_block(bytes);

	// No sufficient free block have been found.
	// Allocate new pages, create new block and add it to linked-list of blocks.
	if (block == nullptr) {
		auto pages		= (bytes / PagingConsts::PAGE_SZ) + 1;
		auto init_block = kernel.pmem->request_frames(pages);

		for (size i = 0; i < pages; i++) {
			auto page = static_cast<void *>((u8 *) init_block + (i * PagingConsts::PAGE_SZ));
			kernel.paging->allocate_page(page, page);
		}

		block			 = reinterpret_cast<BlockHeader *>(init_block);
		block->bytes	 = (pages * PagingConsts::PAGE_SZ) - sizeof(BlockHeader);
		block->is_free	 = true;
		auto prev_block	 = this->_find_block_close_before_addr(static_cast<void *>(block));
		block->next		 = prev_block->next;
		block->prev		 = prev_block;
		prev_block->next = block;

		if (block->next != nullptr) {
			block->next->prev = block;
		}

		block = this->_find_free_block(bytes);
		ASSERT(block != nullptr);
	}

	// Free block has exactly the amount of space that we want
	// or the rest of the space is too small for further fragmentation
	if (block->bytes == bytes || block->bytes - bytes <= sizeof(BlockHeader)) {
		block->is_free = false;

		return get_block_data_addr(block);
	}

	// Free block has way more free space than we want and it's ready
	// to be fragmented.
	//
	// Linked-list structure before:
	// |          block           |     next    |
	// After:
	// |  block  |    new_block   |     next    |
	block->is_free	   = false;
	auto new_block	   = reinterpret_cast<BlockHeader *>((u8 *) get_block_data_addr(block) + bytes);
	new_block->is_free = true;
	new_block->bytes   = block->bytes - bytes - sizeof(BlockHeader);

	ASSERT(new_block->bytes > 0);

	if (block->next != nullptr) {
		block->next->prev = new_block;
	}

	new_block->next = block->next;
	new_block->prev = block;
	block->bytes	= bytes;
	block->next		= new_block;

	return get_block_data_addr(block);
}

void Heap::free(void *ptr) {
	if (ptr == nullptr)
		return;

	auto block	   = reinterpret_cast<BlockHeader *>((u8 *) ptr - sizeof(BlockHeader));
	block->is_free = true;

	// Merge with next block (if blocks are contiguous):
	// The next block is destroyed and its total size is
	// absorbed into the current block.
	if (block->next != nullptr && block->next->is_free &&
		are_blocks_contiguous(block, block->next)) {

		// Skip next block
		block->bytes += get_total_block_size(block->next);
		block->next = block->next->next;

		// Set .prev of next block
		if (block->next != nullptr) {
			block->next->prev = block;
		}
	}

	// Merge with prev block (if blocks are contiguous):
	// The current block is destroyed and its total size is
	// absorbed into the prev block.
	if (block->prev != nullptr && block->prev->is_free &&
		are_blocks_contiguous(block->prev, block)) {

		// Skip current block
		block->prev->bytes += get_total_block_size(block);
		block->prev->next = block->next;

		// Set .prev of next block
		block->next->prev = block->prev;
	}
}

HeapStats Heap::get_heap_stats() {
	auto block		= this->_blocks;
	HeapStats stats = {
		.all_blocks		  = 0,
		.free_blocks	  = 0,
		.allocated_blocks = 0,
	};

	while (block != nullptr) {
		stats.all_blocks++;

		if (block->is_free) {
			stats.free_blocks++;
		} else {
			stats.allocated_blocks++;
		}

		block = block->next;
	}

	return stats;
}