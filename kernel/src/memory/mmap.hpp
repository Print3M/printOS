#pragma once
#include <libc/stdint.hpp>

struct __attribute__((packed)) EfiMMapDesc {
	// EFI Memory Map Descriptor
	u32 type;
	u32 pad;
	void *paddr; // Physical address
	void *vaddr; // Virtual address
	u64 pages;	 // Number of pages
	u64 attr;
};

struct MemoryData {
	EfiMMapDesc *mmap;
	size mmap_sz;
	size desc_sz;
	size entries;
};

enum class EfiMemoryType {
	EFI_RESERVED_MEMORY_TYPE,
	EFI_LOADER_CODE,
	EFI_LOADER_DATA,
	EFI_BOOT_SERVICES_CODE,
	EFI_BOOT_SERVICES_DATA,
	EFI_RUNTIME_SERVICES_CODE,
	EFI_RUNTIME_SERVICES_DATA,
	EFI_CONVENTIONAL_MEMORY,
	EFI_UNUSABLE_MEMORY,
	EFI_ACPI_RECLAIM_MEMORY,
	EFI_ACPI_MEMORY_NVS,
	EFI_MEMORY_MAPPED_IO,
	EFI_MEMORY_MAPPED_IO_PORT_SPACE,
	EFI_PAL_CODE,
	EFI_MAX_MEMORY_TYPE
};

class MemoryMap {
  private:
	size __mmap_sz; // Memory map size
	size __desc_sz; // Memory descriptor size
	size __entries; // Number of mmap entries

  public:
	const size &entries = __entries;
	EfiMMapDesc *mmap; // Memory Map TODO: Is it still valid after exiting UEFI services?

  private:
	size __get_entire_memory_size();

  public:
	MemoryMap(MemoryData &data);
	EfiMMapDesc *get_desc(size idx);
	size get_page_frames_number();
	size get_largest_free_seg(void **mem_seg);
	bool is_usable_memory(EfiMMapDesc *desc);
	void dbg_print_mmap();
};