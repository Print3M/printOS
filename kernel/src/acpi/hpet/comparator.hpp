#pragma once
/*
	Read: HPT, 2.4.2. Interrupt Mapping
	Each comparator selects where an interrupt should be send. In this OS the
	'Standard Option' (HPET.LEG_RT_CNF must be set to 0) is used. This mode
	makes it possible to map every comparator to the IRQ<n> selected by us. There
	is no legacy requirements. We're using I/O APIC to handle these IRQs and
	pass them on. Comparator interrupt settings should match I/O APIC settings.

	Read: HPT, 2.4.5 Interrupt Levels
			   2.4.6 Handling Interrupts

	Important notes for I/O APIC settings:
		- All interrupts are 'active high'
		- Edge-triggered option is used
*/
#include <libc/stdint.hpp>

namespace hpet::comparator {

enum Const {
	NANOSECONDS_PER_TICK = 100,
	BLOCK_SZ			 = 4 * 8, // 4 * 64-bit registers
	FIRST_OFF			 = 0x100
};

enum Type {
	ONE_SHOT = 0,
	PERIODIC = 1
};

// Register offsets: HPT, Table 2. Memory-Mapped Registers
enum Reg {
	CONFIG = 0x0,
	COMPAR = 0x8,
	FSB	   = 0xf
};

// HPT, 2.3.8 Timer N Configuration and Capabilities Register
struct __attribute__((packed)) ConfigReg {
	u8 reserved1	   : 1; //
	u8 int_type_cnf	   : 1; // interrupts: 0=level triggered, 1=edge triggered
	u8 int_enb_cnf	   : 1; // enable interrupts
	u8 type_cnf		   : 1; // set periodic or one-shot type
	u8 per_int_cap	   : 1; // is periodic available?
	u8 size_cap		   : 1; // timer size, 0=32-bits, 1=64-bits
	u8 val_set_cnf	   : 1; // (periodic-mode only, for one-shot should be 0)
	u8 reserved2	   : 1; //
	u8 mode32_cnf	   : 1; // 0=use 64-bit timer mode, 1=use 32-bit
	u8 int_route_cnf   : 5; // I/O APIC interrupt number (IRQ<n>)
	u8 fsb_en_cnf	   : 1; // enable FSB (we use I/O APIC, so should be 0)
	u8 fsb_int_del_cap : 1; //
	u16 reserved3;			//
	u32 int_route_cap;		//
};

// HPT, 2.3.10 Timer N FSB Interrupt Route Register
struct __attribute__((packed)) FsbReg {
	u32 tn_fsb_int_val;
	u32 tn_fsb_int_addr;
};

struct __attribute__((packed)) MMIO {
	ConfigReg config_reg;
	u64 compar_reg;
	FsbReg fsb_reg;
	u64 reserved;
};

class Comparator {
  private:
	u8 __id;

  public:
	Comparator(u8 timer_id);
	void set_one_shot(size ms_time);
	void set_periodic(size ms_time);
	u64 get_compar_value();

  private:
	void *get_compar_reg_ptr(u64 reg);
	void write_reg(u64 reg, u64 val);
	u64 read_reg(u64 reg);
	void set_config(ConfigReg &reg);
	void set_compar_value(u64 val);
	u32 get_available_ioapic_routes();
};

}; // namespace hpet::comparator