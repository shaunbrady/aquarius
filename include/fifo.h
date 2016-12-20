#ifndef _FIFO_H_
#define _FIFO_H_

#include "types.h"
#include "atomic.h"
#include "string.h"
#include "screen.h"

/* macros */
/* constants */
/* structures */
typedef struct {
    uint32 start;
    uint32 end;
    uint8 *buffer;
    atomic32_t available; 
    uint32 size;
} fifo_t;

/* variables */
/* functions */

uint32 fifo_init(fifo_t *f, void *buf, uint32 size);

uint32 fifo_pop(fifo_t *f, void *drop, uint32 size);
uint32 fifo_pop_all(fifo_t *f, void *drop);
uint32 fifo_push(fifo_t *f, void *pickup, uint32 size);
#endif /* _FIFO_H_ */
