#ifndef KRNL_SEM_H_				// Semaphore Implementation
#define KRNL_SEM_H_

#include "semaphor.h"
#include "list.h"


class PCB;

class KernelSem {

private:

	class Suspended_PCB {
	public:
		Suspended_PCB(PCB* _pcb, Time max_time_to_wait) : pcb(_pcb), suspend_time(max_time_to_wait) {}
		PCB* pcb;
		Time suspend_time;
	};

	volatile int val;
	volatile List<Suspended_PCB> suspended_queue;

	static List<KernelSem*> list;	// list of all active semaphores

	void block(Suspended_PCB);
	void unblock();

	static volatile int alert_mutex;
	static void KernelSem::alert_semaphores();
public:
	KernelSem(int _val);
	virtual ~KernelSem();

	virtual int wait (Time maxTimeToWait);
	virtual int signal(int n);

	int get_val() const;

	friend void interrupt timer(...);
};

#endif

