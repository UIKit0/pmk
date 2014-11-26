#ifndef SCHEDULER_SCHEDULER_TYPES
#define SCHEDULER_SCHEDULER_TYPES

// include for required types
#include "vm/vm.h"

/// Define aliases
typedef struct scheduler_pcb scheduler_pcb_t;
typedef struct scheduler_tcb scheduler_tcb_t;

/// type for process/thread ID
typedef unsigned int scheduler_pid_t;
typedef unsigned int scheduler_tid_t;

/// type for CPU time: nanoseconds
typedef uint64_t scheduler_cpu_time_t;

/**
 * Process Control Block (PCB) Structure
 *
 * This structure is allocated by the kernel to represent the state of specific
 * process. Processes serve simply as a means to collect several threads in a
 * central space. Think of it like a box containing several potatoes.
 *
 * 
 */
struct scheduler_pcb {
	scheduler_pid_t process_id;
	scheduler_pid_t parent_pid;

	unsigned int exit_status;

	// CPU uptime in nanoseconds
	scheduler_cpu_time_t time_created;
	scheduler_cpu_time_t time_exited;

	// pagetable for all threads in process
	platform_pagetable_t pagetable;

	// base address of .text section
	uintptr_t text_base;

	// time spent in kernel/user mode
	scheduler_cpu_time_t time_kernel;
	scheduler_cpu_time_t time_user;

	// singly linked list to threads
	scheduler_tcb_t *thread;
};

/**
 * Thread Control Block (TCB) Structure
 *
 * This structure is allocated by the kernel to represent the state of threads
 * on the system.
 */
struct scheduler_tcb {
	scheduler_tid_t thread_id;

	scheduler_cpu_time_t time_kernel;
	scheduler_cpu_time_t time_user;

	// singly linked list of threads: terminated by NULL
	scheduler_tcb_t *next;

	// kernel-mode stack
	uint8_t kernel_stack[256];

	// platform reserved use
	uint8_t platform[1024];
};

#endif