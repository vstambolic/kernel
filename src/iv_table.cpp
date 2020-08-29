#include "iv_table.h"
#include <dos.h>
#include "utils.h"
#include "krnl_evt.h"

IVTEntry* IVTEntry::iv_table[IVT_SIZE] = {nullptr};

/*----- Constructor ------*/
#ifndef BCC_BLOCK_IGNORE
IVTEntry::IVTEntry(IVTNo entry_num, Intr_Routine new_routine) 	\
		: entry_num(entry_num), new_routine(new_routine), krnl_event(nullptr), old_routine(getvect(entry_num)) {
	iv_table[entry_num] = this;
}
#endif

void IVTEntry::alert_event() {
	krnl_event->signal();
}



/* Lock / Unlock ----------------------------------------------------
/*
 	static int is_locked();
	static void lock();
	static void unlock();
int IVTEntry::is_locked() {
	return lock_flag;
}
void  IVTEntry::lock() {
	asm cli
}
void  IVTEntry::unlock() {
	asm sti
}
*/

