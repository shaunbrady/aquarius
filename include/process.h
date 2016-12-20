#ifndef _PROCESS_H_
#define _PROCESS_H_

/* constants */

#define PROC_ARRAY_SIZE 65536 /* should match pid of process struct */

/* variables */

/* structures */
struct {
	unsigned short pid;
    unsigned short flags;
	*process next;
	unsigned int memory_start;//protected mode address
	unsigned int memory_len;//bytes
	unsigned int avg_memory_len;//bytes (for determining were to keep this in memory)
    *non_contig_block non_cont;
} process

struct {
    unsigned int memory_start;
    *non_contig_block next;
}

/* functions */

#endif
