#pragma once

// Vector (entries) indexes in IDT.
// For all standard possibilities, see:
// IM3, Table 6-1. Protected-Mode Exceptions and Interrupts
enum Interrupt {
	DIVISION_BY_ZERO = 0,
	PAGE_FAULT		 = 14,
	TIMER			 = 40,
	SPURIOUS		 = 0xff
};

struct InterruptFrame;

void interrupt(Interrupt isr_index);

__attribute__((interrupt)) void page_fault_handler(InterruptFrame *frame);
__attribute__((interrupt)) void divide_by_zero_handler(InterruptFrame *frame);
__attribute__((interrupt)) void timer_handler(InterruptFrame *frame);
__attribute__((interrupt)) void spurious_handler(InterruptFrame *frame);
__attribute__((interrupt)) void apic_error_handler(InterruptFrame *frame);