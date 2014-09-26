#ifndef PLATFORM_X86_MSR
#define PLATFORM_X86_MSR

///////////////////////////////// Intel MSRs //////////////////////////////////
#define	MSR_IA32_SYSENTER_CS	0x174 // base selector for CS/SS
#define	MSR_IA32_SYSENTER_ESP	0x175 // %esp for sysenter
#define	MSR_IA32_SYSENTER_EIP	0x176 // %eip for sysenter

////////////////////////////////// AMD MSRs ///////////////////////////////////
#define	MSR_AMD_SYSCALL_STAR	0xC0000081 // syscall %eip in low 32 bits, CS/SS for high 32
#define	MSR_AMD_SYSCALL_LSTAR	0xC0000082 // %rip for 64 bit caller (ignore)
#define	MSR_AMD_SYSCALL_CSTAR	0xC0000083 // %rip for 32 bit caller (ignore)
#define	MSR_AMD_SYSCALL_CFMASK	0xC0000084 // low 32 bits are mask for rFLAGS

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