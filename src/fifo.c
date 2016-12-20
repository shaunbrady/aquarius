
#include "fifo.h"

uint32 fifo_init(fifo_t *f, void *buf, uint32 size){
    /*f->buffer = f->start = f->end = buf;*/
    f->buffer = buf;
    f->start = f->end = 0;
    f->size = size;
    atomic_set(&f->available, 0); /* ATOMIC_INIT fails here... why? */
    return 0;
}
/*typedef struct {
    uptr *start;
    uptr *end;
    uptr *buffer;
    atomic32_t available;
    uint32 size;
} fifo_t;*/

uint32 fifo_pop(fifo_t *f, void *drop, uint32 size){
    if(atomic_read(&f->available) == 0){ /* We are emtpy */
        return 0;
    }
    uint32 bytes_copied = 0;
    if(f->start <= f->end){ /* "contiguous" mode */
        uint32 delta = f->end - f->start;
        if(size <= delta){ /* we fit */
            memcpy(drop, f->buffer + f->start, size);
            bytes_copied = size;
            atomic_sub(size, &f->available);
            f->start += size;
        }
        else{/* size too large, copy as much as possible */
            memcpy(drop, f->buffer + f->start, delta);
            bytes_copied = delta;
            atomic_sub(delta, &f->available);
            f->start += delta;
        }
    }
    else{ /* "wrapping" mode */
        /* Use up the back */
        uint32 back_delta = f->size - f->start;
        if (back_delta > 0){
            if( size <= back_delta ){ /* We fit */
                /*printf("Got here.\n");
                printf("fifo.c: f->buffer: %x\n",f->buffer);
                printf("fifo.c: f->end: %x\n",f->end);
                printf("fifo.c: f->buffer + f->end: %x\n",f->buffer + f->end);*/
                memcpy(drop, f->buffer + f->start, size);
                f->start += size;
                bytes_copied = size;
                atomic_sub(size, &f->available);
                size = 0; /* All gone */
            }
            else{ /* size too large, copy as much as possible */
                memcpy(drop, f->buffer + f->start, back_delta);
                f->start += 0; /* wrapped */
                bytes_copied = back_delta;
                atomic_sub(back_delta, &f->available);
                size -= back_delta;
            }
        } 
        /* Put what's left on the front */
        if( size > 0){
            uint32 front_delta = f->end;
            if( size <= front_delta ) { /* We fit */
                memcpy(drop + back_delta, f->buffer + f->start, size);
                f->start = size;
                bytes_copied += size;
                atomic_sub(size, &f->available);
                /* size = 0; */ /* Need? All gone */
            }
            else{ /* size too large, copy as much as possible */
                memcpy(drop + back_delta, f->buffer + f->start, front_delta);
                f->start = front_delta; /* wrapped */
                bytes_copied += front_delta;
                atomic_sub(front_delta, &f->available);
            }
        }
    }
    return bytes_copied;
}

uint32 fifo_pop_all(fifo_t *f, void *drop){
    return 0;
}

uint32 fifo_push(fifo_t *f, void *pickup, uint32 size){
    if(atomic_read(&f->available) == f->size){ /* We are full */
        printf("fifo full\n");
        return 0;
    }
    uint32 bytes_copied;
    if(f->start <= f->end){ /* "wrapping" mode */
        /* Use up the back */
        uint32 back_delta = f->size - f->end;
        if (back_delta > 0){
            if( size <= back_delta ){ /* We fit */
                printf("push,wrapping,back,fit\n");
                /*printf("Got here.\n");
                printf("fifo.c: f->buffer: %x\n",f->buffer);
                printf("fifo.c: f->end: %x\n",f->end);
                printf("fifo.c: f->buffer + f->end: %x\n",f->buffer + f->end);*/
                memcpy(f->buffer + f->end, pickup, size);
                f->end += size;
                /* hack */
                /*if(f->end == f->size){
                    f->end = 0;
                }*/
                bytes_copied = size;
                atomic_add(size, &f->available);
                size = 0; /* All gone */
            }
            else{ /* size too large, copy as much as possible */
                printf("push,wrapping,back,too_large\n");
                memcpy(f->buffer + f->end, pickup, back_delta);
                f->end += 0; /* wrapped */
                bytes_copied = back_delta;
                atomic_add(back_delta, &f->available);
                size -= back_delta;
            }
        } 
        /* Put what's left on the front */
        if( size > 0){
            uint32 front_delta = f->start;
            if( size <= front_delta ) { /* We fit */
                printf("push,wrapping,front,fit\n");
                printf("pickup + back_delta: %s\n",pickup + back_delta);
                printf("f->buffer + f->end: %s\n",f->buffer + f->end);
                memcpy(f->buffer + f->end, pickup + back_delta, size);
                f->end = size;
                bytes_copied += size;
                atomic_add(size, &f->available);
                /* size = 0; */ /* Need? All gone */
            }
            else{ /* size too large, copy as much as possible */
                printf("push,wrapping,front,too_large\n");
                memcpy(f->buffer + f->end, pickup + back_delta, front_delta);
                f->end = front_delta; /* wrapped */
                bytes_copied += front_delta;
                atomic_add(front_delta, &f->available);
            }
        }
    }
    else{ /* "contiguous" mode */
        uint32 delta = f->start - f->end;
        if( size <= delta ){ /* We fit */
            printf("push,contiguous,fit\n");
            memcpy(f->buffer + f->end, pickup, size);
            bytes_copied = size;
            atomic_add(size, &f->available);
            f->end += size;
        }
        else{ /* size too large, copy as much as possible */
            printf("push,contiguous,too_large\n");
            memcpy(f->buffer + f->end, pickup, delta);
            bytes_copied = delta;
            atomic_add(delta, &f->available);
            f->end += delta;
        }
    }
    uint32 i;
    for(i = 0; i < 10; i ++){
        printf("|%c",f->buffer[i]);
    }
    printf("|%x|%x|%x\n",f->start,f->end,atomic_read(&f->available));
    return bytes_copied;
}
