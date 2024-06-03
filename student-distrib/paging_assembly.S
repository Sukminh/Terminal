#define ASM 1
#include "paging.h"

// enable paging directory, putting directory address to cr3 register
.globl loadPageDirectory
.align 4 // padding memory space allocation
loadPageDirectory:
    pushl %ebp
    movl %esp, %ebp
    movl 8(%ebp), %eax
    mov %eax, %cr3
    mov %cr4, %eax
    or $0x00000010, %eax // To extend page size to 4mB
    mov %eax, %cr4
    mov %cr0, %eax
    or $0x80000001, %eax // set 32th bit in the cr0 register for pagining bit, 1st bit to turn on the protected mode to prevent interrupt
    mov %eax, %cr0
    movl %ebp, %esp
    popl %ebp
    ret

.globl directory, pageTable, pageTabletwo
.align 4096 // padding memory space allocation
directory:
    .rept ENTRIES // number of index in page directory
    .long 0
    .endr

.align 4096 // padding memory space allocation
pageTable:
    .rept ENTRIES // number of index in page table
    .long 0
    .endr

.align 4096 // padding memory space allocation
pageTabletwo:
    .rept ENTRIES // number of index in page table
    .long 0
    .endr
    
