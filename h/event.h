#ifndef _event_h_
#define _event_h_

#include "iv_table.h"

typedef unsigned char IVTNo;


class KernelEv;

class Event {

public:
	Event(IVTNo ivtNo);
   ~Event();
   void wait ();

protected:

   friend class KernelEv;
   void signal(); // can call KernelEv

private:
   KernelEv* myImpl;
};

#endif

