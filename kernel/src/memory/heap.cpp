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

BlockHeader *Heap::__find_block(size bytes) {
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

void *Heap::malloc(size bytes) {

	// DEBUGGING: There's bug somewhere
	auto tmp = this->__blocks;
	for (; tmp->next != nullptr; tmp = tmp->next) {
		kprintf("%p|", tmp);
	}

	kprintf("\n");

	auto block = this->__find_block(bytes);
	ASSERT(block != nullptr);

	// No sufficient free block have been found
	if (block == nullptr) {
		// Allocate more memory
		kprintf("Allocate more memory!");
		return nullptr;
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
	auto new_block = reinterpret_cast<BlockHeader *>(get_block_data_addr(block) + bytes);
	kprintf("block: %p, new_block: %p\n", block, new_block);
	new_block->is_free = true;
	new_block->prev	   = block;
	new_block->next	   = nullptr;
	block->next		   = new_block;

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