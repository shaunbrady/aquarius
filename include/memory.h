#ifndef _MEMORY_H_
#define _MEMORY_H_

#include "types.h"

/* constants */
#define X_MEM_MINBYTESIZE 65536
#define MEM_MINBYTESIZE 4096
#define MEM_MINBYTESIZE_BIT 16
#define MEM_BLOCKARRAY_SIZE  2048
#define MEM_BLOCK_USED 1
#define MEM_BLOCK_FREE 0
#define MEM_PAGE_SIZE 4096
#define MEM_BIAB 8 /* Bits In A Byte */

#define MEM_PDE_PSNT 0x01 /* Present */
#define MEM_PDE_RW   0x02 /* Read/Write */
#define MEM_PDE_USR  0x04 /* User/Supervisor */
#define MEM_PDE_WT   0x08 /* Write Through */
#define MEM_PDE_CD   0x10 /* Cache Disabled */
#define MEM_PDE_ACC  0x20 /* Accessed */
#define MEM_PDE_ZERO 0x00 /* A zero for the 0x40 bit */
#define MEM_PDE_4MBS 0x80 /* 4MB Page size */
#define MEM_PDE_4KBS 0x00 /* 4KB Page size (see above) */
#define MEM_PDE_IGNR  256 /* Ignored */

#define MEM_PTE_PSNT 0x01 /* Present */
#define MEM_PTE_RW   0x02 /* Read/Write */
#define MEM_PTE_USR  0x04 /* User/Supervisor */
#define MEM_PTE_WT   0x08 /* Write Trhough */
#define MEM_PTE_CD   0x10 /* Cache Disabled */
#define MEM_PTE_ACC  0x20 /* Accessed */
#define MEM_PTE_DRTY 0x40 /* Dirty - CPU will NOT reset */
#define MEM_PTE_ZERO 0x00 /* A zero for the 0x80 bit */
#define MEM_PTE_GOBL  256 /* Global */

#define ALL_ONES 0xFFFFFFFF /* 32BITASSUMED */

/* macros */
#define MEM_FLOOR_PAGE(addr) (uptr*)(((uptr) addr) & 0xFFFFF000)   /* 32BITASSUMED */
#define MEM_CEIL_PAGE(addr) (uptr*)((((uptr) addr) & 0xFFFFF000) + MEM_PAGE_SIZE)
#define MEM_PGS_NUM(addr) ((((uptr) addr) & 0xFFFFF000) / MEM_PAGE_SIZE)
#define MEM_PGS_REG_IDX(addr) MEM_PGS_NUM(addr) / (sizeof(ureg)*MEM_BIAB)
#define MEM_PGS_BIT_IDX(addr) MEM_PGS_NUM(addr) % (sizeof(ureg)*MEM_BIAB)

/* structures */
struct mem_phys_page{
    uptr *addr;
    uptr *buddy_addr; /* def: when buddy is free, we join */
    ureg flags;
};

/* variables */
unsigned long mem_blocks[MEM_BLOCKARRAY_SIZE];  /* 2048 for now */
unsigned long mem_blocks_avail;
unsigned long mem_blocks_noncontig;
unsigned long mem_free_start;                   /* address after kernel */
unsigned long mem_free_length;                  /* how much free */
unsigned long mem_phys_amount;                  /* amount of physical memory */
/*unsigned int  *mem_kernl_pg_dir;
unsigned int  *mem_ident_pg_tbl;
unsigned int  *mem_no_mans_page;*/

uptr *mem_phys_end;
uptr *mem_usable_start;      /* Image usually lives here. */
uptr *mem_elf_start;         /* Usually right after our code, still in image */
uptr *mem_modules_start;     /* Usually right after elf section header */
uptr *mem_modules_end;       /* Usually right after elf section header */
uptr *mem_pgs_bitmap_start;  /* Goes after last of modules/elf/image */ 
uptr *mem_kernl_pg_dir;      /* Identity page directory */
uptr *mem_ident_pg_tbl;      /* Identity page table */
uptr *mem_itr_desc_tbl;      /* Interrupt Descriptor Table */
uptr *mem_no_mans_page;      /* No Mans Page - nothing lives here */

/*uint32 mem_page_bitmap_page_count;*/

/* functions */

/* Page Availability Bitmap */
int mem_pgs_bitmap_need();
void mem_pgs_bitmap_zero(uptr *addr, int num_pgs);
void mem_pgs_bitmap_make_avail(uptr *addr, int num_pgs);
void mem_pgs_bitmap_zero_below_addr(uptr *addr); 

void mem_init_blocks(unsigned long initval);
void mem_mark_used_above(int blkaddr);
void mem_demarc_noncontig();
void mem_free_block(int ptr, int blkcount);
int mem_blk_size();
void mem_init_ident_paging(int num_blocks);
void mem_init_no_mans_page();


#endif
