#include "scheduler.h"

#include "vm/kmalloc.h"
#include "vm/physical.h"

// this is the shared scheduler lock
static mutex_t scheduler_lock;

// current thread and process IDs
static scheduler_pid_t next_pid;
static scheduler_tid_t next_tid;

// A bitmap indicating which 4K blocks are available for structures.
static unsigned int* frames;
static unsigned int nframes;

// Macros used in the bitset algorithms.
#define INDEX_FROM_BIT(a) (a/(8*4))
#define OFFSET_FROM_BIT(a) (a%(8*4))

/**
 * Allocates a new process ID. This depends on the scheduler lock.
 */
static scheduler_pid_t scheduler_new_pid(void) {
	mutex_take_spin(&scheduler_lock);
	next_pid++;
	mutex_give(&scheduler_lock);

	return next_pid;
}

/**
 * Allocates a new thread ID. This depends on the scheduler lock.
 */
static scheduler_tid_t scheduler_new_tid(void) {
	mutex_take_spin(&scheduler_lock);
	next_tid++;
	mutex_give(&scheduler_lock);

	return next_tid;
}

/**
 * Set a bit in the frames bitset as used.
 *
 * @param frame Offset into the array
 */
static inline void set_frame(unsigned int frame) {
	unsigned int idx = INDEX_FROM_BIT(frame);
	unsigned int off = OFFSET_FROM_BIT(frame);
	frames[idx] |= (0x1 << off);
}

/**
 * Sets a bit in the frames bitset as unused.
 *
 * @param frame Offset in the array.
 */
/*static void clear_frame(unsigned int frame) {
	unsigned int idx = INDEX_FROM_BIT(frame);
	unsigned int off = OFFSET_FROM_BIT(frame);
	frames[idx] &= ~(0x1 << off);
}*/

/**
 * Find the first free frame that can be allocated for a structure.
 *
 * @return Offset into the bitset
 */
static unsigned int find_free_frame() {
	unsigned int i, j;
	for(i = 0; i < INDEX_FROM_BIT(nframes); i++) {
		if(frames[i] != 0xFFFFFFFF) { // nothing free, check next
			// At least one bit is free here
			for (j = 0; j < 32; j++) {
				unsigned int toTest = 0x1 << j;

				// If this frame is free, return
				if (!(frames[i] & toTest)) {
					return i*4*8+j;
				}
			}
		}
	}

	return -1;
}

/**
 * Initialises the scheduler. This sets up several required data structures and
 * memory segments, and sets up the scheduler to be ready to begin executing.
 */
void scheduler_init(void) {
	// Allocate frameset
	unsigned int nframes = (TCB_END + 1) - TCB_START;
	nframes /= 0x1000;

	KDEBUG("Scheduler supports %u threads max\n", nframes);

	frames = (unsigned int *) kmalloc(INDEX_FROM_BIT(nframes));
	memclr(frames, INDEX_FROM_BIT(nframes));
}

/**
 * Creates a new process structure. This will have one thread created for it,
 * but with no information populated.
 */
scheduler_pcb_t *scheduler_new_process(void) {
	unsigned int frame = find_free_frame();
	set_frame(frame);

	frame *= 0x1000;
	frame += TCB_START;

	// allocate the memory for this structure
	uintptr_t phys = vm_allocate_phys();
	platform_pm_map(platform_pm_get_kernel_table(), frame, phys, VM_FLAGS_KERNEL);

	// allocate the process struct here
	scheduler_pcb_t *pcb = (scheduler_pcb_t *) frame;
	pcb->process_id = scheduler_new_pid();

	// allocate a TCB
	scheduler_tcb_t *tcb = scheduler_new_tcb(pcb);

	// initialise process struct
	pcb->thread = tcb;

	// initialise thread struct

	return pcb;
}

/**
 * Creates a new thread structure. If @process is not NULL, the thread is added
 * to the linked list of threads for the given process.
 */
scheduler_tcb_t *scheduler_new_tcb(scheduler_pcb_t *process) {
	unsigned int frame = find_free_frame();
	set_frame(frame);

	frame *= 0x1000;
	frame += TCB_START;

	// allocate the memory for this structure
	uintptr_t phys = vm_allocate_phys();
	platform_pm_map(platform_pm_get_kernel_table(), frame, phys, VM_FLAGS_KERNEL);

	// allocate a thread struct
	scheduler_tcb_t *tcb = (scheduler_tcb_t *) frame;
	tcb->thread_id = scheduler_new_tid();

	// add it to the linked list of threads
	if(process) {
		scheduler_tcb_t *thread = process->thread;

		while(thread) {
			// next thread NULL?
			if(!thread->next) {
				thread->next = tcb;
				break;
			}

			// check the next thread
			thread = thread->next;
		}
	}

	return tcb;
}