#include <cpu/cpu.hpp>
#include <drivers/ps2.hpp>
#include <kutils/bits.hpp>

namespace ps2::controller {

u8 send_cmd(Command cmd, u8 data) {
	// Utility to execute PS/2 Controller command and read output.
	set_data(data);
	cpu::io_wait();
	cpu::outb(static_cast<u16>(Port::COMMAND_W), static_cast<u8>(cmd));
	cpu::io_wait();

	return read_data();
}

bool test() {
	auto result = send_cmd(Command::TEST_CONTROLLER, 0x0);

	return result == static_cast<u8>(CtrlTestResult::PASSED);
}

u8 read_status() {
	// Read STATUS port of PS/2 Controller
	return cpu::inb(static_cast<u8>(Port::STATUS_R));
}

void set_status(u8 val) {
	// Set STATUS port of PS/2 Controller
	send_cmd(Command::CPY_LOW_TO_STAT, val);
	send_cmd(Command::CPY_HIGH_TO_STAT, val);
}

u8 read_data() {
	// Read DATA port of PS/2 Controller
	return cpu::inb(static_cast<u16>(Port::DATA_RW));
}

void set_data(u8 val) {
	// Set DATA port of PS/2 Controller
	return cpu::outb(static_cast<u16>(Port::DATA_RW), val);
}

} // namespace ps2::controller

namespace ps2::port_1 {

using namespace ps2::controller;

PortTestResult test() {
	auto result = send_cmd(Command::TEST_1_PORT, 0x0);

	return static_cast<PortTestResult>(result);
}

bool is_interrupt_enabled() {
	// Bit 0: First PS/2 port interrupt (1 = enabled, 0 = disabled)
	auto cfg = send_cmd(Command::GET_CONFIG, 0x0);

	return test_bit(cfg, 0) == 1;
}

void disable() {
	send_cmd(Command::DISABLE_1_PORT, 0x0);
}

void enable() {
	send_cmd(Command::ENABLE_1_PORT, 0x0);
}

} // namespace ps2::port_1