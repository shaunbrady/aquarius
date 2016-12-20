#ifndef _ATOMIC_H_
#define _ATOMIC_H_

/* Borrowed and modified from Linux ~3.12
 arch/x86/include/asm/atomic.h 11Jan2014 SB */

#include "types.h"

/* macros */
#define ATOMIC_INIT(i)  { (i) }

/* constants */
/* structures */
/* variables */

/* functions */
static inline int atomic_read(const atomic32_t *v)
{
        return (*(volatile int *)&(v)->counter);
}

static inline void atomic_set(atomic32_t *v, int i)
{
    v->counter = i;
}

static inline void atomic_add(uint32 i, atomic32_t *v)
{
    asm volatile("lock;"
                 "addl %1,%0;"
                 : "+m" (v->counter)
                 : "ir" (i));
}

static inline void atomic_sub(uint32 i, atomic32_t *v)
{
    asm volatile("lock;"
                 "subl %1,%0;"
                 : "+m" (v->counter)
                 : "ir" (i));
}

#endif /* _ATOMIC_H_ */

