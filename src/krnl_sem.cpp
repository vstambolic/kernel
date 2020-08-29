#include "krnl_sem.h"

#include "utils.h"
#include "SCHEDULE.H"
#include "pcb.h"

List<KernelSem*> KernelSem::list;

/*----------------- Constructor -----------------------*/
KernelSem::KernelSem(int _val) : val(_val) {
		list.push_back(this);
}

int KernelSem::get_val() const {
	return this->val;
}
/*--------------- Block / Unblock ---------------------*/

void KernelSem::block(Suspended_PCB suspended_pcb) {			// suspend thread
	alert_mutex = 1;
	PCB::running->status = PCB::SUSPENDED;
	suspended_queue.push_back(suspended_pcb);
	alert_mutex = 0;

}
void KernelSem::unblock() {			// unblock thread
	alert_mutex = 1;
	PCB * tmp = suspended_queue.pop_front().pcb;
	tmp->suspension_recover_code = BEFORE_TIME;
	tmp->status = PCB::READY;
	Scheduler::put(tmp);
	alert_mutex = 0;
}

/*---------------- Wait / Signal ----------------------*/
int KernelSem::wait (Time maxTimeToWait) {
	LOCK
		int x = 1;
		if (--val < 0) {
			block(Suspended_PCB((PCB*)PCB::running, maxTimeToWait));
			UNLOCK
			dispatch();										 // when running thread gets unblocked and continues
			// ...											 // its execution, it will start again from here
			x = PCB::running->suspension_recover_code;
		}
		else
			UNLOCK

	return x;
}
int KernelSem::signal(int n) {
	LOCK
		if (n == 0) {											// if n == 0, signal() behaves conventionally
			if (val++ < 0)										// returns 0
				unblock();
		}
		else
			if (n > 0) {										// if n > 0, signal() unblocks maximum n threads
				int i = 0;										// and returns the number of unblocked threads
				while (!suspended_queue.empty() && i++ < n)
					unblock();
				val += n;
				n = i;
			}
	UNLOCK
	return n;
}

volatile int KernelSem::alert_mutex = 0;
void KernelSem::alert_semaphores() {
	LOCK
	if (alert_mutex == 0) {
		alert_mutex = 1;

		for (List<KernelSem*>::Iterator i = KernelSem::list.begin(); i.exists(); ++i) {
			List<Suspended_PCB>::Iterator j = (*i)->suspended_queue.begin();
			while (j.exists()) {
				if ((*j).suspend_time != 0 && (--((*j).suspend_time) == 0)) {
					PCB * tmp = (*j).pcb;
					tmp->suspension_recover_code = TIME_EXPIRED;
					tmp->status = PCB::READY;
					Scheduler::put(tmp);

					(*i)->val++;

					List<Suspended_PCB>::Iterator old = j;
					++j;
					(*i)->suspended_queue.remove(old);
				}
				else
					++j;
			}

		}
		alert_mutex = 0;
	}
	UNLOCK
}

KernelSem::~KernelSem() {
	LOCK
		List<KernelSem*>::Iterator i; 						// remove this semaphore from the list
		for (i = list.begin(); (*i) != this; ++i); 		    // of all semaphores
		list.remove(i);
	UNLOCK
}

