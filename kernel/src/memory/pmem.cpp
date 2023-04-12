#include <kernel.hpp>
#include <kutils/bitmap.hpp>
#include <kutils/console.hpp>
#include <libc/stdint.hpp>
#include <libc/stdlib.hpp>
#include <memory/mmap.hpp>
#include <memory/pmem.hpp>

Bitmap PMem::__init_bitmap() {
	void *free_mem = nullptr;
	auto free_mem_sz = kernel.mmap->get_largest_free_seg(&free_mem);

	auto page_frames = kernel.mmap->get_page_frames_number();
	if (free_mem_sz < page_frames / 8) {
		// TODO: ERROR: Space for bitmap is to small
	}

	return Bitmap(free_mem, page_frames);
}

PMem::PMem() {
	/*
		Array of frames is based on a memory map recived from the UEFI.
		There might be holes like MMIO.

		Note: It is not just the available memory. If the system actually has 256MB of
		memory, here we will still have the number of frames rounded up to 4GB. Empty spaces will
		simply be locked as they do not exist in the Memory Map provided by UEFI. MMIO spaces will
		also be locked. Therefore, the free frames number will be around 250 MB - the amount of
		usable memory actually installed in the machine.
	*/
	this->__frames = kernel.mmap->get_page_frames_number();
	this->__free_frames = this->__frames;
	this->__locked_frames = 0;

	// Lock all frames at the beggining
	for (size i = 0; i < this->__frames; i++) {
		this->lock_frame(i);
	}

	// Initialize frames by memory map
	for (size i = 0; i < kernel.mmap->entries; i++) {
		auto desc = kernel.mmap->get_desc(i);

		if (kernel.mmap->is_usable_memory(desc)) {

			// Free frames that can be used
			for (size n = 0; n < desc->pages; n++) {
				auto idx = this->ptr_to_frame_idx(static_cast<u8 *>(desc->paddr) +
												  (n * PMemConsts::FRAME_SZ));
				this->free_frame(idx);
			}
		}
	}

	// !!! TODO: Lock frames with kernel itself !!!

	// Lock frame with stack itself
	auto stack_frame = this->ptr_to_frame_idx(kernel.stack);
	this->lock_frame(stack_frame);

	// Lock frames where the bitmap is placed
	auto start_frame_idx = this->ptr_to_frame_idx(this->__bitmap.buf);
	for (size i = 0; i < this->__bitmap.get_mem_size() / PMemConsts::FRAME_SZ; i++) {
		this->lock_frame(start_frame_idx + i);
	}
}

void PMem::free_frame(size idx) {
	// If is locked -> free
	if (this->__bitmap.get(idx) == true) {
		this->__bitmap.unset(idx);
		this->__free_frames++;
		this->__locked_frames--;
	}
}

void PMem::lock_frame(size idx) {
	// If is free -> lock
	if (this->__bitmap.get(idx) == false) {
		this->__bitmap.set(idx);
		this->__free_frames--;
		this->__locked_frames++;
	}
}

size PMem::ptr_to_frame_idx(const void *ptr) {
	auto intptr = reinterpret_cast<size>(ptr);

	return intptr / PMemConsts::FRAME_SZ;
}

void *PMem::idx_to_frame_ptr(size idx) {
	return reinterpret_cast<void *>(idx * PMemConsts::FRAME_SZ);
}

void *PMem::request_frame() {
	if (this->__free_frames == 0) {
		return nullptr;
	}

	for (size i = this->__last_free_idx; i < this->__frames; i++) {
		if (this->__bitmap.get(i) == false) {
			this->lock_frame(i);
			this->__last_free_idx = i;

			return this->idx_to_frame_ptr(i);
		}
	}

	return nullptr;
}