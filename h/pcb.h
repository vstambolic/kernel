#ifndef PCB_H_
#define PCB_H_

#include "thread.h"
#include "list.h"

// Signals ------------------------
#define SIGNAL_COUNT 16
#define	SIGNAL0 0
#define SIGNAL1 1
#define SIGNAL2 2

class PCB {

private:

	static ID pcb_cnt; 		 		  			   // PCB counter
    ID id;								 		   // PCB ID

    static const StackSize MAX_STACK_SIZE;		   // Mozda bolje da bude #define jer se koristi u statickim running
    static const StackSize MIN_STACK_SIZE;

    static void run_wrapper();						// thread run method wrapper
    static void idle_wrapper();						// idle method, does nothing

    List<PCB*> on_hold_list;

    List<SignalHandler> handler_list[SIGNAL_COUNT];	// Array of handler list
    List<SignalId> signal_queue;					// generated signals queue
    unsigned signal_block_flag;						// bit indicates if signal is allowed 1 -> allowed 0 -> blocked
    static unsigned global_signal_block_flag;		// same but globally


    enum Status {INITIALIZED, READY, RUNNING, SUSPENDED, TERMINATED, IDLE};
   	unsigned* stack;  								// Allocated memory for stack
   	unsigned ss;      							    // Stack Segment Register
   	unsigned sp;								    // Stack Pointer Register
   	unsigned bp;									// Base Pointer Register
   	Time time;		  							    // Time slice for execution
   	Status status;	 							    // Status
   	Thread * thread;								// Matching Thread
   	PCB * parent;									// Parent Thread
   	//unsigned lock_cnt;							 	// Interrupt Level counter
   	unsigned suspension_recover_code;				// Suspension code, set after thread gets unblocked

   	static volatile PCB *volatile running;		    // Running PCB
    static volatile PCB*const idle;					// Idle PCB

    static List<PCB*> list;							// List of all PCBs

    PCB(ID pcb_id); 								// Kernel
public:
    PCB(StackSize stack_size, Time time_slice, Thread* thread);
    ~PCB();

    ID get_id();
    static ID get_running_id();
    static Thread* get_thread_by_id(ID id);
    void start();
    void put_on_hold();
    void release_from_hold() volatile;
    static void clean_up();

    // Signal operations ---------------------------------------------------------

    void signal(SignalId signal_id) volatile;
	void register_handler(SignalId signal_id, SignalHandler handler);
	void unregister_all_handlers(SignalId signal_id);
	void swap(SignalId signal_id, SignalHandler handler1, SignalHandler handler2);
	void block_signal(SignalId signal_id);
	static void block_signal_globally(SignalId signal_id);
	void unblock_signal(SignalId signal_id);
	static void unblock_signal_globally(SignalId signal_id);

	void handle_signals() volatile;

    // Friends --------------------------------------------------------------------

	friend void interrupt timer(...);
	friend class KernelSem;
	friend class KernelEv;
};




#endif
