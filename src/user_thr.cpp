#include "user_thr.h"


User_Thread::~User_Thread() {
	for (int i=0; i < argc; i++)
		delete[] argv[i];
	delete[] argv;
}
