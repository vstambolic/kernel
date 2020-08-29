//===========================================================================//
//   Project: Operating system project
//   File:    user.cpp
//   Date:    June 2020
//===========================================================================//
#include <iostream.h>
#include <stdlib.h>
#include <assert.h>

#include "bounded.h"
#include "intLock.h"
#include "keyevent.h"
#include "user.h"
#include "Event.h"
#include "semaphor.h"

//---------------------------------------------------------------------------//
//  Uncomment to test asynchronous signals
//---------------------------------------------------------------------------//
// #define SIGNAL
//---------------------------------------------------------------------------//

//---------------------------------------------------------------------------//
//  Ovo se menja u testu
//---------------------------------------------------------------------------//

Time TIME_SLICE = 2;   // 0 or defaultTimeSlice

int N = 3;            // 1 <= N <= 19

//---------------------------------------------------------------------------//

volatile int theEnd=0;

class Producer : public Thread {
public:
	Producer (BoundedBuffer* bb, char y, Time time_slice);
	virtual ~Producer() {waitToComplete(); }
    Thread* clone() const { return new Producer(myBuffer, x, time_slice_clone); }
protected:
	virtual void run ();
	char produce() {return x;}; // Produce an item

private:
	Time time_slice_clone;
	BoundedBuffer* myBuffer;
	char x;
	Semaphore sleep;
};



//---------------------------------------------------------------------------//
class Consumer : public Thread {
public:
	Consumer (BoundedBuffer* bb) : Thread(defaultStackSize, 0), myBuffer(bb), sleep(0) {}
	virtual ~Consumer() {waitToComplete(); }
	Thread* clone() const { return new Consumer(myBuffer); }
protected:
	virtual void run ();
	void consume(char p); // Consume an item

private:
	BoundedBuffer* myBuffer;
	Semaphore sleep;
};





void tick(){
}

//---------------------------------------------------------------------------//


Producer::Producer (BoundedBuffer* bb, char y, Time time_slice) 
: Thread(defaultStackSize, time_slice),myBuffer(bb), x(y), sleep(0), time_slice_clone(time_slice) {}
	
	void Producer::run () {
	while(!theEnd) {
		char d = produce();
		myBuffer->append(d);
		assert(1 != sleep.wait(10));
	}
}

//---------------------------------------------------------------------------//


void Consumer::consume(char p) {
	intLock
	cout<<p<<" ";
	intUnlock
} // Consume an item

void Consumer::run () {
		
	int i = 0;
	while(!theEnd) {
		char d = myBuffer->take();
		consume(d);
		if (i++ == 40) {
			assert(1 != sleep.wait(5));
			i = 0;
		}else for(int j=0;j<200;j++);
	}
 
	intLock
	cout<<endl<<"ESC pressed - empty the buffer!"<<endl;
	intUnlock
	
	while (myBuffer->fullCount()){
		char d = myBuffer->take();
		consume(d);
		dispatch();
	}
	
	
	intLock
	cout<<endl<<"Happy End"<<endl;
	intUnlock
}

#ifdef SIGNAL
Semaphore signalTest(0);
void signal1() {
	signalTest.signal();
}

#endif


int userMain (int argc, char* argv[])
{
	BoundedBuffer *buff;
	Consumer *con;
	
	intLock
	if(argc <2){
		cout<<"Invalid input!"<<endl;
		intUnlock
		return -1;
	}
	int buffSize = atoi(argv[1]);
	N = atoi(argv[2]);
	N = N>19 ? 19 : N;
	TIME_SLICE = atoi(argv[3]);
	
	if(buffSize<N) {
		cout<<"Number of Producers is larger than Buffer size!"<<endl;
		intUnlock
		return 1;
	}

#ifdef SIGNAL
	Thread *current = Thread::getThreadById(Thread::getRunningId());
	current->registerHandler(1, signal1);
	current->blockSignal(1);
#endif

	buff = new BoundedBuffer(buffSize);
	Producer **pro = new Producer*[N];
	KeyboardEvent* kev;
	int i;
	
	con = new Consumer(buff);
	  
	con->start();

	for (i=0; i<N; i++){
		pro[i] = new Producer(buff,'0'+i, TIME_SLICE);
#ifdef SIGNAL
		if (i==0)
		{
			pro[i]->registerHandler(5, signal1);
		}
#endif
		pro[i]->start();
	}
  
	kev = new KeyboardEvent(buff);
	intUnlock
	
	kev->start();

#ifdef SIGNAL
	current->unblockSignal(1);
	Semaphore timer(0);
	timer.wait(10);
	if (N > 1) {
		pro[0]->signal(5);
		// Kill thread
		pro[0]->signal(0);

		signalTest.wait(0);
	}
#endif

	for (i=0; i<N; i++){
		delete pro[i];
	}  
	delete [] pro;
	delete kev;
	delete con;
	delete buff;

	intLock
	cout<<"userMain finished!"<<endl;
	intUnlock
	return 0;
}
//---------------------------------------------------------------------------//
