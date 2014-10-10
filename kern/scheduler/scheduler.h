#ifndef SCHEDULER_H
#define SCHEDULER_H

#include "types.h"
#include "scheduler_types.h"

#define	TCB_START	0xC4000000
#define	TCB_END		0xCFFFFFFF

/**
 * Initialises the scheduler. This sets up several required data structures and
 * memory segments, and sets up the scheduler to be ready to begin executing.
 */
void scheduler_init(void);

/**
 * Creates a new process structure. This will have one thread created for it,
 * but with no information populated.
 */
scheduler_pcb_t *scheduler_new_process(void);

/**
 * Creates a new thread structure. If @process is not NULL, the thread is added
 * to the linked list of threads for the given process.
 */
scheduler_tcb_t *scheduler_new_tcb(scheduler_pcb_t *process);

#endif