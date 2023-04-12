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
#include <kutils/bitmap.hpp>
#include <kutils/console.hpp>
#include <memory/mmap.hpp>
#include <memory/paging.hpp>
#include <memory/pmem.hpp>
#include <pic/pic.hpp>

// Global kernel structures
KernelData kernel;
DevicesInfo devices;

extern "C" void _start(BootloaderData *bd) {
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
	kprint_cpu_info();

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
			kprint_err("unknown version of ACPI detected");
			break;
	}

	if (acpi::verify_rsdp() == false) {
		kprint_err("RSDP not verified");
	} else {
		kprint_succ("RSDP verified");
	}

	if (acpi::sdt::verify_rsdt() == false) {
		kprint_err("RSDT not verified");
	} else {
		kprint_succ("RSDT verified");
	}

	/*
		MADT
	*/
	kernel.acpi_tables.madt = (madt::SDT_Table *) acpi::sdt::get_header(acpi::sdt::MADT_SIG);
	kernel.acpi_tables.lapic =
		(madt::ics::CPU_LAPIC_Struct *) madt::ics::get_header(madt::ics::CPU_LAPIC);
	kernel.acpi_tables.ioapic =
		(madt::ics::IO_APIC_Struct *) madt::ics::get_header(madt::ics::IO_APIC);

	/*
		HPET
	*/
	kernel.acpi_tables.hpet = (hpet::SDT_Table *) acpi::sdt::get_header(acpi::sdt::HPET_SIG);
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

	/*
		Main timer
	*/
	hpet::init();
	auto comp = hpet::comparator::Comparator(0);
	comp.set_periodic(100);

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