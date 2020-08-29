#include "PCB.h"

#include <dos.h>
#include "SCHEDULE.H"
#include "utils.h"

// Reserved IDs ---------------------------------------------
#define KERNEL_ID 0
#define IDLE_ID 1
#define FREE_ID 2

// Static Members -------------------------------------------

volatile PCB*volatile PCB::running = new PCB(KERNEL_ID);
volatile PCB*const PCB::idle = new PCB(IDLE_ID);

void PCB::clean_up(){
   	delete running;
   	delete idle;
}
void PCB::run_wrapper() {
	running->thread->run();

	LOCK
	 	// running->release_from_hold();
		while (!running->on_hold_list.empty()) {
				PCB* tmp = running->on_hold_list.pop_front();
				tmp->status = READY;
				Scheduler::put(tmp);
		}
		running->status = TERMINATED;

		if (running->parent != nullptr)
			running->parent->signal_queue.push_back(SIGNAL1); 	// running->parent->signal(SIGNAL1);
		running->signal_queue.push_back(SIGNAL2);				// running->signal(SIGNAL2);

		running->handle_signals();
	UNLOCK

	dispatch();
}

void PCB::idle_wrapper() {
	while(1);				// in case scheduler doesn't have any ready PCBs, do nothing
}

const StackSize PCB::MAX_STACK_SIZE = 65536; 	// 64 KB
const StackSize PCB::MIN_STACK_SIZE = 1024;		// 1 KB

List<PCB*> PCB::list;

// PCB count & ID getters --------------------------------------------

ID PCB::pcb_cnt = FREE_ID;
ID PCB::get_id() {
	return this->id;
}
ID PCB::get_running_id() {
    return running->id;
}

Thread* PCB::get_thread_by_id(ID id) {
	LOCK
	Thread* thread = nullptr;
	for (List<PCB*>::Iterator it = list.begin(); it.exists(); ++it)
		if ((*it)->get_id() == id) {
			thread = (*it)->thread;
			break;
		}
	UNLOCK
	return thread;
}

// Constructors & Destructor -------------------------------------------
PCB::PCB (StackSize stack_size, Time time, Thread * thread) :		\
	id(pcb_cnt++), time(time), thread(thread), parent((PCB*)running), status(INITIALIZED), suspension_recover_code(1)  \
	{

   	this->signal_block_flag = parent->signal_block_flag;
   //this->signal_queue = parent->signal_queue;
   	for (int i = 0; i < SIGNAL_COUNT; i++)
   		this->handler_list[i] = parent->handler_list[i];

	if (stack_size > MAX_STACK_SIZE)
		stack_size = MAX_STACK_SIZE;
	else
		if (stack_size < MIN_STACK_SIZE)
			stack_size = MIN_STACK_SIZE;

	stack_size /= sizeof(unsigned);		 	// sizeof(unsigned) = 2B
											// stack_size now represents number of stack entries
	LOCK
		this->stack = new unsigned[stack_size];
	UNLOCK
	this->stack[stack_size - 1] = 0x200;  // PSW register, setting the I flag

#ifndef BCC_BLOCK_IGNORE
	this->stack[stack_size - 2] = FP_SEG(run_wrapper);  // CS register
	this->stack[stack_size - 3] = FP_OFF(run_wrapper);  // IP register
														// CS:IP all together gives PC
	/*
	 *  entries [stack_size - 4] to [stack_size - 11]
	 *  are reserved for program available registers
	 */
	this->ss = FP_SEG(this->stack + stack_size - 12);				//sizeof(unsigned*) == 4
	this->sp = FP_OFF(this->stack + stack_size - 12);
	this->bp = this->sp;
#endif
	list.push_back(this);
}

PCB::PCB(ID pcb_id) : \
		id(pcb_id), thread(nullptr), parent(nullptr), suspension_recover_code(0), signal_block_flag(~0U) {

	switch (pcb_id) {
	case KERNEL_ID: {
		this->stack = nullptr;
		this->bp = this->ss = this->sp = 0U;
		this->status = RUNNING;
		this->time = defaultTimeSlice;
		break;
	}
	case IDLE_ID: {
		this->time = 1;
		this->status = IDLE;
		LOCK
			this->stack = new unsigned[MIN_STACK_SIZE];
		UNLOCK

		this->stack[MIN_STACK_SIZE - 1] = 0x200;  // PSW register, setting the I flag

	#ifndef BCC_BLOCK_IGNORE
		this->stack[MIN_STACK_SIZE - 2] = FP_SEG(idle_wrapper);  // CS register
		this->stack[MIN_STACK_SIZE - 3] = FP_OFF(idle_wrapper);  // IP register
																 // CS:IP all together gives PC
		/*
		 *  entries [stack_size - 4] to [stack_size - 11]
		 *  are reserved for program available registers
		 */
		this->ss = FP_SEG(this->stack + MIN_STACK_SIZE - 12);				//sizeof(unsigned*) == 4
		this->sp = FP_OFF(this->stack + MIN_STACK_SIZE - 12);
		this->bp = this->sp;
	#endif
		break;
	}
	}
	//      list.push_back(this);

}
PCB::~PCB() {
	LOCK
		if (id != KERNEL_ID)
			delete[] stack;
		if (id != KERNEL_ID && id != IDLE_ID) {
			thread->myPCB = nullptr;
			List<PCB*>::Iterator it;					// Remove from list of all PCBs
			for (it= list.begin(); (*it) != this; ++it);
			list.remove(it);
		}
	UNLOCK
}

void PCB::start() {
	LOCK
		if (this->status == INITIALIZED) {
			Scheduler::put(this);
			this->status = READY;
		}
	UNLOCK
}

void PCB::put_on_hold() {
	LOCK
	if (running != this && this->status != TERMINATED && this->status != INITIALIZED) {
		running->status = SUSPENDED;
		on_hold_list.push_back((PCB*)running);
		UNLOCK
		dispatch();
	}
	else
		UNLOCK

}

void PCB::release_from_hold() volatile {
	LOCK
		while (!on_hold_list.empty()) {
			PCB* tmp = on_hold_list.pop_front();
			tmp->status = READY;
			Scheduler::put(tmp);
		}
	UNLOCK
}


unsigned PCB::global_signal_block_flag = ~0U; // 1111...11 binary (unsigned type);

//////////////////////////////////////////////////////////////////

/* Signal operations */
void PCB::signal(SignalId signal_id) volatile {
	if (signal_id >= SIGNAL_COUNT)
		return;
	LOCK
		this->signal_queue.push_back(signal_id);
	UNLOCK
}
void PCB::register_handler(SignalId signal_id, SignalHandler handler) {
	if (signal_id >= SIGNAL_COUNT)
		return;
	LOCK
		handler_list[signal_id].push_back(handler);
	UNLOCK
}
void PCB::unregister_all_handlers(SignalId signal_id) {
	if (signal_id >= SIGNAL_COUNT)
		return;
	LOCK
		handler_list[signal_id].clear();
	UNLOCK
}
void PCB::swap(SignalId signal_id, SignalHandler handler1, SignalHandler handler2) {
	if (signal_id >= SIGNAL_COUNT)
		return;
	LOCK
		List<SignalHandler>::Iterator i, j;
		for (List<SignalHandler>::Iterator it = handler_list[signal_id].begin(); it.exists() && !(i.exists() && j.exists()); ++it) {
			if (!i.exists() && *it == handler1)
				i = it;
			if (!j.exists() && *it == handler2)
				j = it;
		}
		if (i.exists() && j.exists()) {
			(*i) = handler2;
			(*j) = handler1;
		}
	UNLOCK
}
void PCB::block_signal(SignalId signal_id) {
	if (signal_id >= SIGNAL_COUNT)
		return;
	LOCK
		this->signal_block_flag &= ~(1U << signal_id);
	UNLOCK
}
void PCB::block_signal_globally(SignalId signal_id) {
	if (signal_id >= SIGNAL_COUNT)
		return;
	LOCK
		global_signal_block_flag  &= ~(1U << signal_id);
	UNLOCK
}
void PCB::unblock_signal(SignalId signal_id) {
	if (signal_id >= SIGNAL_COUNT)
		return;
	LOCK
		this->signal_block_flag |= (1U << signal_id);
	UNLOCK
}
void PCB::unblock_signal_globally(SignalId signal_id) {
	if (signal_id >= SIGNAL_COUNT)
		return;
	LOCK
		global_signal_block_flag |= (1U << signal_id);
	UNLOCK
}
void PCB::handle_signals() volatile {
	List<SignalId>::Iterator it = signal_queue.begin();
	while (it.exists()) {
		if (((signal_block_flag & global_signal_block_flag) >> *it) & 1U) {
			for (List<SignalHandler>::Iterator jt = handler_list[*it].begin(); jt.exists(); ++jt)
				(*jt)();
			if (*it == SIGNAL0) {
				release_from_hold();
				process_killed_flag = 1;
				delete this; 			// Process killed
				break;
			}
			List<SignalId>::Iterator old = it;
			++it;
			signal_queue.remove(old);
		}
		else
			++it;
	}
}
