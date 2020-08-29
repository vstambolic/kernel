#ifndef TIMER_H_
#define TIMER_H_


typedef void interrupt (*intr_routine)(...);

void interrupt timer(...);

void initialize_ivt();
void restore_ivt();


#endif
