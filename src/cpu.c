#ifndef _CPU_C_
#define _CPU_C_

#include "cpu.h"

void cpu_fill_vendorstring(unsigned int registers[4]){
    cpu_max_feats=registers[0];               /* EAX */
    cpu_vendorstring[0]=registers[1];
    cpu_vendorstring[1]=registers[1] >> 8;
    cpu_vendorstring[2]=registers[1] >> 16;   /* this feels wrong, but justified    */
    cpu_vendorstring[3]=registers[1] >> 24;   /* because the order is EBX, EDX, ECX */
    cpu_vendorstring[4]=registers[3];
    cpu_vendorstring[5]=registers[3] >> 8;
    cpu_vendorstring[6]=registers[3] >> 16;  /* <---wth Intel? --+ */
    cpu_vendorstring[7]=registers[3] >> 24;  /*                  | */
    cpu_vendorstring[8]=registers[2];        /* <---srsly?------ + */
    cpu_vendorstring[9]=registers[2] >> 8;
    cpu_vendorstring[10]=registers[2] >> 16;
    cpu_vendorstring[11]=registers[2] >> 24;
    cpu_vendorstring[12]='\0';    
}

/* If this can be made into one function with a macro   */
/* free candy will be passed out.  No switch statements */
void cpu_reg_read_cr0(){
    asm volatile("mov %%cr0, %0": "=r"(cpu_cur_regs.cr[0]));
}
void cpu_reg_read_cr1(){
    asm volatile("mov %%cr1, %0": "=r"(cpu_cur_regs.cr[1]));
}
void cpu_reg_read_cr2(){
    asm volatile("mov %%cr2, %0": "=r"(cpu_cur_regs.cr[2]));
}
void cpu_reg_read_cr3(){
    asm volatile("mov %%cr3, %0": "=r"(cpu_cur_regs.cr[3]));
}
void cpu_reg_read_cr4(){
    asm volatile("mov %%cr4, %0": "=r"(cpu_cur_regs.cr[4]));
}

void cpu_reg_read_eflags(){
    asm volatile("pushf\n\t"
                 "pop %0\n\t"
                : "=r"(cpu_cur_regs.eflags));
}

void cpu_reg_write_cr0(){
    asm volatile("mov %0, %%cr0":: "r"(cpu_cur_regs.cr[0]));
}
void cpu_reg_write_cr1(){
    asm volatile("mov %0, %%cr1":: "r"(cpu_cur_regs.cr[1]));
}
void cpu_reg_write_cr2(){
    asm volatile("mov %0, %%cr2":: "r"(cpu_cur_regs.cr[2]));
}
void cpu_reg_write_cr3(){
    asm volatile("mov %0, %%cr3":: "r"(cpu_cur_regs.cr[3]));
}
void cpu_reg_write_cr4(){
    asm volatile("mov %0, %%cr4":: "r"(cpu_cur_regs.cr[4]));
}


#endif
