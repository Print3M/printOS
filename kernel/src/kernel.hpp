#pragma once
#include <acpi/acpi.hpp>
#include <acpi/apic/madt.hpp>
#include <acpi/hpet/hpet.hpp>
#include <console/console.hpp>
#include <cpu/cpu.hpp>
#include <drivers/keyboard.hpp>
#include <framebuffer/framebuffer.hpp>
#include <gdt/gdt.hpp>
#include <idt/idt.hpp>
#include <memory/heap.hpp>
#include <memory/mmap.hpp>
#include <memory/paging.hpp>
#include <memory/pmem.hpp>

struct BootloaderData {
	FramebufferData *framebuffer = nullptr;
	ConsoleFont *font			 = nullptr;
	MemoryData *memory			 = nullptr;
	void *acpi_rsdp				 = nullptr;
};

struct ACPI_Tables {
	madt::SDT_Table *madt			   = nullptr;
	madt::ics::CPU_LAPIC_Struct *lapic = nullptr;
	madt::ics::IO_APIC_Struct *ioapic  = nullptr;
	hpet::SDT_Table *hpet			   = nullptr;
};

struct DevicesInfo {
	cpu::Info cpu = {};
};

struct KernelData {
	void *stack			  = nullptr;
	Console *console	  = nullptr;
	acpi::RSDP *acpi_rsdp = nullptr;
	gdt::GDT *gdt		  = nullptr;
	idt::IDT *idt		  = nullptr;
	ACPI_Tables acpi_tables;
	Framebuffer *framebuffer = nullptr;

	// Memory
	MemoryData *memory = nullptr;
	MemoryMap *mmap	   = nullptr;
	PMem *pmem		   = nullptr;
	Paging *paging	   = nullptr;
	Heap *heap		   = nullptr;
	Keyboard *keyboard = nullptr;
};

extern DevicesInfo devices;
extern KernelData kernel;
