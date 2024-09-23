/*
	Universal PS/2 driver.

	https://wiki.osdev.org/PS/2
	https://wiki.osdev.org/PS/2_Keyboard

	How it works:
	Actually, the PS/2 driver itself doesn't do much.
	After setting the appropriate interrupts (IRQ1 for the keyboard) on
	the APIC, the corresponding ISR is executed. The DATA port is read taking
	the scan code of the pressed key.

	Commands to the PS/2 Controller are sent via the COMMAND port. Their result
	is read from the DATA port. Data from the PS/2 devices arrives to the DATA
	port. They must be read to free up space. The distinction between devices
	(PS/2 Port 1 and Port 2) is made through different ISRs.
*/

#pragma once
#include <libc/stdint.hpp>

namespace ps2::controller {

enum class Port {
	DATA_RW	  = 0x60,
	STATUS_R  = 0x64,
	COMMAND_W = 0x64
};

enum class Command {
	TEST_CONTROLLER = 0xAA,
	GET_CONFIG		= 0x20,

	// Copy bits 0 to 3 of Data Reg to Status Reg bits 0 to 3
	CPY_LOW_TO_STAT = 0xC1,
	// Copy bits 4 to 7 of Data Reg to Status Reg bits 4 to 7
	CPY_HIGH_TO_STAT = 0xC2,

	TEST_1_PORT	   = 0xAB,
	DISABLE_1_PORT = 0xAD,
	ENABLE_1_PORT  = 0xAE,

	TEST_2_PORT	   = 0xA9,
	DISABLE_2_PORT = 0xA7,
	ENABLE_2_PORT  = 0xA8,
};

enum class CtrlTestResult {
	PASSED = 0x55,
	FAILED = 0xFC
};

bool test();
u8 send_cmd(Command cmd, u8 data);
void set_status(u8 val);
u8 read_status();
void set_data(u8 val);
u8 read_data();

} // namespace ps2::controller

namespace ps2::port_1 {

enum class PortTestResult {
	PASSED		  = 0x00,
	CLOCK_STUCK_L = 0x01,
	CLOCK_STUCK_H = 0x02,
	DATA_STUCK_L  = 0x03,
	DATA_STUCK_H  = 0x04,
};

PortTestResult test();
bool is_interrupt_enabled();
void disable();
void enable();

} // namespace ps2::port_1

namespace ps2::port_2 {

bool test();
void disable();
void enable();

} // namespace ps2::port_2