#import <types.h>

/*
 * Functions for dealing with stack guards. This will cause the kernel to panic
 * if a stack frame becomes corrupted.
 */
// unsigned int __stack_chk_guard[4] = {0xDEADBEEF, 0xCAFEBABE, 0x80808080, 0xC001C0DE};
void *__stack_chk_guard = NULL;

void __stack_chk_guard_setup(void) {
	unsigned int *p = (unsigned int *) __stack_chk_guard;
	*p = rand_32();
	
	// KSUCCESS("Stack guards initialised");
}
 
void __attribute__((noreturn)) __stack_chk_fail() { 
	IRQ_OFF();

//	klog(kLogLevelCritical, "Kernel stack memory corruption detected!");

	uint32_t ebp;
	__asm__("mov %%ebp, %0" : "=r" (ebp));

	// Halt by going into an infinite loop.
	for(;;);
}