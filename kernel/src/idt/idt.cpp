#include <acpi/apic/lapic.hpp>
#include <gdt/gdt.hpp>
#include <idt/idt.hpp>
#include <idt/interrupts.hpp>
#include <libc/stdint.hpp>
#include <libc/stdlib.hpp>

namespace idt {

IDT::IDT() {
	this->add_int(TRAP_GATE, Interrupt::DIVISION_BY_ZERO, &divide_by_zero_handler);
	this->add_int(INTERRUPT_GATE, Interrupt::PAGE_FAULT, &page_fault_handler);
	this->add_int(INTERRUPT_GATE, Interrupt::SPURIOUS, &spurious_handler);
	this->add_int(INTERRUPT_GATE, Interrupt::TIMER, &timer_handler);

	this->__load_idtr();
}

void IDT::add_int(Gate gate, Interrupt index, void (*handler)(InterruptFrame *)) {
	/*
		Add interrupt handler into IDT.
		NOTE: It doesn't check if the table entry is already occupied.
	*/
	auto entry = &idt[index];
	entry->present = 1;
	entry->gate_type = gate;
	entry->segment_selector = gdt::SegmentSelector::KERNEL_CODE;

	u64 offset = (u64) handler;
	entry->offset1 = (u16) (offset & 0xffff);
	offset >>= 16;
	entry->offset2 = (u16) (offset & 0xffff);
	offset >>= 16;
	entry->offset3 = (u32) (offset & 0xffffffff);
}

void IDT::__load_idtr() {
	// Load IDT to IDTR
	u16 limit = sizeof(Entry) * ALL_IDT_ENTRIES;
	IDTR idtr = {.limit = limit, .base_addr = (void *) this->idt};
	__asm__ volatile("lidt %0" ::"m"(idtr));
}

void enable_external_interrupts() {
	/*
		DEVLOG: I literally spent weeks debugging IOAPIC, LAPIC and HPET.
		I rewrote the kernel to C++ thinking it would help. It turned out
		that the IF flag is not set in the CPU. Well... n00b exposed.
	*/
	asm("sti");
}

} // namespace idt