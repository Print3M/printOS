#include <acpi/apic/ioapic.hpp>
#include <acpi/apic/lapic.hpp>
#include <acpi/apic/madt.hpp>
#include <acpi/hpet/comparator.hpp>
#include <acpi/hpet/hpet.hpp>
#include <console/console.hpp>
#include <cpu/cpu.hpp>
#include <framebuffer/framebuffer.hpp>
#include <gdt/gdt.hpp>
#include <idt/idt.hpp>
#include <kernel.hpp>
#include <kutils/assertions.hpp>
#include <kutils/bitmap.hpp>
#include <kutils/console.hpp>
#include <memory/heap.hpp>
#include <memory/mmap.hpp>
#include <memory/paging.hpp>
#include <memory/pmem.hpp>
#include <pic/pic.hpp>

// Global kernel structures
KernelData kernel;
DevicesInfo devices;

extern "C" void _start(BootloaderData *bd) {
	ASSERT(bd != nullptr);

	kernel.stack	 = static_cast<void *>(bd);
	kernel.acpi_rsdp = (acpi::RSDP *) bd->acpi_rsdp;

	/*
		Memory map
	*/
	auto mmap	= MemoryMap(*bd->memory);
	kernel.mmap = &mmap;

	/*
		Framebuffer
	*/
	auto fb			   = Framebuffer(*bd->framebuffer);
	kernel.framebuffer = &fb;
	kernel.memory	   = bd->memory;

	/*
		Kernel console
	*/
	auto con	   = Console(*bd->font, *kernel.framebuffer);
	kernel.console = &con;
	kernel.console->clear();

	/*
		Physical memory
	*/
	auto pmem	= PMem();
	kernel.pmem = &pmem;
	kprint_succ("Physical memory initalization success");
	kprint_pmem_info();

	/*
		CPU
	*/
	cpu::get_info(devices.cpu);
	auto cpu_s = cpu::check_state();
	if (cpu_s != cpu::SUCCESS) {
		kprintf("[-] CPU state error: %d", cpu_s);
	} else {
		kprint_succ("CPU state correct");
	}
	// kprint_cpu_info();

	/*
		Global Description Table
	*/
	kernel.gdt = gdt::init();
	kprint_succ("GDT initalization success");

	/*
		IDT
	*/
	auto idt   = idt::IDT();
	kernel.idt = &idt;
	kprint_succ("IDT initialization success");

	/*
		ACPI
	*/
	switch (acpi::get_version()) {
		case acpi::VER_1:
			kprint_succ("ACPI 1.0 detected");
			break;
		case acpi::VER_2:
			kprint_succ("ACPI 2.0 detected");
			break;
		default:
			kpanic("Unknown version of ACPI detected");
			break;
	}

	if (acpi::verify_rsdp() == false) {
		kpanic("RSDP not verified");
	} else {
		kprint_succ("RSDP verified");
	}

	if (acpi::sdt::verify_rsdt() == false) {
		kpanic("RSDT not verified");
	} else {
		kprint_succ("RSDT verified");
	}

	/*
		MADT
	*/
	kernel.acpi_tables.madt = (madt::SDT_Table *) acpi::sdt::get_header(acpi::sdt::MADT_SIG);
	ASSERT(kernel.acpi_tables.madt != nullptr);

	kernel.acpi_tables.lapic =
		(madt::ics::CPU_LAPIC_Struct *) madt::ics::get_header(madt::ics::CPU_LAPIC);
	ASSERT(kernel.acpi_tables.lapic != nullptr);

	kernel.acpi_tables.ioapic =
		(madt::ics::IO_APIC_Struct *) madt::ics::get_header(madt::ics::IO_APIC);
	ASSERT(kernel.acpi_tables.ioapic != nullptr);

	/*
		HPET
	*/
	kernel.acpi_tables.hpet = (hpet::SDT_Table *) acpi::sdt::get_header(acpi::sdt::HPET_SIG);
	ASSERT(kernel.acpi_tables.hpet != nullptr);
	kprint_succ("HPET initialization success");

	/*
		Paging
	*/
	auto paging	  = Paging();
	kernel.paging = &paging;
	kprint_succ("4-level paging initialization success");

	madt::dbg_print_lapic_info();

	/*
		External interrupts
	*/
	pic::disable();
	idt::enable_external_interrupts();
	ioapic::init();
	lapic::init();

	auto heap	= Heap();
	kernel.heap = &heap;

	// TODO: PS/2 keyboard driver

	auto ptr1 = kernel.heap->malloc(10);
	auto ptr2 = kernel.heap->malloc(10);
	auto ptr3 = kernel.heap->malloc(10);
	auto ptr4 = kernel.heap->malloc(10);
	auto ptr5 = kernel.heap->malloc(10);
	auto ptr6 = kernel.heap->malloc(10);
	auto ptr7 = kernel.heap->malloc(10);
	auto ptr8 = kernel.heap->malloc(10);
	auto ptr9 = kernel.heap->malloc(5000);
	kprintf("init: ");
	kprint_heap_info();
	kernel.heap->free(ptr2);
	kprintf("ptr2: ");
	kprint_heap_info();
	kernel.heap->free(ptr3);
	kprintf("ptr3: ");
	kprint_heap_info();
	kernel.heap->free(ptr8);
	kprintf("ptr8: ");
	kprint_heap_info();
	kernel.heap->free(ptr6);
	kprintf("ptr6: ");
	kprint_heap_info();
	kernel.heap->free(ptr4);
	kprintf("ptr4: ");
	kprint_heap_info();
	kernel.heap->free(ptr1);
	kprintf("ptr1: ");
	kprint_heap_info();
	kernel.heap->free(ptr5);
	kprintf("ptr5: ");
	kprint_heap_info();
	kernel.heap->free(ptr7);
	kprintf("ptr7: ");
	kprint_heap_info();
	kernel.heap->free(ptr9);

	auto hdr = reinterpret_cast<BlockHeader *>(ptr9 - sizeof(BlockHeader));
	kprintf("Bytes: %d, next: %d, prev: %d, header: %d\n",
			hdr->bytes,
			hdr->next->bytes,
			hdr->prev->bytes,
			sizeof(BlockHeader));

	/*
		Main timer
	hpet::init();
	auto comp = hpet::comparator::Comparator(0);
	comp.set_periodic(100);
	*/

	// Just in sake of debugging
	for (u64 i = 0; i < 999999999999; i++) {
		u8 x  = i + 1;
		u16 y = x + 3 - i;

		if (i % 6000000 == 0) {
			// kprintf(".");
		}
	}
	kprintf("Done :/");
	for (;;)
		;
}