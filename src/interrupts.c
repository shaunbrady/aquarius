#ifndef _INTERRUPTS_C_
#define _INTERRUPTS_C_


#include "interrupts.h"
#include "io.h"
#include "screen.h"
#include "keyboard.h"

/*removing static removes a warning.  Will check later to see */
/*if this breaks something.  This function needs special      */
/*treatment because it happens in an ISR (usually)            */
/*static __inline__ void interrupts_send_eoi(unsigned long no)*/
__inline__ void interrupts_send_eoi(unsigned long no)
{
    /*             Val, Port */
    if (no>7){
        outb(0x20,0xa0);
    }
    outb(0x20,0x20);
}

void interrupts_handler(unsigned long int_num, unsigned long edi, unsigned long esi, unsigned long ebp, unsigned long esp,
                                               unsigned long edx, unsigned long ecx, unsigned long ebx, unsigned long eax){
    /* do something */
    switch(int_num){
        case 8:
            break;
        case 9:
            keyboard_handler();
            break;
        default:
            printf("Interrupt %x happened!  AX was %x\n",int_num,eax);
            printf("Interrupt %x happened!  BX was %x\n",int_num,ebx);
            printf("Interrupt %x happened!  CX was %x\n",int_num,ecx);
            printf("Interrupt %x happened!  DX was %x\n",int_num,edx);
    }
    interrupts_send_eoi(int_num);
}

void interrupts_init_idt(unsigned long code_seg){
    unsigned int extern _isr_wrapper_0, _isr_wrapper_1;
    //unsigned short offset_1in,offset_2in;
    unsigned short code_seg_shrt=code_seg & 0xFFFF;
    int isrsize=(unsigned int)&_isr_wrapper_1 - (unsigned int)&_isr_wrapper_0;
    //printf("&_isr_wrapper_1 is %x and &_isr_wrapper_0 is %x nd isrsize is %x\n",&_isr_wrapper_1,&_isr_wrapper_0,isrsize);
    int i;
    //printf("offset is %x and &_isr_wrapper is %x\n",offset,&_isr_wrapper);
    for(i=0;i<256;i++){
        //if(i<3){
        //    int xoffset=(unsigned int)&_isr_wrapper_0 + (i * isrsize); /* how many ISR blocks away from 0 */
        //    printf("xoffset is %x and i is %x\n",xoffset,i);
        //}
        unsigned int offset=(unsigned int)&_isr_wrapper_0 + (i * isrsize); /* how many ISR blocks away from 0 */
        interrupts_idt[i].offset_1=offset & 0xFFFF;  /* 0-15 */ ;
        interrupts_idt[i].offset_2=offset >> 16; /* 16-31 */ ;
        interrupts_idt[i].selector=code_seg_shrt;
        //interrupts_idt[i].type_attr=0x8E;
        interrupts_idt[i].type_attr=0x8E00;/* FIXME, Endian-error */
        /*printf("interrupts_idt[%d].offset_1 is %x\n",i,interrupts_idt[i].offset_1);
        printf("   interrupts_idt[%d].offset_2 is %x\n",i,interrupts_idt[i].offset_2);
        printf("      interrupts_idt[%d].selector is %x\n",i,interrupts_idt[i].selector);
        printf("         interrupts_idt[%d].type_attr is %x\n",i,interrupts_idt[i].type_attr);
        int j;
        for(j=0;j<2000000000;j++){}*/
        /* ( P=1, DPL=00b, S=0, type=1110b => type_attr=1000_1110b=0x8E) */
    }
    interrupts_lidt_data.size=0xFF;  /* size -1 */
    interrupts_lidt_data.offset=(unsigned int)interrupts_idt;
    INTERRUPTS_LIDT(interrupts_lidt_data)
}

void interrupts_enable(){
    /* Enable Interrupts */
    asm volatile("sti");      
}



#endif
