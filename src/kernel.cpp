#include <iostream.h>

#include "user_thr.h"
#include "timer.h"
#include "pcb.h"


int main(int argc, char* argv[]) {

	User_Thread user_thread(argc, argv);

	initialize_ivt();

	user_thread.start();
	user_thread.waitToComplete();

	restore_ivt();

	PCB::clean_up();

	int return_code = user_thread.get_return_code();
	cout << endl << "Process finished with code: " << return_code << endl;

	return return_code;
}

