#ifndef _CLI_C_
#define _CLI_C_

#include "cli.h"
#include "screen.h"
#include "cpu.h" /* remove */

uint8 cli_buffer_size = 0;

uint8 cli_add(uint8 achar){
    if(cli_buffer_size < CLI_BUFFER_MAX){
        cli_buffer[cli_buffer_size] = achar;
        cli_buffer[cli_buffer_size + 1] = 0; /* Not an overflow */
        cli_buffer_size++;
        return 0;
    }
    else{
        return -1;
    }
}

uint8 cli_clear(){
    cli_buffer_size = 0;
    cli_buffer[0] = 0;
    return 0;
}

uint8* cli_get(){
    return cli_buffer;
}

void cli_print(){
    printf("cli_print %s\n",cli_buffer);
}
#endif
