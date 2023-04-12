/*
	About APIC in general in IM3:
		10. Advanced Programmable Interrupt Controller (APIC)

	Local APIC handles interrupts from externally connected I/O devices
	and local interrupt sources (processor LINT0 and LINT1 pins, APIC timer,
	performance-monitoring counters, thermal sensors, internal APIC error
	detector). LAPIC manages all external interrupts for CPU. There is one
	LAPIC in each CPU in the system.

	Useful commands Qemu monitor commands: info pic, info lapic, info irq.
*/
#pragma once
#include <libc/stdint.hpp>

namespace lapic {

enum Const {
	BASE_MSR = 0x1b,
};
enum Reg {
	// IM3, Table 10-1 Local APIC Register Address Map
	ID		 = 0x020,
	VERSION	 = 0x030,
	TPR		 = 0x080,
	EOI		 = 0x0b0,
	LDR		 = 0x0d0,
	DFR		 = 0x0e0,
	SPURIOUS = 0x0f0,
	IRR		 = 0x200,
	ERR		 = 0x280,
	IRC1	 = 0x300, // IM3, 10.6.1 Interrupt Command Register
	IRC2	 = 0x310
};

// IM3, 10.4.4 Local APIC Status and Location
struct __attribute__((packed)) MSR {
	u8 reserved1;
	u8 bsp		  : 1;
	u8 reserved2  : 2;
	u8 is_enabled : 1;
	u32 base_addr : 24;
};

void init();
void write_eoi();

}; // namespace lapic