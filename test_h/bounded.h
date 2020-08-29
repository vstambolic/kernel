//===========================================================================//
//   Project: Operating system project
//   File:    bounded.h
//   Date:    June 2020
//===========================================================================//

#ifndef _OS1_BOUNDED_BUFFER_
#define _OS1_BOUNDED_BUFFER_



#include "semaphor.h"



class BoundedBuffer {
public:

	BoundedBuffer (unsigned size);
	virtual ~BoundedBuffer ();
	int append (char);
	char take ();
	int  fullCount(){return itemAvailable.val();};    // needed for consumer

private:
	unsigned Size;
	Semaphore mutexa, mutext;
	Semaphore spaceAvailable, itemAvailable;
	char* buffer;
	int head, tail;
   
};


#endif  // _OS1_BOUNDED_BUFFER_
