/*
 * Macros for producing x86-specific atomic exchange operations.
 *
 * The general pattern here is that macros with a _local suffix are NOT locked,
 * and those with _sync or no suffix are.
 */
extern void __xchg_wrong_size(void);
extern void __cmpxchg_wrong_size(void);
extern void __xadd_wrong_size(void);
extern void __add_wrong_size(void);

// Size values
#define	__X86_CASE_B	1
#define	__X86_CASE_W	2
#define	__X86_CASE_L	4
#define	__X86_CASE_Q	-1

/* 
 * An exchange-type operation, which takes a value and a pointer, and
 * returns the old value.
 */
#define __xchg_op(ptr, arg, op, lock) ({							\
			__typeof__ (*(ptr)) __ret = (arg);						\
			switch(sizeof(*(ptr))) {								\
			 case __X86_CASE_B:										\
					 __asm__ volatile(lock #op "b %b0, %1\n"		\
									: "+q" (__ret), "+m" (*(ptr))	\
									: : "memory", "cc");			\
					 break;											\
			 case __X86_CASE_W:										\
					 __asm__ volatile(lock #op "w %w0, %1\n"		\
									: "+r" (__ret), "+m" (*(ptr))	\
									: : "memory", "cc");			\
					 break;											\
			 case __X86_CASE_L:										\
					 __asm__ volatile(lock #op "l %0, %1\n"		\
									: "+r" (__ret), "+m" (*(ptr))	\
									: : "memory", "cc");			\
					 break;											\
			 case __X86_CASE_Q:										\
					 __asm__ volatile(lock #op "q %q0, %1\n"		\
									: "+r" (__ret), "+m" (*(ptr))	\
									: : "memory", "cc");			\
					 break;											\
			 default:												\
					 __ ## op ## _wrong_size();						\
			}														\
			__ret;													\
	 })

// XCHG has an implied LOCK prefix
#define xchg(ptr, v)	__xchg_op((ptr), (v), xchg, "")

/*
 * Atomic compare and exchange. Compare OLD with MEM, if identical,
 * store NEW in MEM.  Return the initial value in MEM. Success is
 * indicated by comparing RETURN with OLD.
 */
#define __raw_cmpxchg(ptr, old, new, size, lock) ({					\
	 __typeof__(*(ptr)) __ret;										\
	 __typeof__(*(ptr)) __old = (old);								\
	 __typeof__(*(ptr)) __new = (new);								\
	switch(size) {													\
	 case __X86_CASE_B: {											\
			 volatile uint8_t *__ptr = (volatile uint8_t *)(ptr);	\
			 __asm__ volatile(lock "cmpxchgb %2,%1"					\
							: "=a" (__ret), "+m" (*__ptr)			\
							: "q" (__new), "" (__old)				\
							: "memory");							\
			 break;													\
	 }																\
	 case __X86_CASE_W: {											\
			 volatile uint16_t *__ptr = (volatile uint16_t *)(ptr);	\
			 __asm__ volatile(lock "cmpxchgw %2,%1"					\
							: "=a" (__ret), "+m" (*__ptr)			\
							: "r" (__new), "" (__old)				\
							: "memory");							\
			 break;													\
	 }																\
	 case __X86_CASE_L: {											\
			 volatile uint32_t *__ptr = (volatile uint32_t *)(ptr);	\
			 __asm__ volatile(lock "cmpxchgl %2,%1"					\
							: "=a" (__ret), "+m" (*__ptr)			\
							: "r" (__new), "" (__old)				\
							: "memory");							\
			 break;													\
	 }																\
	 case __X86_CASE_Q: {											\
			 volatile uint64_t *__ptr = (volatile uint64_t *)(ptr);	\
			 __asm__ volatile(lock "cmpxchgq %2,%1"					\
							: "=a" (__ret), "+m" (*__ptr)			\
							: "r" (__new), "" (__old)				\
							: "memory");							\
			 break;													\
	 }																\
	 default:														\
			 __cmpxchg_wrong_size();								\
	}																\
	__ret;															\
})

#define __cmpxchg(ptr, old, new, size) __raw_cmpxchg((ptr), (old), (new), (size), "lock")
#define __sync_cmpxchg(ptr, old, new, size)	__raw_cmpxchg((ptr), (old), (new), (size), "lock; ")
#define __cmpxchg_local(ptr, old, new, size) __raw_cmpxchg((ptr), (old), (new), (size), "")

#define cmpxchg(ptr, old, new) __cmpxchg(ptr, old, new, sizeof(*(ptr)))
#define sync_cmpxchg(ptr, old, new) __sync_cmpxchg(ptr, old, new, sizeof(*(ptr)))
#define cmpxchg_local(ptr, old, new) __cmpxchg_local(ptr, old, new, sizeof(*(ptr)))


/*
 * xadd() adds "inc" to "*ptr" and atomically returns the previous
 * value of "*ptr".
 */
#define __xadd(ptr, inc, lock) __xchg_op((ptr), (inc), xadd, lock)

#define xadd(ptr, inc) __xadd((ptr), (inc), "lock")
#define xadd_sync(ptr, inc) __xadd((ptr), (inc), "lock; ")
#define xadd_local(ptr, inc) __xadd((ptr), (inc), "")

#define __add(ptr, inc, lock) ({									\
			 __typeof__ (*(ptr)) __ret = (inc);						\
			switch(sizeof(*(ptr))) {							 	\
			 case __X86_CASE_B:										\
					 __asm__ volatile(lock "addb %b1, %0\n"			\
									: "+m" (*(ptr)) : "qi" (inc)	\
									: "memory", "cc");				\
					 break;											\
			 case __X86_CASE_W:										\
					 __asm__ volatile(lock "addw %w1, %0\n"			\
									: "+m" (*(ptr)) : "ri" (inc)	\
									: "memory", "cc");				\
					 break;											\
			 case __X86_CASE_L:										\
					 __asm__ volatile(lock "addl %1, %0\n"			\
									: "+m" (*(ptr)) : "ri" (inc)	\
									: "memory", "cc");				\
					 break;											\
			 case __X86_CASE_Q:										\
					 __asm__ volatile(lock "addq %1, %0\n"			\
									: "+m" (*(ptr)) : "ri" (inc)	\
									: "memory", "cc");				\
					 break;											\
			 default:												\
					 __add_wrong_size();							\
			}														\
			__ret;													\
	 })
