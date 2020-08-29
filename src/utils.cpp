#include "utils.h"

volatile unsigned switch_context_code = NIL;			// Initial values
volatile unsigned lock_flag = SWITCH_CONTEXT_ALLOWED;
volatile unsigned process_killed_flag = 0U;
