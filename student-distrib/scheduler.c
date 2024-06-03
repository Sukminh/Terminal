#include "scheduler.h"
#include "syscall.h"
#include "keyboard.h"

/* Refered Source */
/* https://wiki.osdev.org/Programmable_Interval_Timer#Channel_0 */
/* http://www.osdever.net/bkerndev/Docs/pit.htm */

// set cur_pid to 3; 0, 1, 2 set for base shells of three terminals 
int cur_pid = 3;
int counter = 0;
int terminal_match_flag = 0;
// 3 because first shell is opened in kernel.c and two open in pit_handler 
int start_flag = 3;
int terminal_idx = 0;
// int pit_counter = 2;

/* pit_init
 * Initialize PIT
 * Inputs: None
 * Outputs: None
 * Side Effects: Set the default frequency, enable interrupts
 */
void pit_init() {
    // divisor value we want to write to the Data register is a 16-bit value, 
    // so we will need to transfer both the MSB (Most Significant Byte) and LSB (Least Significant Byte) to the data register
    int divisor = INPUT_CLOCK / HZ;         /* Calculate our divisor */

    // first select the channel that want to update using the command register before writing to the data/divisor register
    // Bit definitions for 8253 and 8254 chip's Command Register located at 0x43
    // outb(data, port)
    outb(CMD_BYTE, CMD_REG);                /* Set our command byte 0x36 */
 
    // 0xFF to get least 8 bit, 0xFF to get first 8bit then shift by 8 so that they go to least bit
	outb(divisor&0xFF, CHANNEL_0);		    /* Set low byte of divisor */
	outb((divisor&0xFF00)>>8, CHANNEL_0);	/* Set high byte of divisor */

    // initialize terminal struct flag to 0 for each terminal 0, 1, 2
    terminal_arr[0].flag = 0;
    terminal_arr[1].flag = 0;
    terminal_arr[2].flag = 0;

    // initialize parent_pid for each terminal 0, 1, 2
    terminal_arr[0].parent_pid = 0;
    terminal_arr[1].parent_pid = 1;
    terminal_arr[2].parent_pid = 2;

    // initialize youngest_pid for each temrinal 0, 1, 2
    terminal_arr[0].youngest_pid = 0;
    terminal_arr[1].youngest_pid = 1;
    terminal_arr[2].youngest_pid = 2;

    // enable IRQ0 (PIT_IRQ) on PIC
    enable_irq(PIT_IRQ);
}

/* pit_handler
 * Handle PIT interrupts
 * Inputs: None
 * Outputs: None
 * Side Effects: Handles rtc by decrementing curr_count until it reaches zero.
 * Set rtc_flag to 1 and curr_count to max_count when the count reach.
 */
//notice: if register C is not read after an IRQ 8, then the interrupt will not happen again. 
void pit_handler(){
    cli();
    send_eoi(PIT_IRQ);

    task_video_mapping(0, 1);

    pcb_t* cur_pcb = get_pcb(terminal_arr[terminal_idx].youngest_pid);
    // store esp value of parent into pcb->esp
    asm("movl %%esp, %0" : "=r"(cur_pcb->esp));
    // store ebp value of parent into pcb->ebp
    asm("movl %%ebp, %0" : "=r"(cur_pcb->ebp));

    // check if start_flag is 3
    if(start_flag == 3){
        start_flag --;
        // terminal_idx + 1 because it starts at index 0; %3 for terminals 0, 1, 2
        // modulo by 3 because we have three terminals
        terminal_idx = (terminal_idx + 1) % 3;
        // switch to terminal 1
        keyboard_switch_buffer(1);
        terminal_switch_buffer(1);
        terminal_switch(1);
        clear();
        // check terminal 1 flag to open shell 
         if(terminal_arr[1].flag == 0) {
            terminal_arr[1].flag = 1;
            system_execute((const uint8_t*)"shell");
        }
    }
    // check if start_flag is 2
    else if (start_flag == 2) {
        start_flag --;
        // terminal_idx + 1 because it starts at index 0; %3 for terminals 0, 1, 2
        // modulo by 3 because we have three terminals
        terminal_idx = (terminal_idx + 1) % 3;
        // switch to terminal 2
        keyboard_switch_buffer(2);
        terminal_switch_buffer(2);
        terminal_switch(2);
        clear();
        // check terminal 2 flag to open shell 
        if(terminal_arr[2].flag == 0) {
            terminal_arr[2].flag = 1;
            system_execute((const uint8_t*)"shell");
        }
    }
    // check if start_flag is 1
    else if (start_flag == 1) {
        start_flag --;
        // swtich to terminal 0
        keyboard_switch_buffer(0);
        terminal_switch_buffer(0);
        terminal_switch(0);
    }

    // check if flag_alt1 is set in keyboard.c (if F1 is pressed)
    if(flag_alt1 == 1){
        flag_alt1 = 0;
        // switch to terminal 0
        keyboard_switch_buffer(0);
        terminal_switch_buffer(0);
        terminal_switch(0);
    }

    // check if flag_alt2 is set in keyboard.c (if F2 is pressed)
    if(flag_alt2 == 1){
        flag_alt2 = 0;
        // switch to temrinal 1
        keyboard_switch_buffer(1);
        terminal_switch_buffer(1);
        terminal_switch(1);
    }

    // check if flag_alt3 is set in keyboard.c (if F3 is pressed)
    if(flag_alt3 == 1){
        flag_alt3 = 0;
        // switch to terminal 2
        keyboard_switch_buffer(2);
        terminal_switch_buffer(2);
        terminal_switch(2);
    }
    
    // call schedule (w PIT implementation)
    schedule();
}

/* void schedule()
 * Handle scheduling 
 * Inputs: None
 * Outputs: None
 * Keep track of tasks and schedule timer interrupt to switch to next task in round-robin 
 */
void schedule(){
    // get cur pcb of currently scheduled task
    pcb_t* cur_pcb = get_pcb(terminal_arr[terminal_idx].youngest_pid);

    // init next pcb var.
    pcb_t* next_pcb;
    
    // save current stack of the terminal
    // store esp value of parent into pcb_esp 
    asm("movl %%esp, %0" : "=r"(cur_pcb->esp));
    // store ebp value of parent into pcb->ebp
    asm("movl %%ebp, %0" : "=r"(cur_pcb->ebp));

    // terminal_idx + 1 because it starts at index 0; %3 for terminals 0, 1, 2
    // modulo by 3 because we have three terminals
    terminal_idx = (terminal_idx + 1) % 3;
   
    // map process: 128 virtual ->pcb process 
    pageSetup(terminal_arr[terminal_idx].youngest_pid);

    // map process video: if(curr_idx == terminal_idx){pt[0xb8].addr = 0xB8}
    // else if(terminal_idx == 0) pt[0xb8].addr = 0xB9} (save for T1, T2, T3)
    if(terminal_idx == cur_terminal_num){
        task_video_mapping(0, 1);
        task_vidmap(0, 1);
    }
    else {
        task_video_mapping(terminal_idx, 0);
        task_vidmap(terminal_idx, 0);
    }

    // change stack (save esp and ebp of leaving pcb and put saved esp and ebp from next_scheduled into stack)
    next_pcb = get_pcb(terminal_arr[terminal_idx].youngest_pid);

    // update tss
    // subtract by 4 so that it provides right above so that bottom of the stack
    tss.esp0 = EIGHT_MB - EIGHT_KB_SIZE * terminal_arr[terminal_idx].youngest_pid - 4;
    tss.ss0 = KERNEL_DS;

    // restore next scheduled task's esp, ebp
    asm volatile ("movl %0, %%esp       \n\
                movl %1, %%ebp          \n\
                " 
                : 
                : "r"(next_pcb->esp), "r"(next_pcb->ebp)
                );
    
    // flush tlp to remap
    flush_TLB();
}

