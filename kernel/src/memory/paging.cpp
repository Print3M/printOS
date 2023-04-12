#include <acpi/apic/lapic.hpp>
#include <acpi/hpet/hpet.hpp>
#include <cpu/cpu.hpp>
#include <kernel.hpp>
#include <kutils/bits.hpp>
#include <kutils/console.hpp>
#include <libc/stdint.hpp>
#include <libc/stdlib.hpp>
#include <memory/paging.hpp>
#include <memory/pmem.hpp>

extern "C" void enable_cpu_4_level_paging();

void set_cr3_register(size paddr, bool pwt, bool pcd) {
	/*
		IN :paddr - physical address of first paging-translation struct
		IN :pwt   - page-level write-through
		IN :pcd   - page-level cache disable

		More about what is going on here:
			IM3, chapter: 4.5 - 4-level paging
	*/
	u64 cr3 = paddr;

	if (pwt)
		cr3 = set_bit(cr3, CR3_PWT);
	if (pcd)
		cr3 = set_bit(cr3, CR3_PCD);

	__asm__ volatile("mov %0, %%cr3" ::"r"(cr3));
}

PageIndexer::PageIndexer(u64 vaddr) {
	/*
		Virtual address schema (48-bits in total):
		| 111111111 | 111111111 | 111111111 | 111111111 | 111111111111
			PML4         PDPT         PD          PT         OFFSET
		   9 bits		9-bits		9 bits      9 bits		12 bits
	*/

	vaddr >>= PAGE_OFFSET_BITS;			// Remove first 12 bytes (page offset)
										// -
	this->__pt = vaddr & BITS_9_MASK;	// PageTable entry
	vaddr >>= 9;						// -
	this->__pd = vaddr & BITS_9_MASK;	// PageDirectory entry
	vaddr >>= 9;						// -
	this->__pdpt = vaddr & BITS_9_MASK; // PageDirectoryPointerTable entry
	vaddr >>= 9;						//
	this->__pml4 = vaddr & BITS_9_MASK; // PageMapLevel4 entry
}

Paging::Paging() {
	/*
		IM3: 4.1 - Paging modes and control bits
		Software enables paging by using the MOV to CR0 instruction to set CR0.PG.
		Before doing so, software should ensure that control register CR3 contains
		the physical address of the first paging structure that the processor will
		use for linear-address translation (see Section 4.2) and that structure is
		initialized as desired.
	*/

	// Clear space for a page structure
	memset((void *) this->PML4, 0x0, sizeof(PML4_Entry) * 512);

	// IM3: 4.1.1 - specific options to set in order to get 4-level paging enabled
	enable_cpu_4_level_paging();

	// Set IA32_EFER.LME bit
	u64 msr = cpu::read_msr(IA32_EFER);
	cpu::write_msr(IA32_EFER, set_bit(msr, EFER_LME));
	this->make_paging();

	// Load PML4 structure -> enable paging
	set_cr3_register((u64) PML4, false, false);

	// Test paging abilities
	size test_addr = 0x99999999999; // Way more than physical memory
	u8 *test_addr_ptr = (u8 *) test_addr;
	this->map_vaddr_to_paddr((void *) test_addr, (void *) 0x0);
	*test_addr_ptr = 123;

	if (*test_addr_ptr != 123) {
		kpanic("Paging test failed!");
	}
}

bool Paging::is_page_present(void *vaddr) {
	// Check if vaddr has its page present in the page table
	auto index = PageIndexer((u64) vaddr);

	PML4_Entry *pml4_entry = &PML4[index.pml4];
	if (pml4_entry->present == false) {
		return false;
	}

	PDPT_Entry *PDPT = reinterpret_cast<PDPT_Entry *>(pml4_entry->pdpt_addr << PAGE_OFFSET_BITS);
	PDPT_Entry *pdpt_entry = &PDPT[index.pdpt];
	if (pdpt_entry->present == false) {
		return false;
	}

	PD_Entry *PD = reinterpret_cast<PD_Entry *>(pdpt_entry->pd_addr << PAGE_OFFSET_BITS);
	PD_Entry *pd_entry = &PD[index.pd];
	if (pd_entry->present == false) {
		return false;
	}

	PT_Entry *PT = reinterpret_cast<PT_Entry *>(pd_entry->pt_addr << PAGE_OFFSET_BITS);
	PT_Entry *pt_entry = &PT[index.pt];
	return pt_entry->present;
}

void Paging::map_vaddr_to_paddr(void *vaddr, void *paddr) {
	// Go down the paging-structure (by :vaddr) to the page and set its :paddr
	// TODO: What if pmem_request_frame returns NULL
	// TODO: What if pmem_request_frame returns addr higher than 44 bits (32 + PAGE_OFFSET)?

	auto index = PageIndexer((u64) vaddr);

	/*
		PML4
	*/
	PML4_Entry *pml4_entry = &PML4[index.pml4];
	PDPT_Entry *PDPT;
	if (pml4_entry->present == false) {
		// If not already present, create PDPT
		PDPT = (PDPT_Entry *) kernel.pmem->request_frame();
		memset(PDPT, 0, PagingConsts::PAGE_SZ);

		// Set PML4 entry
		pml4_entry->pdpt_addr = (u64) PDPT >> PAGE_OFFSET_BITS;
		pml4_entry->present = true;
		pml4_entry->rw = true;
	} else {
		PDPT = reinterpret_cast<PDPT_Entry *>(pml4_entry->pdpt_addr << PAGE_OFFSET_BITS);
	}

	/*
		PDPT
	*/
	PDPT_Entry *pdpt_entry = &PDPT[index.pdpt];
	PD_Entry *PD;
	if (pdpt_entry->present == false) {
		// If PDPT entry not present, create PD and set PDPT entry
		PD = (PD_Entry *) kernel.pmem->request_frame();
		memset(PD, 0, PagingConsts::PAGE_SZ);

		// Set PDPT entry
		pdpt_entry->pd_addr = (u64) PD >> PAGE_OFFSET_BITS;
		pdpt_entry->present = true;
		pdpt_entry->rw = true;
	} else {
		PD = reinterpret_cast<PD_Entry *>((pdpt_entry->pd_addr << PAGE_OFFSET_BITS));
	}

	/*
		PD
	*/
	PD_Entry *pd_entry = &PD[index.pd];
	PT_Entry *PT;
	if (pd_entry->present == false) {
		// If PD entry not present, create PT and set PD entry
		PT = (PT_Entry *) kernel.pmem->request_frame();
		memset(PT, 0, PagingConsts::PAGE_SZ);

		// Set PD entry
		pd_entry->pt_addr = (u64) PT >> PAGE_OFFSET_BITS;
		pd_entry->present = true;
		pd_entry->rw = true;
	} else {
		PT = reinterpret_cast<PT_Entry *>(pd_entry->pt_addr << PAGE_OFFSET_BITS);
	}

	/*
		PT
	*/
	PT_Entry *pt_entry = &PT[index.pt];
	pt_entry->page_addr = (u64) paddr >> PAGE_OFFSET_BITS;
	pt_entry->present = true;
	pt_entry->rw = true;
}

void Paging::make_paging() {
	// Identity paging for the entire memory physically installed in the system.
	// Just iterate over memory map and map every segment. Holes are not present
	// in the memory map - only physically present memory.
	for (size i = 0; i < kernel.mmap->entries; i++) {
		auto desc = kernel.mmap->get_desc(i);
		this->__identity_paging(desc->paddr, PMemConsts::FRAME_SZ * desc->pages);
	}

	// Identy paging for framebuffer
	this->__identity_paging(kernel.framebuffer->get_address(), kernel.framebuffer->get_size());

	// Identy paging for Local APIC
	this->__identity_paging(reinterpret_cast<void *>(kernel.acpi_tables.madt->lapic_addr), 4096);

	// Identy paging for I/O APIC
	this->__identity_paging(reinterpret_cast<void *>(kernel.acpi_tables.ioapic->io_apic_addr), 8);

	// Identy paging for HPET
	this->__identity_paging(reinterpret_cast<void *>(kernel.acpi_tables.hpet->base_address.address),
							hpet::get_mmio_sz());
}

void Paging::__identity_paging(void *addr, size n) {
	// Perform identity paging (1:1) page frame -> virtual page
	// NOTICE: :n will be always rounded up to full page size
	// :addr - start address of identity paging area
	// :n    - number of bytes

	for (size i = 0; i < (n / (PagingConsts::PAGE_SZ + 1)) + 1; i++) {
		this->map_vaddr_to_paddr(addr, addr);
		addr = reinterpret_cast<void *>((size) addr + PagingConsts::PAGE_SZ);
	}
}
