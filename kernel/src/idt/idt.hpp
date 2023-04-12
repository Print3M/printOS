/*
	About interrupts and exceptions in general in IM3:
		6. Interrupt and exception handling
*/
#pragma once
#include <idt/interrupts.hpp>
#include <libc/stdint.hpp>

#define ALL_IDT_ENTRIES 256

namespace idt {

// IM3, Table 3-2 - System-Segment and Gate-Descriptor Types
enum Gate {
	CALL_GATE	   = 12,
	INTERRUPT_GATE = 14,
	TRAP_GATE	   = 15
};

struct __attribute__((packed)) Entry {
	// IM3, Figure 6-8 - 64-Bit IDT Gate Descriptor
	u16 offset1;
	u16 segment_selector;
	u8 ist		 : 3;
	u8 ignore1	 : 5;
	u8 gate_type : 4;
	u8 ignore2	 : 1;
	u8 dpl		 : 2;
	u8 present	 : 1;
	u16 offset2;
	u32 offset3;
	u32 reserved1;
};

struct __attribute__((packed)) IDTR {
	// IM3, Figure 6-1 - Relationship of the IDTR and IDT
	u16 limit;
	void *base_addr;
};

// Interrupt Descript Table
class IDT {
  private:
	Entry idt[ALL_IDT_ENTRIES];

  public:
	IDT();

  private:
	void add_int(Gate gate, Interrupt index, void (*handler)(InterruptFrame *));
	void __load_idtr();
};

void enable_external_interrupts();

} // namespace idt