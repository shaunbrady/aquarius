/* main.c - the C main body of the kernel */
/* Top of cmain is borrowed from GNU Multiboot howto */
/* Copyright (C) 2009  Shaun Brady */


#include "multiboot.h"
#include "memory.h"
#include "cpu.h"
#include "interrupts.h"
#include "screen.h"
#include "types.h"
#include "atomic.h"
#include "fifo.h"

/* Macros. */

/* Check if the bit BIT in FLAGS is set. */
#define CHECK_FLAG(flags,bit)   ((flags) & (1 << (bit)))

unsigned mod_jump;

/* Forward declarations. */
//void cmain (unsigned long magic, unsigned long addr);

/* Check if MAGIC is valid and print the Multiboot information structure
   pointed by ADDR. */
void
cmain (ureg magic, ureg addr)
{
    /* from boot.S */
    extern ureg _start, _end;
    extern ureg _edata, stack;

    multiboot_info_t *mbi;
    /* Clear the screen. */
    cls ();

    /* Am I booted by a Multiboot-compliant boot loader? */
    if (magic != MULTIBOOT_BOOTLOADER_MAGIC){
        printf ("Invalid magic number: 0x%x\n", (unsigned) magic);
        return;
    }

    /* Set MBI to the address of the Multiboot information structure. */
    mbi = (multiboot_info_t *) addr;

    /* Print out the flags. */
    printf ("flags = 0x%x\n", (unsigned) mbi->flags);

    /* Are mem_* valid? */
    if (CHECK_FLAG (mbi->flags, 0))
        printf ("mem_lower = %uKB, mem_upper = %uKB\n",
            (unsigned) mbi->mem_lower, (unsigned) mbi->mem_upper);

    /* Is boot_device valid? */
    if (CHECK_FLAG (mbi->flags, 1))
        printf ("boot_device = 0x%x\n", (unsigned) mbi->boot_device);

    /* Is the command line passed? */
    if (CHECK_FLAG (mbi->flags, 2))
        printf ("cmdline = %s\n", (char *) mbi->cmdline);

    /* Are mods_* valid? */
    if (CHECK_FLAG (mbi->flags, 3)){
        module_t *mod;
        int i;

        printf ("mods_count = %d, mods_addr = 0x%x\n",
                (int) mbi->mods_count, (int) mbi->mods_addr);
        mem_modules_start = (uptr*)0xFFFFFFFF;
        mem_modules_end = 0;
        for (i = 0, mod = (module_t *) mbi->mods_addr;
             i < mbi->mods_count;
             i++, mod++)
        {
            printf (" mod_start = 0x%x, mod_end = 0x%x, string = %s\n",
                    (unsigned) mod->mod_start,
                    (unsigned) mod->mod_end,
                    (char *) mod->string);
            mod_jump=mod->mod_start;
            /* get lower bound */
            if(mem_modules_start > (uptr*)mod->mod_start){
                mem_modules_start = (uptr*) mod->mod_start;
            }
            /* get upper bound */
            if(mem_modules_end < (uptr*)mod->mod_end){
                mem_modules_end = (uptr*)mod->mod_end; 
            }
        }
    }

    /* Bits 4 and 5 are mutually exclusive! */
    if (CHECK_FLAG (mbi->flags, 4) && CHECK_FLAG (mbi->flags, 5)){
        printf ("Both bits 4 and 5 are set.\n");
        return;
    }

    /* Is the symbol table of a.out valid? */
    if (CHECK_FLAG (mbi->flags, 4)){
        aout_symbol_table_t *aout_sym = &(mbi->u.aout_sym);

        printf ("aout_symbol_table: tabsize = 0x%0x, "
                "strsize = 0x%x, addr = 0x%x\n",
                (unsigned) aout_sym->tabsize,
                (unsigned) aout_sym->strsize,
                (unsigned) aout_sym->addr);
    }

    /* Is the section header table of ELF valid? */
    if (CHECK_FLAG (mbi->flags, 5)){
        elf_section_header_table_t *elf_sec = &(mbi->u.elf_sec);

        printf ("elf_sec: num = %u, size = 0x%x,"
                " addr = 0x%x, shndx = 0x%x\n",
                (unsigned) elf_sec->num, (unsigned) elf_sec->size,
                (unsigned) elf_sec->addr, (unsigned) elf_sec->shndx);
        mem_elf_start = (uptr*)elf_sec->addr;
    }

    /* Are mmap_* valid? */
    if (CHECK_FLAG (mbi->flags, 6)){
        memory_map_t *mmap;
        printf ("mmap_addr = 0x%x, mmap_length = 0x%x\n",
                (unsigned) mbi->mmap_addr, (unsigned) mbi->mmap_length);
        for (mmap = (memory_map_t *) mbi->mmap_addr;
             (unsigned long) mmap < mbi->mmap_addr + mbi->mmap_length;
             mmap = (memory_map_t *) ((unsigned long) mmap
                                    + mmap->size + sizeof (mmap->size))){
            printf (" size = 0x%x, base_addr = 0x%x%x,"
                    " length = 0x%x%x, type = 0x%x\n",
                    (unsigned) mmap->size,
                    (unsigned) mmap->base_addr_high,
                    (unsigned) mmap->base_addr_low,
                    (unsigned) mmap->length_high,
                    (unsigned) mmap->length_low,
                    (unsigned) mmap->type);
            if(mmap->type == 1){ /* this is the last '1', we want to know about it */
                mem_free_start = (int) &_end;
                mem_free_length = 0; /* may have been altered last mmap->type==1 */
                while((mem_free_start % MEM_MINBYTESIZE) != 0){
                    /* this puts us at an even page */
                    mem_free_start++;
                }
                /* Calculate physical memory amount (will be over wrote until last) */
                /* FIXME see if we can do this with a union instead */
                long mmapbase,mmaplength; 
                /*mmapbase = mmap->base_addr_high;*/              /* put it in          */
                /*mmapbase = mmapbase << (sizeof(mmap->base_addr_high)*4);*/ /* move to upper half */
                /*mmapbase += mmap->base_addr_low;*/              /* put in bottom half */
                mem_usable_start = (void *)
                /*mmapbase = */
                    (mmap->base_addr_high << (sizeof(mmap->base_addr_high)*4)) 
                    + mmap->base_addr_low;

                mmapbase = 0;/* CLEANME */
                mmaplength = 
                    (mmap->length_high << (sizeof(mmap->length_high)*4)) 
                    + mmap->length_low;

                long free_mem_end = mmapbase + mmaplength;/* this will tell use where to */
                                                          /* stop the following for loop */
               
                /* will this modulo always be 0? */
                mem_phys_end = mem_usable_start + (mmaplength/4); 
                /*printf("floor is: %x\n",mem_floor_page((uptr*)0xDEADDEAD));*/
                /*printf("floor is: %x\n",MEM_CEIL_PAGE((uptr*)0xDEADDEAD));*/
                /*printf("mmapbase is %x \n",mmapbase);*/
                /*printf("mmaplength is %x \n",mmaplength);*/
                while((mem_free_start + mem_free_length) < free_mem_end){
                    /* As a side effect, this should keep mem_free_length in even block sizes */
                    mem_free_length += MEM_MINBYTESIZE;
                }
                /* I don't like getting mem_phys_amount this way.   This is the end */
                /* of the usable stuff, but I think there are some driver things    */
                /* above this.   If they need to be ident paged, we'll have to      */
                /* figure out how to get past them.  Grub always gives a map of 4Gs */
                mem_phys_amount = mem_free_start + mem_free_length;
            }
        }
    }
    /* Set up memory allocation */
    /* Find out where to put the page bitmap. */
    /* It will be after one of the 3 follows things. */
    mem_pgs_bitmap_start = 0; /* start lowest and increase */
    if(MEM_CEIL_PAGE(_end) > mem_pgs_bitmap_start){
        mem_pgs_bitmap_start = MEM_CEIL_PAGE(_end);
    }
    if(MEM_CEIL_PAGE(mem_elf_start) > mem_pgs_bitmap_start){
        mem_pgs_bitmap_start = MEM_CEIL_PAGE(mem_elf_start);
    }
    if(MEM_CEIL_PAGE(mem_modules_end) > mem_pgs_bitmap_start){
        mem_pgs_bitmap_start = MEM_CEIL_PAGE(mem_modules_end);
    }
    /*printf("mem_modules_start: %x \n",mem_modules_start);
    printf("mem_modules_end: %x \n",mem_modules_end);
    printf("mem_pgs_bitmap_start: %x \n",mem_pgs_bitmap_start);
    printf("mem_pgs_bitmap_need: %x \n",mem_pgs_bitmap_need(mem_phys_end));*/
    /* Mark all pages available */
    mem_pgs_bitmap_make_avail(mem_pgs_bitmap_start, 
                              mem_pgs_bitmap_need(mem_phys_end));
    /* Allocate space for 1:1 page directory, 1:1 page tables, and IDT */
    mem_kernl_pg_dir = mem_pgs_bitmap_start + 
                       (MEM_PAGE_SIZE * mem_pgs_bitmap_need(mem_phys_end)) / 
                       sizeof(uptr); /* skip page bitmap */
    mem_ident_pg_tbl = MEM_CEIL_PAGE(mem_kernl_pg_dir); /* next... */
    mem_itr_desc_tbl = MEM_CEIL_PAGE(mem_ident_pg_tbl); /* next... */
    /*printf("mem_kernl_pg_dir: %x \n",mem_kernl_pg_dir);
    printf("mem_ident_pg_tbl: %x \n",mem_ident_pg_tbl);
    printf("mem_itr_desc_tbl: %x \n",mem_itr_desc_tbl);*/
    /*mem_pgs_bitmap_zero_below_addr(0x1F000); */
    /*mem_pgs_bitmap_zero_below_addr(0x19000); */
    /*mem_pgs_bitmap_zero_below_addr(0x20000); */
    /*mem_pgs_bitmap_zero_below_addr(0x21000); */
    mem_pgs_bitmap_zero_below_addr(mem_ident_pg_tbl);
    /*mem_pgs_bitmap_zero_below_addr(mem_itr_desc_tbl); */

    /* Investigate CPU */
    unsigned int regs[4];
    cpuid_string(CPUID_GETVENDORSTRING,regs);
    cpu_fill_vendorstring(regs);  /* also fills cpu_max_feats */
    printf("CPU vendorstring is %s\n",cpu_vendorstring); 
    printf(" CPU max feats: %x\n",cpu_max_feats);
    printf("_start is %x and _end is %x and _edata is %x, and stack is %x\n",&_start,&_end,&_edata,&stack); 
    /*printf("stack is %x \n",&stack); */
    /* Here is a good place as any to set up the IDT                   */
    /* It does NOT have to be 4k aligned so we'll leave                */ 
    /* it static in the kernel, until we come up with something better */

    /* We did not do anything with (A)PIC yet.   We did the CPU invesitgation above */
    /* to prep for it.   If there is a problem with the defaults, we'll change it   */
    unsigned long cs; /* IDT needs cs register */
    asm volatile("mov %%cs,%0": "=r"(cs));
    interrupts_init_idt(cs);
    interrupts_enable();
    /* We're getting ready for paging... so we need an ident page set up.
     To do the full 4GB would be wasteful, so we need to allocate         
     enough blocks to address our physical memory (or what the            
     boot loader told us we have)                                            */

    /* Since we don't want this moving around, we're going to manually adjust  */
    /* mem_free_start and mem_free_length, and update memblocks[] according    */

    /* 4 bytes per pg_tbl_entry */
    int num_blocks_needed=(4*mem_phys_amount)/(MEM_PAGE_SIZE*MEM_MINBYTESIZE);
    /* Because the above might end in fraction, finish it off... */
    if(((4*mem_phys_amount) % (MEM_PAGE_SIZE*MEM_MINBYTESIZE)) > 0){
        num_blocks_needed++;
    }

    /* save mem_free_start to mem_kernl_pg_dir before we lose it */
    mem_kernl_pg_dir= (void *) mem_free_start; 

    /* put mem_no_mans_page after it */
    mem_no_mans_page= (void *) mem_free_start + 0x1000; /* skip mem_kernl_pg_dir */

    /* start mem_ident_pg_tbl at the next block */
    mem_ident_pg_tbl= (void *) mem_free_start + MEM_MINBYTESIZE; /* skip to next block */

    /* now we can set up the kernels ident paging */
    mem_init_ident_paging(num_blocks_needed); 
    num_blocks_needed++; /* include mem_no_mans_page and mem_kernl_pg_dir */

    /* update mem_free_start/length */
    mem_free_start += num_blocks_needed*MEM_MINBYTESIZE;
    mem_free_length -= num_blocks_needed*MEM_MINBYTESIZE;

    mem_init_blocks(0xFFFFFFFF); /* mark all used */ /* 32BITASSUMED */

    /* start mem_free_start, and free mem_free_length in MEM_MINBYTESIZE chunks */
    mem_free_block(mem_free_start, (mem_free_length/MEM_MINBYTESIZE));

    /*printf("mem_phys_amount is %x\n",mem_phys_amount);*/
    /*printf("&mem_kernl_pg_dir starts at %x\n",&mem_kernl_pg_dir[0]);
    printf("&mem_ident_pg_tbl starts at %x\n",&mem_ident_pg_tbl[0]);
    printf("&mem_no_mans_page starts at %x\n",&mem_no_mans_page[0]);*/

    /* Enable paging */

    cpu_cur_regs.cr[3]=(int) mem_kernl_pg_dir;
    cpu_reg_write_cr3();

    cpu_reg_read_cr0();
    CPU_ENABLE_BIT_ON_CR(CPU_CR0_PG,0);
    cpu_reg_write_cr0();

    /* unsigned long long gdt;
    asm volatile("sgdt %0": "=m"(gdt)); */

    cpu_reg_read_eflags();
    printf("eflags is %x\n",cpu_cur_regs.eflags);

    /* pci stuff */
    /*
    unsigned short i,func;
    unsigned long vendev,vendor,device,header;
    for(i=0;i<11;i++){
        func=0;
        vendev=pci_config_read(0,i,func,0);
        vendor=vendev & 0xFFFF;
        if (vendor != 0xFFFF) {
            device = vendev >> 16;
            printf("pci device %d on bus 0 is %x:%x\n",i,vendor,device);
            header=(pci_config_read(0,i,func,0x0C) >> 16) & 0xFF;
            printf("\t header is %x\n",header);
            if((header >> 7) == 1){*/
                /*multi-function */
                /*
                do{
                    func++;
                    vendev=pci_config_read(0,i,func,0);
                    vendor=vendev & 0xFFFF;
                    device = vendev >> 16;
                    printf("\t\tpci device %d on bus 0 is %x:%x\n",i,vendor,device);
                    header=(pci_config_read(0,i,func,0x0C) >> 16) & 0xFF;
                    printf("\t\t header is %x\n",header);
                }while(vendor != 0xFFFF);
            }
        }
    }*/

    //unsigned int j;
    //printf("waiting...\n");
    //for(j=0; j < 40000000;j++){}
    printf("mod_jump is %x\n",mod_jump);
    //goto *mod_jump;
    printf("Done\n");
printf("0000000000000000000000000000000000000000000000000000000000000000000000000000000\n");
printf("1000000000000000000000000000000000000000000000000000000000000000000000000000001\n");
printf("2000000000000000000000000000000000000000000000000000000000000000000000000000002\n");
printf("3000000000000000000000000000000000000000000000000000000000000000000000000000003\n");
printf("4000000000000000000000000000000000000000000000000000000000000000000000000000004\n");
printf("5000000000000000000000000000000000000000000000000000000000000000000000000000005\n");
printf("6000000000000000000000000000000000000000000000000000000000000000000000000000006\n");
printf("7000000000000000000000000000000000000000000000000000000000000000000000000000007\n");
printf("8000000000000000000000000000000000000000000000000000000000000000000000000000008\n");
printf("9000000000000000000000000000000000000000000000000000000000000000000000000000009\n");
printf("1000000000000000000000000000000000000000000000000000000000000000000000000000010\n");
printf("1100000000000000000000000000000000000000000000000000000000000000000000000000011\n");
printf("1200000000000000000000000000000000000000000000000000000000000000000000000000012\n");
printf("1300000000000000000000000000000000000000000000000000000000000000000000000000013\n");
printf("1400000000000000000000000000000000000000000000000000000000000000000000000000014\n");
printf("1500000000000000000000000000000000000000000000000000000000000000000000000000015\n");
printf("1600000000000000000000000000000000000000000000000000000000000000000000000000016\n");
printf("1700000000000000000000000000000000000000000000000000000000000000000000000000017\n");
printf("1800000000000000000000000000000000000000000000000000000000000000000000000000018\n");
printf("1900000000000000000000000000000000000000000000000000000000000000000000000000019\n");
uint8 mybuf[10];
int i;
for (i = 0; i < 10; i++){
    mybuf[i] = 0;
}
uint8 mypickup[20];
mypickup[0] = 'S';
mypickup[1] = 'h';
mypickup[2] = 'a';
mypickup[3] = 'u';
mypickup[4] = 'n';
mypickup[5] = 'B';
mypickup[6] = 'r';
mypickup[7] = 'a';
mypickup[8] = 'd';
mypickup[9] = 'y';
mypickup[10] = 's';
mypickup[11] = 'H';
mypickup[12] = 'A';
mypickup[13] = 'U';
mypickup[14] = 'N';
mypickup[15] = 'b';
mypickup[16] = 'R';
mypickup[17] = 'A';
mypickup[18] = 'D';
mypickup[19] = 'Y';
uint8 mydrop[20];
for (i = 0; i < 20; i++){
    mydrop[i] = 0;
}
fifo_t myfifo;
fifo_init(&myfifo, mybuf, 10);
/*printf("Before:\n");
printf("myfifo.size: %x\n", myfifo.size);
printf("myfifo.start: %x\n", myfifo.start);
printf("myfifo.end: %x\n", myfifo.end);
printf("myfifo.available: %x\n", atomic_read(&myfifo.available));
printf("mybuf: %s\n",mybuf);*/
/*for (i = 0; i < 10; i++){
    printf("mybuf[i]: %x\n",mybuf[i]);
}*/
fifo_push(&myfifo, mypickup, 5);
/*for(i = 0; i < 10; i ++){
    printf("|%c",mybuf[i]);
}
printf("\n");*/
fifo_push(&myfifo, mypickup + 5, 5);
fifo_pop(&myfifo, mydrop, 5);
printf("mydrop: %s\n",mydrop);
for (i = 0; i < 20; i++){
    mydrop[i] = 0;
}
fifo_push(&myfifo, mypickup + 10, 5);
fifo_pop(&myfifo, mydrop, 5);
printf("mydrop: %s\n",mydrop);
for (i = 0; i < 20; i++){
    mydrop[i] = 0;
}

/*printf("After:\n");
printf("myfifo.size: %x\n", myfifo.size);
printf("myfifo.start: %x\n", myfifo.start);
printf("myfifo.end: %x\n", myfifo.end);
printf("myfifo.available: %x\n", atomic_read(&myfifo.available));*/
/*printf("mybuf: %s\n",mybuf);*/
fifo_pop(&myfifo, mydrop + 2, 2);
printf("mydrop: %s\n",mydrop);
/*for (i = 0; i < 10; i++){
    printf("mybuf[i]: %x\n",mybuf[i]);
}*/
/*uint32 fifo_init(fifo_t *f, uptr *buf, uint32 size);

uint32 fifo_push(fifo_t *f, uptr *pickup, uint32 size);*/
}

/*typedef struct {
    uint32 start;
    uint32 end;
    uptr *buffer;
    atomic32_t available;
    uint32 size;
} fifo_t; */
