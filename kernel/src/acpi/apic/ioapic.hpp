/*
	I/O APIC

	# Example:
	Hardware sends IRQ0. IRQ0 is connected with INTIN2 (INTerrupt INput 2) pin
	in I/O APIC chip. To see all of these default mappings, see:
	2.4. Interrupt Signals.
*/
#pragma once
#include <libc/stdint.hpp>

namespace ioapic {

enum Polarity {
	HIGH = 0x0,
	LOW	 = 0x1
};
enum TriggerMode {
	EDGE  = 0x0,
	LEVEL = 0X1
};
enum DestMode {
	PHYSICAL = 0x0,
	LOGICAL	 = 0x1
};
enum DeliveryMode {
	FIXED			= 0x0,
	LOWEST_PRIORITY = 0x1,
	SMI				= 0x2,
	NMI				= 0x4,
	INIT			= 0x5,
	EXTINT			= 0x7
};

// IOA, Table 1. Memory Mapped Registers
enum MMIO_Reg {
	IOREGSEL = 0x00, // IOA, 3.1.1. I/O Register Select Register
	IOWIN	 = 0x10	 // IOA, 3.1.2. I/O Window Register
};

enum Reg {
	// Accessible registers offsets + access
	// IOA, Table 2. IOAPIC Registers
	IOAPICID  = 0x00, // R/W
	IOAPICVER = 0x01, // RO
	IOAPICARB = 0x02, // RO
	IOREDTBL  = 0x10  // R/W
};

// Numbers
enum IRQ {
	HPET = 13
};

// IOA, 3.2.4. I/O Redirection Table Registers (table)
union RedirEntry {
	struct __attribute__((packed)) {
		u8 intvec;			// R/W - Interrupt Vector (range: 0x10 - 0xFE)
		u8 delmod	  : 3;	// R/W - Delivery Mode
		u8 destmod	  : 1;	// R/W - Destination Mode, physical=APIC
		u8 delivs	  : 1;	// RO  - Delivery Status (0=idle, 1=send and pending)
		u8 intpol	  : 1;	// R/W - Interrupt Input Pin Polarity (0=high, 1=low)
		u8 remote_irr : 1;	// RO  - (for level trigerring)
		u8 trigmod	  : 1;	// R/W - Trigger Mode
		u8 int_mask	  : 1;	// R/W - Interrupt Mask, 1=pin is masked (inactive)
		u64 reserved  : 39; //
		u8 destination;		// R/W - APIC ID
	};
	struct __attribute__((packed)) {
		u32 lower_half;
		u32 upper_half;
	};
};

void init();

}; // namespace ioapic