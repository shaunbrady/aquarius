#ifndef _INTERRUPTS_H_
#define _INTERRUPTS_H_

/* macros */
#define INTERRUPTS_LIDT(x) asm volatile("lidt %0":: "m"(x));

/* constants */

/* structures */
struct idt_entry{
    unsigned short offset_1;  /* offset bits 0..15 */
    unsigned short selector;  /* a code segment selector in GDT or LDT */
    unsigned short type_attr; /* 8 bits reserved (0), type and attributes */
    unsigned short offset_2;  /* offset bits 16..31 */
} ;

struct lidt_struct{
    unsigned short size; /* -1 */
    unsigned int   offset;
}__attribute((packed));  /* GCCASSUMED */

/* variables */
struct idt_entry interrupts_idt[256];
struct lidt_struct interrupts_lidt_data; /* Only needed once, but if in a local function */
                                         /* could disappear before lidt completes        */
/* functions */
/* void interrupts_handler(unsigned long int_num, unsigned long ax); */
void interrupts_handler(unsigned long int_num, unsigned long edi, unsigned long esi, unsigned long ebp, unsigned long esp,
                                               unsigned long edx, unsigned long ecx, unsigned long ebx, unsigned long eax);
void interrupts_init_idt(unsigned long code_seg);
/*static __inline__ void interrupts_send_eoi(unsigned long no);*/
__inline__ void interrupts_send_eoi(unsigned long no);
void interrupts_enable();
#endif
