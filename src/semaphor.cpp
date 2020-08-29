#include "semaphor.h"
#include "krnl_sem.h"



Semaphore::Semaphore(int init) {
	LOCK
		myImpl = new KernelSem(init);
	UNLOCK
}
Semaphore::~Semaphore() {
	LOCK
		delete myImpl;
	UNLOCK
}

int Semaphore::signal(int n) {
	return myImpl->signal(n);
}

int Semaphore::wait(Time maxTimeToWait) {
	return myImpl->wait(maxTimeToWait);
}
int Semaphore::val() const {
	return myImpl->get_val();
}
