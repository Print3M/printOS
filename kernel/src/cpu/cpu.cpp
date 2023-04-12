#include <acpi/apic/lapic.hpp>
#include <cpu/cpu.hpp>
#include <kutils/bits.hpp>
#include <kutils/console.hpp>
#include <libc/stdint.hpp>
#include <libc/stdlib.hpp>

namespace cpu {

void _get_cpu_brand(char *brand) {
	u32 eax = 0, ebx = 0, ecx = 0, edx = 0;

	get_cpuid(0x80000002, &eax, &ebx, &edx, &ecx);
	memcpy(&brand[0], &eax, 4);
	memcpy(&brand[4], &ebx, 4);
	memcpy(&brand[8], &edx, 4);
	memcpy(&brand[12], &ecx, 4);

	get_cpuid(0x80000003, &eax, &ebx, &edx, &ecx);
	memcpy(&brand[16], &eax, 4);
	memcpy(&brand[20], &ebx, 4);
	memcpy(&brand[24], &edx, 4);
	memcpy(&brand[28], &ecx, 4);

	get_cpuid(0x80000004, &eax, &ebx, &edx, &ecx);
	memcpy(&brand[32], &eax, 4);
	memcpy(&brand[36], &ebx, 4);
	memcpy(&brand[40], &edx, 4);
	memcpy(&brand[44], &ecx, 4);

	brand[48] = '\0';
}

void get_info(Info &info) {
	// To know more about certain registers, see:
	// https://en.wikipedia.org/wiki/CPUID
	u32 eax = 0, ebx = 0, ecx = 0, edx = 0;

	// Get manufacturer id
	get_cpuid(0x0, NULL, &ebx, &ecx, &edx);
	memcpy(&info.manufacturer[0], &ebx, 4);
	memcpy(&info.manufacturer[4], &edx, 4);
	memcpy(&info.manufacturer[8], &ecx, 4);
	info.manufacturer[12] = '\0';

	// Get model info
	get_cpuid(0x1, &eax, &ebx, NULL, &edx);
	info.stepping_id = eax & 0xf;
	eax >>= 4;
	info.model_id = eax & 0xf;
	eax >>= 4;
	info.family_id = eax & 0xf;
	eax >>= 4;
	info.type = eax & 0x3;
	eax >>= 4;

	if (info.model_id == 0xf) {
		// Extended model id
		info.model_id += eax & 0xf;
	}
	eax >>= 4;

	if (info.family_id == 0xf) {
		// Extended family id
		info.family_id += eax & 0xff;
	}

	_get_cpu_brand(info.brand);

	// Get CPU technical info
	info.logical_cores = (ebx >> 16) & 0xff;
	info.hyperthreading = test_bit(edx, 28);

	get_cpuid(0x80000008, NULL, NULL, &ecx, NULL);
	info.physical_cores = (u8) ecx;
	info.physical_cores += 1;
}

State check_state() {
	u32 edx = 0;

	// ----------------------------------------------
	get_cpuid(0x1, NULL, NULL, NULL, &edx);
	if (test_bit(edx, 0) == false) {
		return State::NO_FPU;
	}
	if (test_bit(edx, 5) == false) {
		return State::NO_MSR;
	}
	if (test_bit(edx, 9) == false) {
		// Read: 10.4.2 Presence of the Local APIC
		return State::NO_LAPIC;
	}

	// ----------------------------------------------
	get_cpuid(0x80000001, NULL, NULL, NULL, &edx);
	if (test_bit(edx, 29) == false) {
		return State::NO_LONG_MODE;
	}

	return State::SUCCESS;
}

} // namespace cpu