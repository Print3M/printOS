/*
	PIC - Programmable Interrupt Controller
	Legacy device which is replaced by APIC.

	It should be disabled to use Local APIC and I/O APIC.
	https://wiki.osdev.org/8259_PIC
*/

namespace pic {

void disable();

}