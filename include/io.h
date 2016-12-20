#ifndef _IO_H_
#define _IO_H_
#include "types.h"
/* constants */

/* variables */

/* functions */
static __inline__ void outb(unsigned short port, unsigned char val)
{
    asm volatile("outb %0,%1"::"a"(val), "Nd" (port));
}

static __inline__ unsigned char inb(unsigned short port)
{
    unsigned char ret;
    asm volatile ("inb %1,%0":"=a"(ret):"Nd"(port));
    return ret;
}

static __inline__ void outl(uint16 port, uint32 val)
{
    asm volatile("outl %0,%1"::"a"(val), "Nd" (port));
}

static __inline__ uint32 inl(uint16 port)
{
    uint32 ret;
    asm volatile ("inl %1,%0":"=a"(ret):"Nd"(port));
    return ret;
}
#endif
