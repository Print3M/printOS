#pragma once
#include <libc/stdint.hpp>

namespace cpu {

enum State {
	ERROR,
	SUCCESS,
	NO_LONG_MODE,
	NO_FPU,
	NO_LAPIC,
	NO_MSR
};

struct Info {
	char manufacturer[13]; // + \0
	u8 stepping_id : 4;
	u8 type		   : 2;
	u16 model_id;
	u16 family_id;
	char brand[49]; // + \0
	bool hyperthreading;
	u8 physical_cores;
	u8 logical_cores;
};

// Assembly functions
extern "C" void get_cpuid(u32 cpuid_eax, u32 *eax, u32 *ebx, u32 *ecx, u32 *edx);
extern "C" u64 read_msr(u32 msr_no);
extern "C" void write_msr(u32 msr_no, u64 value);

void get_info(Info &info);
State check_state();

} // namespace cpu