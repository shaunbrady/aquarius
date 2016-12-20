#ifndef _KEYBOARD_H_
#define _KEYBOARD_H_

#include "types.h"
/* macros */
/* constants */
extern uint8 keyboard_single_keymaps[256];
/*#define CLI_BUFFER_SIZE 256*/
/* structures */
/* variables */
/*uint8 cli_buffer[CLI_BUFFER_SZ];*/ 
/* functions */
void keyboard_handler();
uint8 keyboard_trasnlate_scancode(uint8 code);



#endif
