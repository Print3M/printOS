#include <acpi/apic/lapic.hpp>
#include <acpi/hpet/comparator.hpp>
#include <acpi/hpet/hpet.hpp>
#include <idt/idt.hpp>
#include <idt/interrupts.hpp>
#include <kutils/console.hpp>
#include <libc/stdint.hpp>

void interrupt(Interrupt isr_index) {
	/*
		Call interrupt. Make jump to Interrupt Service Routine.
		NOTE: I've tried to make this cleaner but apparently inline assembly
		cannot put variable value as `int` argument.
	*/
	switch (isr_index) {
		case Interrupt::DIVISION_BY_ZERO:
			__asm__ volatile("int %0" ::"N"(Interrupt::DIVISION_BY_ZERO));
			break;
		case Interrupt::PAGE_FAULT:
			__asm__ volatile("int %0" ::"N"(Interrupt::PAGE_FAULT));
			break;
		case Interrupt::SPURIOUS:
			__asm__ volatile("int %0" ::"N"(Interrupt::SPURIOUS));
		default:
			break;
	}
}

__attribute__((interrupt)) void page_fault_handler(InterruptFrame *frame) {
	kprintf("page fault");

	while (1)
		;
}

__attribute__((interrupt)) void divide_by_zero_handler(InterruptFrame *frame) {
	kpanic("division by zero");

	while (1)
		;
}

size secs = 0;
u8 ms	  = 0;

__attribute__((interrupt)) void timer_handler(InterruptFrame *frame) {
	ms += 1;
	if (ms == 10) {
		ms = 0;
		secs++;
	}
	kprintf("%d.%d\r", secs, ms);

	lapic::write_eoi();
}

__attribute__((interrupt)) void spurious_handler(InterruptFrame *frame) {
	// IM3, 10.9 Spurious Interrupt (APIC)
	kprintf("SPURIOUS!!!");

	while (1)
		;
}

// TODO: Check if needed and remove
__attribute__((interrupt)) void apic_error_handler(InterruptFrame *frame) {
	kprintf("APIC ERROR!!!");

	while (1)
		;
}
