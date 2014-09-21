/*
 * Macros for producing x86-specific code to create read-modify-write cycles.
 */

#define __GEN_RMWcc(fullop, var, cc, ...)							\
do {																\
		char c;														\
		__asm__ volatile(fullop "; set" cc " %1"					\
						: "+m" (var), "=qm" (c)						\
						: __VA_ARGS__ : "memory");					\
		return c != 0;												\
} while (0)
 
#define GEN_UNARY_RMWcc(op, var, arg0, cc) __GEN_RMWcc(op " " arg0, var, cc)

#define GEN_BINARY_RMWcc(op, var, vcon, val, arg0, cc) __GEN_RMWcc(op " %2, " arg0, var, cc, vcon (val))

