#ifndef _CPU_H_
#define _CPU_H_

/* Ref:  "The following code is made Public Domain out of Clicker's x86/cpu.h"
   From: http://wiki.osdev.org/CPUID
*/

/* macros */
#define CPU_ENABLE_BIT_ON_CR(bit, crn) cpu_cur_regs.cr[crn] |= bit;
#define CPU_DISABLE_BIT_ON_CR(bit,crn) cpu_cur_regs.cr[crn] &= ~bit;
/* constants */
enum {
    CPUID_FEAT_ECX_SSE3         = 1 << 0, 
    CPUID_FEAT_ECX_PCLMUL       = 1 << 1,
    CPUID_FEAT_ECX_DTES64       = 1 << 2,
    CPUID_FEAT_ECX_MONITOR      = 1 << 3,  
    CPUID_FEAT_ECX_DS_CPL       = 1 << 4,  
    CPUID_FEAT_ECX_VMX          = 1 << 5,  
    CPUID_FEAT_ECX_SMX          = 1 << 6,  
    CPUID_FEAT_ECX_EST          = 1 << 7,  
    CPUID_FEAT_ECX_TM2          = 1 << 8,  
    CPUID_FEAT_ECX_SSSE3        = 1 << 9,  
    CPUID_FEAT_ECX_CID          = 1 << 10,
    CPUID_FEAT_ECX_FMA          = 1 << 12,
    CPUID_FEAT_ECX_CX16         = 1 << 13, 
    CPUID_FEAT_ECX_ETPRD        = 1 << 14, 
    CPUID_FEAT_ECX_PDCM         = 1 << 15, 
    CPUID_FEAT_ECX_DCA          = 1 << 18, 
    CPUID_FEAT_ECX_SSE4_1       = 1 << 19, 
    CPUID_FEAT_ECX_SSE4_2       = 1 << 20, 
    CPUID_FEAT_ECX_x2APIC       = 1 << 21, 
    CPUID_FEAT_ECX_MOVBE        = 1 << 22, 
    CPUID_FEAT_ECX_POPCNT       = 1 << 23, 
    CPUID_FEAT_ECX_XSAVE        = 1 << 26, 
    CPUID_FEAT_ECX_OSXSAVE      = 1 << 27, 
    CPUID_FEAT_ECX_AVX          = 1 << 28,
 
    CPUID_FEAT_EDX_FPU          = 1 << 0,  
    CPUID_FEAT_EDX_VME          = 1 << 1,  
    CPUID_FEAT_EDX_DE           = 1 << 2,  
    CPUID_FEAT_EDX_PSE          = 1 << 3,  
    CPUID_FEAT_EDX_TSC          = 1 << 4,  
    CPUID_FEAT_EDX_MSR          = 1 << 5,  
    CPUID_FEAT_EDX_PAE          = 1 << 6,  
    CPUID_FEAT_EDX_MCE          = 1 << 7,  
    CPUID_FEAT_EDX_CX8          = 1 << 8,  
    CPUID_FEAT_EDX_APIC         = 1 << 9,  
    CPUID_FEAT_EDX_SEP          = 1 << 11, 
    CPUID_FEAT_EDX_MTRR         = 1 << 12, 
    CPUID_FEAT_EDX_PGE          = 1 << 13, 
    CPUID_FEAT_EDX_MCA          = 1 << 14, 
    CPUID_FEAT_EDX_CMOV         = 1 << 15, 
    CPUID_FEAT_EDX_PAT          = 1 << 16, 
    CPUID_FEAT_EDX_PSE36        = 1 << 17, 
    CPUID_FEAT_EDX_PSN          = 1 << 18, 
    CPUID_FEAT_EDX_CLF          = 1 << 19, 
    CPUID_FEAT_EDX_DTES         = 1 << 21, 
    CPUID_FEAT_EDX_ACPI         = 1 << 22, 
    CPUID_FEAT_EDX_MMX          = 1 << 23, 
    CPUID_FEAT_EDX_FXSR         = 1 << 24, 
    CPUID_FEAT_EDX_SSE          = 1 << 25, 
    CPUID_FEAT_EDX_SSE2         = 1 << 26, 
    CPUID_FEAT_EDX_SS           = 1 << 27, 
    CPUID_FEAT_EDX_HTT          = 1 << 28, 
    CPUID_FEAT_EDX_TM1          = 1 << 29, 
    CPUID_FEAT_EDX_IA64         = 1 << 30,
    CPUID_FEAT_EDX_PBE          = 1 << 31
};

enum cpuid_requests {
  CPUID_GETVENDORSTRING,
  CPUID_GETFEATURES,
  CPUID_GETTLB,
  CPUID_GETSERIAL,
 
  CPUID_INTELEXTENDED=0x80000000,
  CPUID_INTELFEATURES,
  CPUID_INTELBRANDSTRING,
  CPUID_INTELBRANDSTRINGMORE,
  CPUID_INTELBRANDSTRINGEND,
};
 
enum cpu_cr0 {
    CPU_CR0_PG=0x80000000,  /* Paging  If 1, enable paging and use the CR3 register, else disable paging */
    CPU_CR0_CD=0x40000000,  /* Cache disable */
    CPU_CR0_NW=0x20000000,  /* Not-write through */  
    CPU_CR0_AM=0x40000,     /* Alignment mask  Alignment check enabled if AM set, AC flag set (in EFLAGS register, and privilege level is 3) */
    CPU_CR0_WP=0x10000,     /* Write protect */
    CPU_CR0_NE=0x20,        /* Numeric error   Enable internal x87 floating point error reporting when set, else enables PC style x87 error detection */
    CPU_CR0_ET=0x10,        /* Extension type */
    CPU_CR0_TS=0x8,         /* Task switched   Allows saving x87 task context only after x87 instruction used after task switch */
    CPU_CR0_EM=0x4,         /* Emulation   If set, no x87 floating point unit present, if clear, x87 FPU present */
    CPU_CR0_MP=0x2,         /* Monitor co-processor    Controls interaction of WAIT/FWAIT instructions with TS flag in CR0 */
    CPU_CR0_PE=0x1          /* Protected Mode Enable   If 1, system is in protected mode, else system is in real mode */
};

//enum cpu_cr4 {
/* 
14  SMXE    SMX Enable  
13  VMXE    VMX Enable  
10  OSXMMEXCPT  Operating System Support for Unmasked SIMD Floating-Point Exceptions    enable unmasked SSE exceptions
9   OSFXSR  Operating system support for FXSAVE and FXSTOR instructions     Must be set to use SSE instructions, enable fast FPU save and restore
8   PCE     Performance-Monitoring Counter enable   If set, RDPMC can be executed at any privilege level, else it can only be used in kernel mode (ring 0)
7   PGE     Page Global Enabled     
6   MCE     Machine Check Exception     Enables machine check interrupts to occur if set
5   PAE     Physical Address Extension  Changes page table layout to translate 32-bit virtual address into extended 36-bit physical address
4   PSE     Page Size Extensions    If unset page size is 4 KiB, else it is increased to 4 MiB, or with PAE, 2 MiB
3   DE  Debugging Extensions    
2   TSD     Time Stamp Disable  If set, RDTSC instruction can only be executed when in ring 0, otherwise RDTSC can be used at any privilege level
1   PVI     Protected Mode Virtual Interrupts   Enables hardware support of virtual interrupt flag (VIF) in protected mode
0   VME     Virtual 8086 mode extensions    Like the virtual interrupt flag; enabled if set
*/
//};

/* structures */
struct cpu_registers {
    unsigned long long gdt;
    unsigned long eflags;
    unsigned long cr[5];
};

/* variables */
unsigned int cpu_max_feats;
struct cpu_registers cpu_cur_regs;
unsigned char cpu_vendorstring[13];

/* functions */

//static inline void cpuid(int code, unsigned int *a, unsigned int  *d); 
//static inline int cpuid_string(int code, unsigned int where[4]);
/** issue a single request to CPUID. Fits 'intel features', for instance
 *  note that even if only "eax" and "edx" are of interrest, other registers
 *  will be modified by the operation, so we need to tell the compiler about it.
 */
static inline void cpuid(int code, unsigned int *a, unsigned int  *d) {
    asm volatile("cpuid"
                  :"=a"(*a),"=d"(*d)
                  :"0"(code)/* use EAX as input */
                  :"ecx","ebx");
}
 
/** issue a complete request, storing general registers output as a string
 */
static inline int cpuid_string(int code, unsigned int where[4]) {
    int highest = 0; /* unused? SB */
    asm volatile("cpuid"
                  :"=a"(*where),"=b"(*(where+1)),"=c"(*(where+2)),"=d"(*(where+3))
                  :"0"(code));
    return highest;
}

void cpu_fill_vendorstring(unsigned int registers[4]);
void cpu_reg_read_cr0();
void cpu_reg_read_cr1();
void cpu_reg_read_cr2();
void cpu_reg_read_cr3();
void cpu_reg_read_cr4();
void cpu_reg_read_eflags();
void cpu_reg_write_cr0();
void cpu_reg_write_cr1();
void cpu_reg_write_cr2();
void cpu_reg_write_cr3();
void cpu_reg_write_cr4();
#endif
