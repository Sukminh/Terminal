// Checkpoint 1 New File
#include "rtc.h"
#include "lib.h"


int max_count;      // initial value of curr_count
// 3 -> refers three terminals in our OS
char rtc_flag[3] = {0, 0, 0};      // flag to indicate when to read and write
int curr_count[3] = {MAX_FREQ / MIN_FREQ, MAX_FREQ / MIN_FREQ, MAX_FREQ / MIN_FREQ};       // current counter 
int curr_freq[3] = {0, 0, 0};

/* rtc_init
 * Initialize RTC
 * Inputs: None
 * Outputs: None
 * Side Effects: Set the default frequency, enable interrupts
 */
void rtc_init(){
    max_count = 0;
    char prev;
    // select register B, and disable NMI
    outb(REG_B, RTC_PORT);

    // read the current value of register B
    prev = inb(RTC_DATA);

    // set the index again (a read will reset the index to register D)
    outb(REG_B, RTC_PORT);

    // write the previous value ORed with 0x40. This turns on bit 6(0x40) of register B
    outb(prev | 0x40, RTC_DATA);

    // //virtualization
    char rate = 6;//set real frequency to 1024Hz(rate = 6)
 
    rate &= 0x0F;    // rate must be above 2 and not over 15 

    cli();
    // set index to register A, disable NMI
    outb(REG_A, RTC_PORT);
    // get initial value of register A
    prev = inb(RTC_DATA);
    // reset index to A
    outb(REG_A, RTC_PORT);
    //write only our rate to A. Note, rate is the bottom 4 bits.
    outb((prev & 0xF0) | rate, RTC_DATA);

    //enable irq8(RTC irq) on pic
    enable_irq(RTC_IRQ);

    sti();


}


/* rtc_set_freq
 * Set virtual frequency of the RTC to the arg value
 * Inputs: freq -  the virtual frequency we want to set for RTC
 * Outputs: 0 - set freq sucessfully; -1 - fail to set the freq
 * Side Effects: No thing happen if freq is out of range(2 - 1024).
 */
int32_t rtc_set_freq(int32_t freq){
    cli();
    pcb_t* curr_pcb = get_pcb(pid);
    //check if input freq is in range(2-1024)
    if ((freq < MIN_FREQ) || (freq > MAX_FREQ)) return -1;

    //set initial freq
    char log2_freq = log2(freq);// rate of this frequency(2(2) - 10(1024))

    //check input freq is the power of 2
    if (log2_freq == -1) return -1;

    max_count = MAX_FREQ / (freq * 2); //virtual rate = 1024Hz / freq(Hz), multiply by 2 to get the correct speed
    curr_pcb->rtc_val =  MAX_FREQ / (freq); //virtual rate = 1024Hz / freq(Hz)
;

    if (freq == 2){             //default case for initial frequency of 2HZ
        max_count = 512;        //max count = maxFreq/minFreq = 1024/2
    }

    sti();
    return 0;
}

/* rtc_handle
 * Handle RTC interrupts
 * Inputs: None
 * Outputs: None
 * Side Effects: Handles rtc by decrementing curr_count until it reaches zero.
 *  Set rtc_flag to 1 and curr_count to max_count when the count reach.
 */
//notice: if register C is not read after an IRQ 8, then the interrupt will not happen again. 
void rtc_handle(){
    //test_interrupts(); 
    outb(REG_C, RTC_PORT);	// select register C
    inb(RTC_DATA);		// just throw away contents
    // terminal idx 0, 1, 2
    curr_count[0] --;      // keep decrementing curr_count
    curr_count[1] --;      // keep decrementing curr_count
    curr_count[2] --;      // keep decrementing curr_count
    // when curr_count reaches zero, set  to 1 and curr_count back to max_count
    // terminal idx 0, 1, 2
    if (curr_count[0] <= 0) {  
        rtc_flag[0] = 1;
        curr_count[0] = max_count;
    }

    if (curr_count[1] <= 0) {  
        rtc_flag[1] = 1;
        curr_count[1] = max_count;
    }

    if (curr_count[2] <= 0) {  
        rtc_flag[2] = 1;
        curr_count[2] = max_count;
    }
    send_eoi(RTC_IRQ);
}

/* rtc_open
 * Initialize RTC freq to 2Hz
 * Inputs: filename - file name
 * Outputs: 0 - sucess
 */
int32_t rtc_open(const uint8_t* filename){
    //rtc_init();
    // rtc_set_freq(MIN_FREQ);
    return 0;
}

/* rtc_close
 * Do noting
 * Inputs: fd - file descriptor number
 * Outputs: 0 - sucess
 */
int32_t rtc_close(int32_t fd){
    //rtc_set_freq(MIN_FREQ);
    return 0;
}

/* rtc_read
 * Block until the next interrupt (until rtc_flag is changed to 1)
 * Inputs: fd - file descriptor number
 *         buf - pointer to value of frequency store in the buffer
 *         nbytes - number of bytes of the freq value(shoudle be 4 because it is int)
 * Outputs: 0 - sucess
 */
int32_t rtc_read(int32_t fd, void* buf, int32_t nbytes){
    rtc_flag[terminal_idx] = 0;
    while(!rtc_flag[terminal_idx]){}
    return 0;
}

/* rtc_write
 * Change rtc frequency, the input frequency must be the power of 2 and within the range of 2 - 1024
 * Inputs: fd - file descriptor number
 *         buf - pointer to value of frequency store in the buffer
 *         nbytes - number of bytes of the freq value (should be 4 because it is int)
 * Outputs: 0 - success; -1 - fail
 */
int32_t rtc_write(int32_t fd, const void* buf, int32_t nbytes){
    //check if input args are valid
    if ((buf == NULL)||(nbytes != 4)){      //check if nbytes is not equal to 4
        return -1;
    }

    int freq;
    
    freq = *((int*)buf); //fetch frequency value in the buffer
    
    curr_freq[terminal_idx] = freq;

    //if return val of set_freq == -1 means set freq fail power of 2
    if (rtc_set_freq(freq/2) == -1){
        return -1;
    }
    curr_count[terminal_idx] = max_count;
    return 0;
}

/* log2(num)
 * Inputs: num - frequency of the RTC
 * Outputs: count - log2(num); -1 - this number is not the power of 2
 * Side Effect: calculate log2(num)
 */
char log2(int32_t num) {
    char count = 0;
    while(num != 1){
        if (num % 2 != 0) return -1; //this number is not the power of 2         
        num /= 2;
        count++;
    }
    return count;
}

// #include "rtc.h"
// #include "lib.h"

// int counterRTC = MAX_FREQ/MIN_FREQ;
// int rtcFlag = 0;       // flag to indicate when to read and write
// char prev;
// int max_count;      // initial value of curr_count


// /*
// * rtc_init()
// * Initialize RTC to a specified rate and enable interrupts
// * Inputs: None
// * Outputs: None
// * Side Effects: Set default frequency and enable interrupts
// */
// void rtc_init(){
//     // with large help from osdev.org
//     max_count = 0;


//     outb(REG_B, RTC_PORT);          //select Status Register B, and disable NMI
//     prev = inb(CMOS_PORT);          // read the current value of register B
//     outb(REG_B, RTC_PORT);          // set the index again (a read will reset the index to register D)
//     outb(prev | 0x40, CMOS_PORT);   // write the previous value ORed with 0x40. This turns on bit 6 of register B
//     enable_irq(0x08);               //RTC IRQ is 0x08
//     //set_rtc(32);
// }

// /*
// * rtc_handler
// * Handle RTC Interrupts
// * Inputs: None
// * Outputs: None
// * Side Effects: handle rtc by decrementing the counter to 0, and also reset counter
// */
// void rtc_handler(){
//     //test_interrupts(); 
//     outb(REG_C, RTC_PORT);	// select register C
//     inb(CMOS_PORT);		// just throw away contents
//     counterRTC --;      
//     if (counterRTC <= 0) {  
//         rtcFlag = 1;
//         counterRTC = max_count;
//     }

//     send_eoi(0x08);

// }

// /* rtc_open
//  * Initialize RTC freq to 2Hz
//  * Inputs: filename - file name
//  * Outputs: 0 - sucess
//  */
// int32_t rtc_open(const uint8_t* filename){
//     //rtc_init();
//     //set_rtc(2);
//     return 0;
// }

// /* rtc_close
//  * Do nothing
//  * Inputs: fd - file descriptor number
//  * Outputs: 0 - success
//  */
// int32_t rtc_close(int32_t fd){
//     return 0;
// }


// /* rtc_read
//  * Wait for interrupt from RTC, clears flag, returns 0
//  * Inputs: fd - file descriptor number
//  *         buf - pointer to buffer
//  *         nbytes - number of bytes to read
//  * Outputs: 0 - success
//  */
// int32_t rtc_read(int32_t fd, void* buf, int32_t nbytes){
//     rtcFlag = 0;
//     while(!rtcFlag){}
//     return 0;
// }

// /*
// * rtc_write(int32_t fd, const void* buf, int32_t nbytes)
// * writes data to the RTC and changes rtc frequency
// * Inputs: fd - file descriptor
// *         buf - pointer to buffer
// *         nbytes - number of bytes to read
// * Output: 0 - success, -1 - failure
// */
// int32_t rtc_write(int32_t fd, const void* buf, int32_t nbytes){
//     if ((buf == NULL) || (nybtes != 4)){
//         return -1;
//     }

//     int freq;
//     freq = *((int*)buf);
//     if ((freq < MIN_FREQ) || (freq > MAX_FREQ)) return -1;
//     if ((freq & (freq-1)) != 0){
//         //power of two check
//         return -1;
//     }
//     //maybe need this:
//     set_rtc(freq/10); 
//     counterRTC = max_count;


//     return 0;


// }


// /*
// * set_rtc(int32_t freq)
// * set the rtc to the desired frequency
// * Inputs: freq - desired RTC interrupt frequency
// * Output: None
// * Side Effect: Modifies RTC interrupt frequency
// */
// void set_rtc(int32_t freq){
//     if ((freq < MIN_FREQ) || (freq > MAX_FREQ)) return;
//     char rate = 15 - log2(freq) + 1;
//     rate &= 0x0F;
//     if(rate<3) return;
//     cli();
//     outb(REG_A, RTC_PORT);
//     prev = inb(CMOS_PORT);
//     outb(REG_A, RTC_PORT);
//     outb((prev & 0xF0) | rate, CMOS_PORT);
//     max_count = MAX_FREQ / (freq * 2); 
//     if (freq == 2){             //default case for initial frequency of 2HZ
//         max_count = 512;        //max count = maxFreq/minFreq = 1024/2
//     }

//     sti();

// }


// /* log2(int32_t num)
//  * Inputs: num - RTC frequency
//  * Outputs: count - log2(num)
//  * Side Effect: perform the log2 operation on the number provided
//  */
// char log2(int32_t num) {
//     char count = 0;
//     while(num != 1){
//         num /= 2;
//         count++;
//     }
//     return count;
// }

