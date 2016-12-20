#ifndef _MEMORY_C_
#define _MEMORY_C_

#include "memory.h"
#include "screen.h"

int mem_pgs_bitmap_need(int bytes){
    /* Number of pages needed to address total pages of system */
    /* 
        T = pages in system
        N = pages needed to address T

        T = phys mem bytes / PG_SZ 
        N = ceil( (T/8) / PG_SZ )
        ... algebra ...
        N = ceil( phys mem bytes / (8*(PG_SZ*2)) )
    */
    int ret = bytes / (MEM_BIAB*(MEM_PAGE_SIZE*MEM_PAGE_SIZE));
    if(bytes % (MEM_BIAB*(MEM_PAGE_SIZE*MEM_PAGE_SIZE))){
        ret += 1;
    }
    return ret;
}

void mem_pgs_bitmap_make_avail(uptr *addr, int num_pgs){
    /* "One"ing page allocation bitmap */
    /* Get total amount of uregs ... */
    int t = (num_pgs*MEM_PAGE_SIZE)/(sizeof(ureg));
    int i;
    /* ...recast in case register size != pointer size... */
    ureg *addr_rc = addr; 
    /* ..."one" each index */
    for(i = 0; i < t; i++){
        addr_rc[i] = ALL_ONES;
    }
}

void mem_pgs_bitmap_zero(uptr *addr, int num_pgs){
    /* Zeroing page allocation bitmap */
    /* Get total amount of uregs ... */
    int t = (num_pgs*MEM_PAGE_SIZE)/(sizeof(ureg));
    int i;
    /* ...recast in case register size != pointer size... */
    ureg *addr_rc = addr; 
    /* ...zero each index */
    for(i = 0; i < t; i++){
        addr_rc[i] = 0;
    }
}

void mem_pgs_bitmap_zero_below_addr(uptr *addr){
    /* Make unavailable all pages INCLUSIVELY before addr
       ie: Give me the base address of a page 
       that page is now unavailable 
    */
    int ureg_index = MEM_PGS_REG_IDX(addr);
    int bit_index = MEM_PGS_BIT_IDX(addr);
    int i;
    ureg *bitmap = mem_pgs_bitmap_start;/* cast to reg */
    for( i = 0 ; i < ureg_index; i ++){
        bitmap[i] = 0;
    }
    /* Do the bits in the last part */
    bitmap[i] &= ALL_ONES << bit_index;
}

void mem_pgs_bitmap_round_off_phys_mem(){
    /* We allocate N pages for page bitmap... but we might 
    not (likely) use all of it.  make what we don't unavailalbe */
}

void mem_init_blocks(unsigned long initval){
    unsigned long i;
    for(i=0;i<MEM_BLOCKARRAY_SIZE;i++){
        mem_blocks[i]=initval;
    }    
}

void mem_mark_used_above(int blkaddr){
    /* Marks everything above blkaddr as used */
    /* This function alone reveals how hackish this */
    /* implimentation is.  Might keep a marker to */
    /* stop searches at this point */
    //printf("make_above: %d\n",blkaddr);
    mem_blocks_avail=blkaddr;
    unsigned long i;
    for(i=(blkaddr & 31);i<32;i++){
        mem_blocks[(blkaddr>>5)] |= (MEM_BLOCK_USED<<i);
        /* printf("chew chew i is %u %u \n",i,(1<<i)); */
    }
    for(i=(blkaddr>>5)+1;i<MEM_BLOCKARRAY_SIZE;i++){
        mem_blocks[i]=0xFFFFFFFF;
        /*printf("crunch crunch i is %d\n",i);*/
    }
}

void mem_demarc_noncontig(){
    /* In this function we break our mem_blocks_avail */
    /* into the first part which is a complete base 2 */
    /* size.  The 2nd part is everything else.        */
    /* Rational: We will use the first part for our   */
    /* crazy subnetting idea.  The second part will   */
    /* allow us to allocate non-contiguous blocks     */
    for(mem_blocks_noncontig=1;mem_blocks_noncontig<(mem_blocks_avail/2) ;mem_blocks_noncontig<<=1){
    }
}

void mem_free_block(int ptr, int blkcount){
    /* translates ptr into block address and */
    /* frees blkcount blocks starting there */
    int blkaddr = ptr / MEM_MINBYTESIZE;
    while(blkcount > 0){
        mem_blocks[(blkaddr>>5)] &= ~(1<<(blkaddr & 0x1F));
        blkaddr++;
        blkcount--;
    }
}

void mem_init_ident_paging(int num_blks){
    /* num_blks number of blocks worth of ident paging */

    /* set page directory to every 1024th table entry */
    int i;
    for(i=0;i<(num_blks*MEM_MINBYTESIZE/4096);i++){/* 4096=pg_tbl */
        mem_kernl_pg_dir[i]=((unsigned int) &mem_ident_pg_tbl[i*1024]) | 3;/* 3 for access bits */
    }
    mem_init_no_mans_page();
    for(;i<1024;i++){//set rest to no_mans
        mem_kernl_pg_dir[i]=((unsigned int) mem_no_mans_page) | 3;
    }
    /* set page table to every 4096th physical address */
    for(i=0;i<(num_blks*MEM_MINBYTESIZE/4);i++){/* 4=page table entry size */
        mem_ident_pg_tbl[i]=(i*4096) | 3; /* 3 for access bits */
    }
    /* elements after this one shouldnt be accessed */
}

void mem_init_no_mans_page(){
    /* Make a page table in the front of mem_no_mans_page */
    /* that references the rest of mem_no_mans_page       */
    int i;
    for(i=0;i<1024;i++){
        mem_no_mans_page[i]=((unsigned int) &mem_no_mans_page[1024]) | 3; /* 3 for access bits */
    }
    for(;i<2048;i++){
        mem_no_mans_page[i]= 0x41515541; /* AQUA at [1024] */
    }
}

#endif
