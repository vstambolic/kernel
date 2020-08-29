#ifndef _thread_h_
#define _thread_h_


typedef unsigned long StackSize;
const StackSize defaultStackSize = 4096;

typedef unsigned int Time; 			// time, x 55ms
const Time defaultTimeSlice = 2; 	// default = 2*55ms

typedef void (*SignalHandler)();
typedef unsigned SignalId;

typedef int ID;
class PCB; 							// Kernel's implementation of a user's thread

class Thread {

public:

	 void start();
	 void waitToComplete();
	 virtual ~Thread();

	 /* ID getters */
	 ID getId();
	 static ID getRunningId();
	 static Thread * getThreadById(ID id);

	 /* Signal operations */
	 void signal(SignalId signal);
	 void registerHandler(SignalId signal, SignalHandler handler);
	 void unregisterAllHandlers(SignalId id);
	 void swap(SignalId id, SignalHandler hand1, SignalHandler hand2);
	 void blockSignal(SignalId signal);
	 static void blockSignalGlobally(SignalId signal);
	 void unblockSignal(SignalId signal);
	 static void unblockSignalGlobally(SignalId signal);

protected:

	 friend class PCB;
	 Thread (StackSize stackSize = defaultStackSize, Time timeSlice = defaultTimeSlice);
	 virtual void run() {}

private:

	 PCB* myPCB;
};

void dispatch ();

#endif
