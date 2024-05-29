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
	this->__blocks = block;
}

void Heap::__allocate_block(BlockHeader *block) {
	if (block->is_free) {
		block->is_free = false;
	}
}

BlockHeader *Heap::__find_free_block(size bytes) {
	auto block = this->__blocks;
	ASSERT(block != nullptr);

	do {
		size block_addr = reinterpret_cast<u64>(block);

		if (block->is_free && block->bytes >= bytes)
			return block;

		block = block->next;
	} while (block != nullptr);

	return nullptr;
}

inline void *get_block_data_addr(BlockHeader *block) {
	return static_cast<void *>(block) + sizeof(BlockHeader);
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

void *Heap::malloc(size bytes) {
	// TODO:
	// 		[] Allocate one physical page
	//      [] Allocate couple physical pages

	auto block = this->__find_free_block(bytes);
	ASSERT(block != nullptr);

	// No sufficient free block have been found
	if (block == nullptr) {
		auto pages = (bytes / PagingConsts::PAGE_SZ) + 1;

		// Allocate more memory
		kprintf("Allocate more memory!");

		block;

		ASSERT(block != nullptr);
	}

	// Free block has exactly the amount of space that we want
	// or the rest of the space is too small for further fragmentation
	if (block->bytes == bytes || block->bytes - bytes <= sizeof(BlockHeader)) {
		this->__allocate_block(block);

		return get_block_data_addr(block);
	}

	// Free block has way more free space than we want and it's ready
	// to be fragmented
	this->__allocate_block(block);
	auto new_block	   = reinterpret_cast<BlockHeader *>(get_block_data_addr(block) + bytes);
	new_block->is_free = true;
	new_block->prev	   = block;
	new_block->next	   = nullptr;
	new_block->bytes   = block->bytes - bytes;
	block->bytes	   = bytes;
	block->next		   = new_block;

	// TODO: Merge new block with next if free

	return get_block_data_addr(block);

	/*
	auto frame = kernel.pmem->request_frame();
	// ASSERT(frame == nullptr);
	kernel.paging->allocate_page(frame, frame);

	size total_size	   = sizeof(AllocedHeader) + bytes;
	auto header		   = static_cast<AllocedHeader *>(frame);
	auto alloced_block = frame + sizeof(AllocedHeader);
	kprintf("%d\n", total_size);
	kprintf("%p\n", alloced_block);
	kprintf("%p\n", header);

	return alloced_block;
	*/
}

void Heap::free(void *ptr) {
	if (ptr == nullptr)
		return;

	auto block	   = static_cast<BlockHeader *>(ptr - sizeof(BlockHeader));
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
	auto block		= this->__blocks;
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