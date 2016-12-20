#ifndef _TYPES_H_
#define _TYPES_H_

#ifdef __i386
    typedef unsigned       char uint8;
    typedef unsigned      short uint16;
    typedef unsigned        int uint32;
    typedef unsigned  long long uint64;

    typedef unsigned       long ureg;
    typedef unsigned       long uptr;

    typedef   signed       char sint8;
    typedef   signed      short sint16;
    typedef   signed        int sint32;
    typedef   signed  long long sint64;

    typedef   signed       long sreg;

    /* Make more of these? (8, 16, 64) */
    typedef struct {
        uint32 counter;
    } atomic32_t;

    typedef unsigned      long size_t;
#endif

#ifdef __x86_64
    typedef unsigned       char uint8;
    typedef unsigned      short uint16;
    typedef unsigned        int uint32;
    typedef unsigned       long uint64;

    typedef unsigned       long ureg;
    typedef unsigned       long uptr;

    typedef   signed       char sint8;
    typedef   signed      short sint16;
    typedef   signed        int sint32;
    typedef   signed       long sint64;

    typedef   signed       long sreg;

    typedef struct {
        uint32 counter;
    } atomic32_t;

    typedef unsigned      long size_t;
#endif

#endif /* _TYPES_H_ */
