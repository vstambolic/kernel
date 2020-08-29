#ifndef USER_THR_H_
#define USER_THR_H_

#include "thread.h"

int userMain(int argc, char* argv[]);

class User_Thread : public Thread {
private:
	int argc;
	char** argv;
	int return_code;

public:
	User_Thread(int _argc, char** _argv) : Thread(), argc(_argc), argv(_argv), return_code(0) {}
	void run() {
		return_code = userMain(argc, argv);
	}
	int get_return_code() const {
		return return_code;
	}
	~User_Thread();

};



#endif
/*
#ifndef USERTHR_H
#define USERTHR_H
#include "Thread.h"

class UserThread : public Thread {
	int argc;
	char **argv;
	int retCode;

public:
	UserThread(int argc, char** argv) {
		this->argc = argc;
		this->argv = argv;
		retCode = 0;
	}

	int returnCode() { return retCode; }
	~UserThread() { waitToComplete(); }
protected:
	void run();
};
*/

