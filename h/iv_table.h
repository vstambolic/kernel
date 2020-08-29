#ifndef IV_TABLE_H_
#define IV_TABLE_H_

#define PREPAREENTRY(ENTRY_NUM, CALL_OLD) 							    \
	void interrupt intr_routine_##ENTRY_NUM(...); 					    \
	IVTEntry ivt_entry##ENTRY_NUM(ENTRY_NUM,intr_routine_##ENTRY_NUM);  \
	void interrupt intr_routine_##ENTRY_NUM(...) {					    \
			/*IVTEntry::lock();*/										\
			IVTEntry::iv_table[ENTRY_NUM]->alert_event(); 				\
			if (CALL_OLD)												\
				IVTEntry::iv_table[ENTRY_NUM]->old_routine(); 			\
			/*if (!IVTEntry::is_locked())*/ 							\
				dispatch();												\
			/*IVTEntry::unlock();*/										\
	}

// For safer use, uncomment locks (fast typing prevention, prevents typing two keys at almost same time)

#define IVT_SIZE 256

typedef void interrupt (*Intr_Routine)(...);
typedef unsigned char IVTNo;


class KernelEv;

class IVTEntry {
private:
	KernelEv * krnl_event;
	IVTNo entry_num;

	Intr_Routine new_routine;
public:
	IVTEntry(IVTNo entry_num, Intr_Routine intr_routine);

	void alert_event();
	Intr_Routine old_routine;

	static IVTEntry* iv_table[IVT_SIZE];

	friend class KernelEv;
};

#endif
