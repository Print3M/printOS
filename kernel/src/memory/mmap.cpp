#include <kernel.hpp>
#include <kutils/console.hpp>
#include <libc/stdint.hpp>
#include <memory/mmap.hpp>

MemoryMap::MemoryMap(MemoryData &data) {
	this->__desc_sz = data.desc_sz;
	this->__entries = data.entries;
	this->__mmap_sz = data.mmap_sz;
	this->mmap = data.mmap;
}

EfiMMapDesc *MemoryMap::get_desc(size idx) {
	// Get Memory Map Descriptor of index
	return (EfiMMapDesc *) ((u8 *) this->mmap + (idx * this->__desc_sz));
}

size MemoryMap::get_page_frames_number() {
	// Get total number of pages in memory (based on memory map)
	// but holes are included as well.
	// It's described in UEFI Specification "EFI_BOOT_SERVICES.GetMemoryMap()"
	// that memory map might not be contigoues. Bitmap in Physical Memory
	// Allocator should keep number of all pages (even if there is a hole).
	return this->__get_entire_memory_size() / PMemConsts::FRAME_SZ;
}

size MemoryMap::get_largest_free_seg(void **mem_seg) {
	// Return      - size of largest free memory segment
	// OUT mem_seg - largest free memory segment
	size max_sz = 0;

	for (u64 i = 0; i < kernel.memory->entries; i++) {
		auto desc = kernel.mmap->get_desc(i);

		if (desc->type == EFI_CONVENTIONAL_MEMORY) {
			size seg_sz = desc->pages * PMemConsts::FRAME_SZ;

			if (seg_sz > max_sz) {
				max_sz = seg_sz;
				*mem_seg = desc->paddr;
			}
		}
	}

	return max_sz;
}

size MemoryMap::__get_entire_memory_size() {
	// Return size of the memory (with holes and unavailable spaces) in bytes.
	auto last_desc = this->get_desc(this->__entries - 1);

	// size of memory = (paddr of last page + last page size)
	return reinterpret_cast<size>(last_desc->paddr) + (PMemConsts::FRAME_SZ * last_desc->pages) +
		   PMemConsts::FRAME_SZ;
}

void MemoryMap::dbg_print_mmap() {
	kprintf("Memory map (all entries: %d)", this->entries);

	for (size i = 0; i < this->entries; i++) {
		auto desc = this->get_desc(i);
		kprintf("%p-%p: type=%d, frames=%d \n",
				desc->paddr,
				reinterpret_cast<u64>(desc->paddr) + (desc->pages * PMemConsts::FRAME_SZ),
				desc->type,
				desc->pages);
	}
}

bool MemoryMap::is_usable_memory(EfiMMapDesc *desc) {
	// Check if EFI memory segment can be used by kernel. Only these memory
	// segments can be reallocated!
	return (desc->type == EfiMemoryType::EFI_CONVENTIONAL_MEMORY ||
			desc->type == EfiMemoryType::EFI_BOOT_SERVICES_CODE ||
			desc->type == EfiMemoryType::EFI_BOOT_SERVICES_DATA);
}