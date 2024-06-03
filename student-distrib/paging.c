// Checkpoint 1 New File
#include "paging.h"

/* paging_init
 * Initialize paging
 * Inputs: None
 * Outputs: None
 * Side Effects: Set the default page size as 4kB, enable paging
 */
void paging_init(){
    int i;
    directory[0].present = 1;                           //set up for the first 4kb page, points to a page table
    directory[0].RW = 1;
    directory[0].US = 0;
    directory[0].PWT = 0;
    directory[0].PCD = 0;
    directory[0].access = 0;
    directory[0].reserved2 = 0;
    directory[0].PS = 0;                          //the page size is 4kb
    directory[0].G = 0;
    directory[0].avail = 0;
    directory[0].addr = ((unsigned long)pageTable >> 12); //right shift by 12 to get base page address 

    directory[1].present = 1;                           //set up for the kernel page
    directory[1].RW = 1;                                  //change to 0 for kernel r-
    directory[1].US = 0;
    directory[1].PWT = 0;
    directory[1].PCD = 0;
    directory[1].access = 0;
    directory[1].reserved2 = 0;
    directory[1].PS = 1;                          //the page size is 4mb
    directory[1].G = 1;                           
    directory[1].avail = 0;
    directory[1].addr = KERNEL_ADDR >> 12;        //get the upper 10 bits; right shift by 12 to get base page address

    for(i = 0 ; i < ENTRIES; i++) {
        if (i == (VID_MEM >> 12)) {               // right shift by 12 to retain 0xb8 which page addr.
            pageTable[i].present = 1;
            pageTable[i].page_addr = 184;         // 0xb8000 divided by 4096
        }else if (i == 185){
            pageTable[i].present = 1;
            pageTable[i].page_addr = 185;         // 0xb9000 divided by 4096
        }
        else if (i == 186){
            pageTable[i].present = 1;
            pageTable[i].page_addr = 186;         // 0xba000 divided by 4096
        }  
        else if (i == 187){
            pageTable[i].present = 1;
            pageTable[i].page_addr = 187;         // 0xbb000 divided by 4096
        }      
        else {
            pageTable[i].present = 0;
            pageTable[i].page_addr = i;
        }
        pageTable[i].RW = 1;
        pageTable[i].US = 0;
        pageTable[i].PWT = 0;
        pageTable[i].PCD = 0;
        pageTable[i].access = 0;
        pageTable[i].D = 0;
        pageTable[i].PAT = 0;
        pageTable[i].G = 0;
        pageTable[i].avail = 0;
    }
    
    for(i = 2 ; i < ENTRIES; i++) {
        directory[i].present = 0;                           //set up for the kernel page
        directory[i].RW = 1;                                  //change to 0 for kernel r-
        directory[i].US = 0;
        directory[i].PWT = 0;
        directory[i].PCD = 0;
        directory[i].access = 0;
        directory[i].reserved2 = 0;
        directory[i].PS = 1;                          //the page size is 4mb
        directory[i].G = 0;                           
        directory[i].avail = 0;
        directory[i].addr = i;        
    }

    loadPageDirectory((int)directory);
    //enablePaging();
}

/* pageSetup
 * Set up paging for directory index 32
 * Inputs: pid
 * Outputs: None
 */
void pageSetup(int pid){
    // number 32 indicates 128MB as each count is 4MB
    directory[32].present = 1;                             //set up for the kernel page
    directory[32].RW = 1;                                  //change to 0 for kernel r-
    directory[32].US = 1;
    directory[32].PWT = 0;
    directory[32].PCD = 0;
    directory[32].access = 0;
    directory[32].reserved2 = 0;
    directory[32].PS = 1;                                 //the page size is 4mb
    directory[32].G = 1;                           
    directory[32].avail = 0;
    directory[32].addr = (EIGHT_MB + FOUR_MB*pid) >> 12;        //get the upper 10 bits; right shift by 12 to get base page address

    loadPageDirectory((int)directory);
    // enablePaging();
}

/* pageSetup_vidmap()
 * Set up paging for vidmap
 * Inputs: None
 * Outputs: None
 */
void pageSetup_vidmap(){
    // index
    int i;

    // number 33 indicates 132MB as each count is 4MB
    directory[33].present = 1;                           //set up for the first 4kb page, points to a new page table for vidmap
    directory[33].RW = 1;
    directory[33].US = 1;
    directory[33].PWT = 0;
    directory[33].PCD = 0;
    directory[33].access = 0;
    directory[33].reserved2 = 0;
    directory[33].PS = 0;                          //the page size is 4kb
    directory[33].G = 1;
    directory[33].avail = 0;
    directory[33].addr = ((unsigned long)pageTabletwo >> 12); //right shift by 12 to get base page address of new page table

    pageTabletwo[0].present = 1;                          //mark page table entry as present
    pageTabletwo[0].page_addr = 184;                      //set the page address to 184 so that it's same video memory as prev (0xb8000 divided by 4096)
    pageTabletwo[0].RW = 1;
    pageTabletwo[0].US = 1;
    pageTabletwo[0].PWT = 0;
    pageTabletwo[0].PCD = 0;
    pageTabletwo[0].access = 0;
    pageTabletwo[0].D = 0;
    pageTabletwo[0].PAT = 0;
    pageTabletwo[0].G = 1;
    pageTabletwo[0].avail = 0;

    for(i = 1 ; i < ENTRIES; i++) {             //set up the rest of the entires in the new page table
        pageTabletwo[i].present = 1;
        pageTabletwo[i].page_addr = i; 
        pageTabletwo[i].RW = 1;
        pageTabletwo[i].US = 1;
        pageTabletwo[i].PWT = 0;
        pageTabletwo[i].PCD = 0;
        pageTabletwo[i].access = 0;
        pageTabletwo[i].D = 0;
        pageTabletwo[i].PAT = 0;
        pageTabletwo[i].G = 0;
        pageTabletwo[i].avail = 0;
    }

    loadPageDirectory((int)directory);            //flushes tlb
}

void flush_TLB(){
    loadPageDirectory((int)directory);
}

/* void task_video_mapping(int index, int same_flag)
 * Map temrinal to corresponding page address
 * Inputs: int index, int same_flag
 * Outputs: None
 */
void task_video_mapping(int index, int same_flag){
    // index
    // check if task to map corresponds to displayed terminal 
    if (same_flag == 1){
        // map to b8000; 184 = b8000 >> 12
        pageTable[184].page_addr = 184;
        // pageSetup(terminal_arr[cur_terminal_index-1].youngest_pid);
    }
    else{
        // map to b8000 from corresponding to background terminal; 184 = b8000 >> 12
        pageTable[184].page_addr = 184 + index + 1;
        // pageSetup(terminal_arr[cur_terminal_index-1].youngest_pid);
    }
    flush_TLB();

    // loadPageDirectory((int)directory);            //flushes tlb
}

/* void task_vidmap(int index, int same_flag)
 * Map temrinal to corresponding page address
 * Inputs: int index, int same_flag
 * Outputs: None
 */
void task_vidmap(int index, int same_flag){
    // index
    // check if task to map corresponds to displayed terminal 
    if (same_flag == 1){
        // map to b8000; 184 = b8000 >> 12
        pageTabletwo[0].page_addr = 184;
        // pageSetup(terminal_arr[cur_terminal_index-1].youngest_pid);
    }
    else{
        // map to b8000 from corresponding to background terminal; 184 = b8000 >> 12
        pageTabletwo[0].page_addr = 184 + index + 1;
        // pageSetup(terminal_arr[cur_terminal_index-1].youngest_pid);
    }

    loadPageDirectory((int)directory);            //flushes tlb
}
