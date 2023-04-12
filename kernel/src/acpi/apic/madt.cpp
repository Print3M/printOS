#include <acpi/acpi.hpp>
#include <acpi/apic/madt.hpp>
#include <kernel.hpp>
#include <kutils/console.hpp>

namespace madt {

void dbg_print_lapic_info(void) {
	// TODO: enumerate ALL APICs for all CPUs
	auto apic = reinterpret_cast<ics::CPU_LAPIC_Struct *>(ics::get_header(ics::CPU_LAPIC));

	kprintf("ACPI CPU UID: %d, APIC ID: %d, flags: %d\n",
			apic->acpi_cpu_uid,
			apic->apic_id,
			apic->flags);
}

}; // namespace madt

namespace madt::ics {

Header *get_header(Type type) {
	/*
		Return pointer to the MADT Interrupt Controller Structure or NULL if not found.
	*/
	auto header = kernel.acpi_tables.madt->ics_table;

	for (u8 id = 0; id < 0xf; id++) {
		if (header->type == type) {
			return header;
		}

		header = (Header *) ((u64) header + header->length);
	}

	return nullptr;
}

}; // namespace madt::ics