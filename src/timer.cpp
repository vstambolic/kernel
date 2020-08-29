#include "timer.h"

#include <dos.h>

#include "pcb.h"
#include "utils.h"
#include "SCHEDULE.H"
#include "krnl_sem.h"

#define TIMER_ENTRY 0x8

intr_routine old_timer;

volatile Time time_cnt = defaultTimeSlice;
volatile unsigned tmp_ss;
volatile unsigned tmp_sp;
volatile unsigned tmp_bp;

extern void tick();

// at the beginning implicitly masks all the other interrupts,
// and pushes all registers on stack (at the end implicitly does the opposite)
void interrupt timer(...) {
	if (switch_context_code != DEMANDED) {
		old_timer();				// equivalent to (*old_timer)()
		tick();
		time_cnt--;
		KernelSem::alert_semaphores();
	}
	// If time's up for running thread, switch context
	if ((PCB::running->time != 0 && time_cnt == 0) || switch_context_code != NIL) {
		if (lock_flag == SWITCH_CONTEXT_ALLOWED || switch_context_code == DEMANDED) {
			asm {
				mov tmp_ss, ss
				mov tmp_sp, sp
			    mov tmp_bp, bp
			}
			PCB::running->ss = tmp_ss;
			PCB::running->sp = tmp_sp;
			PCB::running->bp = tmp_bp;

			if (PCB::running->status == PCB::RUNNING) {
				PCB::running->status = PCB::READY;
				Scheduler::put((PCB*)(PCB::running));
			}
			do {
				PCB::running = Scheduler::get();
				process_killed_flag = 0;
				if (PCB::running == nullptr)
					PCB::running = PCB::idle;
				else
					PCB::running->status = PCB::RUNNING;

				tmp_ss = PCB::running->ss;
				tmp_sp = PCB::running->sp;
				tmp_bp = PCB::running->bp;

				asm {
					mov ss, tmp_ss
					mov sp, tmp_sp
					mov bp, tmp_bp
				}

				time_cnt = PCB::running->time;
				LOCK
				PCB::running->handle_signals();
				UNLOCK

			} while (process_killed_flag == 1);


			switch_context_code = NIL;
		}
		else
			switch_context_code = AFTER_INTR;		// Critical section in progress, change context when can
	}
}



#ifndef BCC_BLOCK_IGNORE
void initialize_ivt() {
	LOCK
		old_timer = getvect(TIMER_ENTRY);
		setvect(TIMER_ENTRY, timer);
	UNLOCK
}

void restore_ivt() {
	LOCK
		setvect(TIMER_ENTRY,old_timer);
	UNLOCK
}
#endif




