#ifndef UTILS_H_
#define UTILS_H_

#define nullptr 0L


//------------------- Interrupts, lock / unlock
#define SWITCH_CONTEXT_ALLOWED 0
#define SWITCH_CONTEXT_NOT_ALLOWED 1


#define LOCK { 								                           			  				\
			lock_flag = SWITCH_CONTEXT_ALLOWED;       												       				\
		}
#define UNLOCK {                  			 												    \
			lock_flag = SWITCH_CONTEXT_NOT_ALLOWED;												\
			if (switch_context_code == AFTER_INTR)  											\
				dispatch();             							      		    			\
		}

extern volatile unsigned lock_flag;

#define DISABLE_INTERRUPTS asm {																\
								cli 															\
						   }
#define ENABLE_INTERRUPTS asm {																	\
								sti 															\
						  }

//------------------- Context switch code
#define NIL 0
#define DEMANDED 1
#define AFTER_INTR 2

extern volatile unsigned switch_context_code;

//------------------- Suspension recover code
#define TIME_EXPIRED 0
#define BEFORE_TIME 1

//------------------- Thread killed while handling signals
extern volatile unsigned process_killed_flag;

void dispatch();

#endif
