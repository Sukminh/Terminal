// Checkpoint 1 New File
#include "idt.h"

const char* titles[] = {
    [0]="Divide by Zero",
    [1]="Debug",
    [2]="NMI Interrupt",
    [3]="Breakpoint",
    [4]="Overflow",
    [5]="Bound Range Exceeded",
    [6]="Invalid Opcode",
    [7]="Device Busy",
    [8]="Double Fault",
    [9]="Coprocessor Overrun",
    [10]="Invalid TSS",
    [11]="Segment not Present",
    [12]="Stack-Segment Fault", 
    [13]="General Protection",
    [14]="Page Fault",
    [15]="reserve",
    [16]="x87 FPU",
    [17]="Alignment Check",
    [18]="Machine Check",
    [19]="SIMD Floating Point",
};

/*idt_init
    in: none
    out: none
    return: none
    effect: populates the idt table and fills it to idt_desc_ptr
*/
void idt_init(void) {
    int i;
    for (i = 0; i < NUM_VEC; i++) {
        // check if it's Intel reserved Exception
        if (i < NUM_EXCEPTIONS && i != 15)
        {
            idt[i].present = 1;
        }
        else {
            idt[i].present = 0;
        }

        // each entry is 32-bit so size should always be 1
        idt[i].size = 1;

        //Descriptor Privilege Level should be 0 (user programs), but should be 3 for system calls because they are a kernel service from user land, so we need more privilege
        idt[i].dpl = 0;

        // Reserved Bits should be 01110 for the interrupt gate
        idt[i].reserved0 = 0;
        idt[i].reserved1 = 1;
        idt[i].reserved2 = 1;
        idt[i].reserved3 = 1; 
        idt[i].reserved4 = 0;   
        
        //segment selector should be set to KERNEL_SC macro
        idt[i].seg_selector = KERNEL_CS;
    }

    //exception offsets
    SET_IDT_ENTRY(idt[0], divide);
    SET_IDT_ENTRY(idt[1], reserve);
    SET_IDT_ENTRY(idt[2], nmi);
    SET_IDT_ENTRY(idt[3], breakpoint);
    SET_IDT_ENTRY(idt[4], overflow);
    SET_IDT_ENTRY(idt[5], bound);
    SET_IDT_ENTRY(idt[6], opcode);
    SET_IDT_ENTRY(idt[7], dna);
    SET_IDT_ENTRY(idt[8], doubleFault);
    SET_IDT_ENTRY(idt[9], coprocessor);
    SET_IDT_ENTRY(idt[10],tssStack);
    SET_IDT_ENTRY(idt[11],segment);
    SET_IDT_ENTRY(idt[12],stack);
    SET_IDT_ENTRY(idt[13],generalProtection);
    SET_IDT_ENTRY(idt[14],pageFault);
    SET_IDT_ENTRY(idt[16],fpu);
    SET_IDT_ENTRY(idt[17],alignmentCheck);
    SET_IDT_ENTRY(idt[18],machineCheck);
    SET_IDT_ENTRY(idt[19],simd);

    //each interrupt (keyboard, rtc, syscall) will have reserve bit set to 1
    idt[PIT_IDT].reserved3 = 1;
    idt[KEYBOARD_IDT].reserved3 = 1;
    idt[RTC_IDT].reserved3 = 1;
    idt[SYSCALL_IDT].reserved3 = 1;

    //also set all of them to present
    idt[PIT_IDT].present = 1;
    idt[KEYBOARD_IDT].present = 1;
    idt[SYSCALL_IDT].present = 1;
    idt[RTC_IDT].present = 1;

    SET_IDT_ENTRY(idt[PIT_IDT], handle_pit);
    SET_IDT_ENTRY(idt[KEYBOARD_IDT], handle_keyboard);
    SET_IDT_ENTRY(idt[RTC_IDT], handle_rtc);
    SET_IDT_ENTRY(idt[SYSCALL_IDT], syscall_handler);

    idt[SYSCALL_IDT].dpl = 3; // setting privilege level into 3

    lidt(idt_desc_ptr);
};


/* 
    exception_handler
    Handle corresponding Exception based on pushed exception vector
    in: exception vector
    out: none
    return: none
    effect: prints exception message
*/
void exception_handler(int vect) {
    printf("Exception: %s\n", titles[vect]);
    system_exception_halt_flag = 1;
    system_halt(0); 

    cli();
    while(1){}

}

// /* 
//     system_call
//     Testing System Call by priting System Call when it's called
//     in: none
//     out: none
//     return: none
//     effect: executes System Call
// */
// void system_call(){
//     printf("System Call");
//     cli();
//     while(1){}
// }

