/*
	Interesting overview of GTD and segmentation (memory protection at all)
	is included in 'Intel's x86 Manual vol. 3A':
		3.1 MEMORY MANAGEMENT OVERVIEW
	Overview of different memory models utilising segmentation:
		3.2 USING SEGMENTS
	Usage of segmentation on x86-64 (IA-32e):
		3.2.4 Segmentation in IA-32e Mode
	About segment registers on x86-64:
		3.4.4 Segment Loading Instructions in IA-32e Mode
	About segment descriptors:
		3.4.5 Segment Descriptors
	About GDT (Segment Descriptors Table):
		3.5.1 Segment Descriptor Tables
*/
#pragma once
#include <libc/stdint.hpp>

namespace gdt {

enum SegmentSelector { KERNEL_CODE = 8, KERNEL_DATA = 16, USER_CODE = 24, USER_DATA = 32 };

struct __attribute__((packed)) GDTR {
	u16 size;
	u64 offset;
};

struct __attribute__((packed)) SegmentDescriptor {
	u16 limit0;
	u16 base0;
	u8 base1;
	u8 flags1;
	u8 limit1_flags2;
	u8 base2;
};

// Minimal implementation of Protected Flat Model
// 	3.2.2. Protected Flat Model
// 	"Similar design is used by several popular multitasking OS'es."
struct __attribute__((packed, aligned(0x1000))) GDT {
	SegmentDescriptor null;
	SegmentDescriptor kernel_code;
	SegmentDescriptor kernel_data;
	SegmentDescriptor user_code;
	SegmentDescriptor user_data;
};
// https://stackoverflow.com/questions/11770451/what-is-the-meaning-of-attribute-packed-aligned4
// __attribute__((packed)) -> use the smallest possible space for struct (avoid padding)
// __attribute__((aligned(0x1000))) ->  its start address can be divided by 0x1000

GDT *init();

} // namespace gdt