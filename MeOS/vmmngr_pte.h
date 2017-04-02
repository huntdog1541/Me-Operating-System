#ifndef VMMNGR_PTE_H_130516
#define VMMNGR_PTE_H_130516

#ifdef __cplusplus
extern "C" {
#endif

#include "types.h"
#include "mmngr_phys.h"

	// define utilities for a page table entry (pte)

	enum PAGE_PTE_FLAGS
	{
		I86_PTE_PRESENT = 1,
		I86_PTE_WRITABLE = 2,
		I86_PTE_USER = 4,
		I86_PTE_WRITETHROUGH = 8,
		I86_PTE_NOT_CACHABLE = 0x10,
		I86_PTE_ACCESSED = 0x20,
		I86_PTE_DIRTY = 0x40,
		I86_PTE_PAT = 0x80,
		I86_PTE_CPU_GLOBAL = 0x100,
		I86_PTE_LV4_GLOBAL = 0x200,
		I86_PTE_FRAME = 0xFFFFF000		// here the first digit was 7 instead of F (0x7FFF) by brokenthorn...
	};

	typedef uint32 pt_entry;		// every page table entry is 32 bit wide

	// INTERFACE

	// set a flag (see enum above) in a pt_entry
	extern void pt_entry_add_attrib(pt_entry* entry, uint32 attrib);

	// unset a flag from a pt_entry
	extern void pt_entry_del_attrib(pt_entry* entry, uint32 attrib);

	// test if attrib is set
	extern bool pt_entry_test_attrib(pt_entry* entry, uint32 attrib);

	// set the frame address (physical address space) of a pt_entry
	extern void pt_entry_set_frame(pt_entry* entry, physical_addr address);

	// check if entry is present in memory
	extern bool pt_entry_is_present(pt_entry entry);

	// check if entry is writable
	extern bool pt_entry_is_writable(pt_entry entry);

	// get the frame address of the pt_entry
	extern physical_addr pt_entry_get_frame(pt_entry entry);

#ifdef __cplusplus
}
#endif

#endif
