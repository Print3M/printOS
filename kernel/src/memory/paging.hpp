/* More about paging on x86 in IM3:
	4.1.1 - specific options for each mode of paging
	4.2   - overview of paging, description of translation
	4.5   - specific 4-level paging documentation
*/
#pragma once
#include <framebuffer/framebuffer.hpp>
#include <libc/stdint.hpp>
#include <memory/pmem.hpp>

enum PagingConsts { PAGE_SZ = PMemConsts::FRAME_SZ };

#define IA32_EFER 0xC0000080

#define EFER_LME		 8
#define CR3_PWT			 3
#define CR3_PCD			 4
#define BITS_9_MASK		 0x1ff
#define PAGE_OFFSET_BITS 12

/*
	Every paging structure is 4096 Bytes in size and contains a
	number of individual entries. With the 4-level paging modes,
	each  entry is 64 bits (8 bytes); there are thus 512 entries
	in each structure. 4-level paging translates 48-bit linear
	addresses to 52-bit physical addresses. Although 52 bits
	corresponds to 4 PBytes, linear addresses are limited to 48 bits;
	at most 256 TBytes of linear-address space may be accessed at
	any given time. 4-level paging translates linear addresses using
	a hierarchy of in-memory paging structures located using the
	contents of CR3, which is used to locate the first paging-structure.
*/

struct __attribute__((packed)) PT_Entry {
	// Description: IM3, Table 4-20
	size present : 1;
	size rw : 1;
	size us : 1;
	size pwt : 1;
	size pcd : 1;
	size a : 1;
	size d : 1;
	size pat : 1;
	size g : 1;
	size ignored1 : 3;
	size page_addr : 32;
	size reserved1 : 8;
	size ignored3 : 7;
	size protection_key : 4;
	size xd : 1;
}; // PageTable

struct __attribute__((packed)) PD_Entry {
	// Description: IM3, Table 4-19
	size present : 1;
	size rw : 1;
	size us : 1;
	size pwt : 1;
	size pcd : 1;
	size a : 1;
	size ignored1 : 1;
	size ps : 1;
	size ignored2 : 4;
	size pt_addr : 32;
	size reserved1 : 8;
	size ignored3 : 11;
	size xd : 1;
}; // PageDirectory

struct __attribute__((packed)) PDPT_Entry {
	// Description: IM3, Table 4-17
	size present : 1;
	size rw : 1;
	size us : 1;
	size pwt : 1;
	size pcd : 1;
	size a : 1;
	size ignored1 : 1;
	size ps : 1;
	size ignored2 : 4;
	size pd_addr : 32;
	size reserved1 : 8;
	size ignored3 : 11;
	size xd : 1;
}; // PageDirectoryPointerTable

struct __attribute__((packed)) PML4_Entry {
	// Description: IM3, Table 4-15
	size present : 1;
	size rw : 1;
	size us : 1;
	size pwt : 1;
	size pcd : 1;
	size a : 1;
	size ignored1 : 1;
	size ps : 1;
	size ignored2 : 4;
	size pdpt_addr : 32;
	size reserved1 : 8;
	size ignored3 : 11;
	size xd : 1;
}; // PageMapLevel4

class Paging {
  private:
	PML4_Entry __attribute__((aligned(0x1000))) PML4[512];

  public:
	Paging();
	bool is_page_present(void *vaddr);

  private:
	void map_vaddr_to_paddr(void *vaddr, void *paddr);
	void make_paging();
	void __identity_paging(void *addr, size_t n);
};

class PageIndexer {
  private:
	u16 __pml4; // PageMapLevel4
	u16 __pdpt; // PageDirectoryPointerTable
	u16 __pd;	// PageDirectory
	u16 __pt;	// PageTable

  public:
	const u16 &pml4 = __pml4;
	const u16 &pdpt = __pdpt;
	const u16 &pd = __pd;
	const u16 &pt = __pt;

  public:
	PageIndexer(u64 vaddr);
};