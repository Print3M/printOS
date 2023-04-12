#include <gdt/gdt.hpp>
#include <libc/stdint.hpp>

/*
	Segmentation is still required in x86-64 but we use that as many
	other kernels - we are making flat model using four segments. Each
	segment holds entire address space.

	To better understanding why whe are using segmentation
	to make just flat-model:
		https://wiki.osdev.org/Segmentation#Notes_Regarding_C

*/

namespace gdt {

extern "C" void __load_gdt(GDTR *gdtr_ptr);

GDT gdt = {
	.null =
		{
			.limit0 = 0x0,
			.base0 = 0x0,
			.base1 = 0x0,
			.flags1 = 0x0,
			.limit1_flags2 = 0x0,
			.base2 = 0x0,
		},
	.kernel_code =
		{
			.limit0 = 0x0,
			.base0 = 0x0,
			.base1 = 0x0,
			.flags1 = 0x9a,
			.limit1_flags2 = 0xa0,
			.base2 = 0x0,
		},
	.kernel_data =
		{
			.limit0 = 0x0,
			.base0 = 0x0,
			.base1 = 0x0,
			.flags1 = 0x92,		   // 10010010
			.limit1_flags2 = 0xa0, // 10100000
			.base2 = 0x0,
		},
	.user_code =
		{
			.limit0 = 0x0,
			.base0 = 0x0,
			.base1 = 0x0,
			.flags1 = 0x9a,
			.limit1_flags2 = 0xa0,
			.base2 = 0x0,
		},
	.user_data =
		{
			.limit0 = 0x0,
			.base0 = 0x0,
			.base1 = 0x0,
			.flags1 = 0x92,
			.limit1_flags2 = 0xa0,
			.base2 = 0x0,
		},
};

GDTR gdtr = {.size = sizeof(GDT) - 1, .offset = (u64) &gdt};

GDT *init() {
	__load_gdt(&gdtr);

	return &gdt;
}

}; // namespace gdt