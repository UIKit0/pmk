#ifndef PLATFORM_X86_MSR
#define PLATFORM_X86_MSR

/**
 * Writes a model-specific register
 */
inline void msr_write(uint32_t msr_id, uint64_t msr_value) {
	__asm__ volatile("wrmsr" : : "c" (msr_id), "A" (msr_value));
}

/**
 * Reads a model-specific register
 */
inline uint64_t msr_read(uint32_t msr_id) {
	uint64_t msr_value;
	__asm__ volatile("rdmsr" : "=A" (msr_value) : "c" (msr_id));
	return msr_value;
}

#endif