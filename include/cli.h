#ifndef _CLI_H_
#define _CLI_H_

#include "types.h"

/* macros */
#define CLI_BUFFER_MAX 78 /* 80 - 2 (prompt) */
/* constants */

/* structures */
/* variables */
unsigned char cli_buffer[CLI_BUFFER_MAX + 1]; /* + null */
extern uint8 cli_buffer_size;

/* functions */
uint8 cli_add(uint8 achar);
uint8 cli_clear();
uint8* cli_get();
void cli_print();

#endif
