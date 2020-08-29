#include "krnl_evt.h"

#include <dos.h>
#include "pcb.h"
#include "iv_table.h"
#include "SCHEDULE.H"


KernelEv::KernelEv(IVTNo ivt_no) :/* Semaphore(0),*/ ivt_no(ivt_no), pcb((PCB*)PCB::running), val(0) {
	LOCK
	IVTEntry::iv_table[ivt_no]->krnl_event = this;
		#ifndef BCC_BLOCK_IGNORE
				setvect(ivt_no, IVTEntry::iv_table[ivt_no]->new_routine);
		#endif
	UNLOCK
}

KernelEv::~KernelEv() {
	LOCK
		#ifndef BCC_BLOCK_IGNORE
				setvect(ivt_no, IVTEntry::iv_table[ivt_no]->old_routine);
		#endif
		IVTEntry::iv_table[ivt_no]->krnl_event = nullptr;
		IVTEntry::iv_table[ivt_no] = nullptr;
	UNLOCK
}

void KernelEv::wait() {
	LOCK
	if (this->pcb == PCB::running) {
		this->val = 1;
		this->pcb->status = PCB::SUSPENDED;
		UNLOCK
		dispatch();
	}
	else
		UNLOCK
}

void KernelEv::signal() {
	//LOCK
	if (this->val) {
		this->pcb->status = PCB::READY;
		Scheduler::put(this->pcb);
		this->val = 0;
	}
	//UNLOCK
}


/*
int KernelEv::wait(Time maxTimeToWait) {	//override
	if (this->pcb == PCB::running)
		return Semaphore::wait(maxTimeToWait);
	else
		return -1;
}
int KernelEv::signal(int n) {
	if (this->val() < 0)
		return Semaphore::signal(n);
	else
		return -1;
}

*/


