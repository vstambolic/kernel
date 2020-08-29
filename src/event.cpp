#include "event.h"
#include "utils.h"
#include "krnl_evt.h"

Event::Event(IVTNo ivtNo) {
		myImpl = new KernelEv(ivtNo);
}
Event::~Event() {
		delete myImpl;
}

void Event::wait() {
	myImpl->wait();
}
void Event::signal() {
	myImpl->signal();
}
