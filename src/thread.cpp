#include "thread.h"

#include "PCB.h"
#include "timer.h"
#include "utils.h"

/*--------------- ID getters ---------------*/

ID Thread::getId() {
	if (myPCB)
		return myPCB->get_id();
	else
		return -1;
}
ID Thread::getRunningId() {
	return PCB::get_running_id();
}

Thread* Thread::getThreadById(ID id) {
	return PCB::get_thread_by_id(id);
}
/*--------------- Constructor ---------------*/

Thread::Thread (StackSize stackSize, Time timeSlice) {
		myPCB = new PCB(stackSize, timeSlice, this);
}

Thread::~Thread() {
		if (myPCB) {
			waitToComplete();
			delete myPCB;
		}
}

/* Thread operations */

void Thread::start() {
	if (myPCB)
		myPCB->start();
}

void Thread::waitToComplete() {
	if (myPCB)
		myPCB->put_on_hold();
}

void dispatch() {
	DISABLE_INTERRUPTS
		switch_context_code = DEMANDED;
		timer();
	ENABLE_INTERRUPTS
}

/* Signal operations */
void Thread::signal(SignalId signal) {
	if (myPCB)
		myPCB->signal(signal);
}

void Thread::registerHandler(SignalId signal, SignalHandler handler) {
	if (myPCB)
		myPCB->register_handler(signal, handler);
}

void Thread::unregisterAllHandlers(SignalId id) {
	if (myPCB)
		myPCB->unregister_all_handlers(id);
}
void Thread::swap(SignalId id, SignalHandler hand1, SignalHandler hand2) {
	if (myPCB)
		myPCB->swap(id, hand1, hand2);
}
void Thread::blockSignal(SignalId signal) {
	if (myPCB)
		myPCB->block_signal(signal);
}
void Thread::blockSignalGlobally(SignalId signal) {
	PCB::block_signal_globally(signal);
}
void Thread::unblockSignal(SignalId signal) {
	if (myPCB)
		myPCB->unblock_signal(signal);
}
void Thread::unblockSignalGlobally(SignalId signal) {
	PCB::unblock_signal_globally(signal);
}


