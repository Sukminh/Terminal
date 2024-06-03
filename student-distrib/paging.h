
#ifndef _PAGING_H
#define _PAGING_H
#define ENTRIES     1024

#ifndef ASM

#include "types.h"
#include "x86_desc.h"
#include "scheduler.h"
#include "lib.h"

#define KERNEL_ADDR 0x400000       //4MB
#define VID_MEM 0xb8000     // starting location video mem.
#define EIGHT_MB    0x800000
#define FOUR_MB     0x400000

extern void paging_init();
extern void pageSetup(int pid);
extern void pageSetup_vidmap();
void flush_TLB();
extern void task_video_mapping(int index, int same_flag);
void task_vidmap(int index, int same_flag);

typedef struct pde_t {
    uint32_t present          :1;
    uint32_t RW         :1;
    uint32_t US         :1;
    uint32_t PWT        :1;
    uint32_t PCD        :1;
    uint32_t access     :1;
    uint32_t reserved2  :1;      //0 if 4kb, D if 4mb
    uint32_t PS         :1;      //page size, differ between 4kb and 4mb
    uint32_t G          :1;
    uint32_t avail      :3;
    uint32_t addr       :20;     //set the base address for pt or page, depends on size
} pde_t;

typedef struct pte_t {
    uint32_t present    :1;      //present
    uint32_t RW         :1;      //read/write
    uint32_t US         :1;      //user/supervisor
    uint32_t PWT        :1;      //page-level write-through
    uint32_t PCD        :1;      //page-level cache disable
    uint32_t access     :1;      //accessed
    uint32_t D          :1;      //dirty
    uint32_t PAT        :1;      //page attribute table index
    uint32_t G          :1;      //global
    uint32_t avail      :3;      //reserved and available to software bits
    uint32_t page_addr  :20;     //base address of a page
}pte_t;

extern pde_t directory[ENTRIES];
extern pte_t pageTable[ENTRIES];
extern pte_t pageTabletwo[ENTRIES];

#endif

#endif
