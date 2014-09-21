/*
 * Various synchronisation primitives, such as mutexes and semaphores.
 *
 * Also defined are atomic operations that perform one indivisible read, modify
 * write cycle.
 */
#include <types.h>

#include "rmc.h"
#include "xchg.h"

// An atomic integer structure thingie
typedef struct {
	int counter;
} atomic_t;

/*
 * Read atomic variable
 */
static inline int atomic_read(const atomic_t *v) {
	return (*(volatile int *) &(v)->counter);
}

/*
 * Set atomic variable
 */
static inline void atomic_set(atomic_t *v, int i) {
	v->counter = i;
}

/*
 * Add to atomic variable
 */
static inline void atomic_add(int i, atomic_t *v) {
	__asm__ volatile("lock addl %1,%0" : "+m" (v->counter) : "ir" (i));
}

/*
 * Subtract atomic variable
 */
static inline void atomic_sub(int i, atomic_t *v) {
	__asm__ volatile("lock subl %1,%0" : "+m" (v->counter) : "ir" (i));
}

/*
 * Subtract and test
 *
 * @return true if result is zero, false otherwise
 */
static inline int atomic_sub_and_test(int i, atomic_t *v) {
	GEN_BINARY_RMWcc("lock subl", v->counter, "er", i, "%0", "e");
}

/*
 * Increment atomic variable
 */
static inline void atomic_inc(atomic_t *v) {
	__asm__ volatile("lock incl %0" : "+m" (v->counter));
}

/*
 * Increment and test
 *
 * @return true if result is zero, false otherwise
 */
static inline int atomic_inc_and_test(atomic_t *v) {
	GEN_UNARY_RMWcc("lock incl", v->counter, "%0", "e");
}

/*
 * Decrement atomic variable
 */
static inline void atomic_dec(atomic_t *v) {
	__asm__ volatile("lock decl %0" : "+m" (v->counter));
}

/*
 * Decrement and test
 *
 * @return true if result is zero, false otherwise
 */
static inline int atomic_dec_and_test(atomic_t *v) {
	GEN_UNARY_RMWcc("lock decl", v->counter, "%0", "e");
}

/*
 * Add and test if negative

 * @return true if negative, false when greater than or equal to zero.
 */
static inline int atomic_add_negative(int i, atomic_t *v) {
	GEN_BINARY_RMWcc("lock addl", v->counter, "er", i, "%0", "s");
}

/*
 * Add to atomic variable, and return the result.
 */
static inline int atomic_add_return(int i, atomic_t *v) {
	return i + xadd(&v->counter, i);
}
#define atomic_inc_return(v)  (atomic_add_return(1, v))

/*
 * Subtract from atomic variable, and return the result.
 */
static inline int atomic_sub_return(int i, atomic_t *v) {
	return atomic_add_return(-i, v);
}
#define atomic_dec_return(v)  (atomic_sub_return(1, v))

/*
 * Atomically exchanges two values, comparing it against a third.
 *
 * @return true if they are the same, false otherwise.
 */
static inline int atomic_cmpxchg(atomic_t *v, int old, int new) {
	return cmpxchg(&v->counter, old, new);
}

/*
 * Exchange the value in the atomic value with another value.
 *
 * @return The old value
 */
static inline int atomic_xchg(atomic_t *v, int new) {
	return xchg(&v->counter, new);
}

/*
 * Adds to the atomic, unless it is already a specific value.
 *
 * Atomically adds @a to @v, so long as @v was not already @u.
 * @return Old value of the atomic
 */
static inline int atomic_add_unless(atomic_t *v, int addend, int u) {
        int c, old;
        c = atomic_read(v);
        for (;;) {
                if (unlikely(c == (u)))
                        break;
                old = atomic_cmpxchg((v), c, c + (addend));
                if (likely(old == c))
                        break;
                c = old;
        }
        return c;
}
