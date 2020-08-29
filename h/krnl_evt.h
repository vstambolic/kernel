#ifndef KRNL_EVT_H_
#define KRNL_EVT_H_

/*#include "semaphor.h"*/
typedef unsigned char IVTNo;


class PCB;
class IVTEntry;

class KernelEv /* : public Semaphore*/ {
private:
	PCB* pcb;
	IVTNo ivt_no;
	int val; // 1 -> pcb blocked on binary semaphore | 0 -> pcb not blocked on binary semaphore
public:
	KernelEv(IVTNo ivt_no);
	~KernelEv();
	/*int wait(Time maxTimeToWait);	//override
	int signal(int n = 0);			//override*/
	void wait();
	void signal();
};

#endif


