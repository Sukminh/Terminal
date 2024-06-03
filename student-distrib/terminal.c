#include "terminal.h"

static int8_t char_buf[3][BUF_SIZE] = {{0}};  // character buffer used for storing keyboard buffer
static int enter_flag = 0;          // terminal_arr[cur_terminal_num].enter_flag = 1 if enter is pressed
static int num_chars = 0;           // number of chars in char_buf
int cur_terminal_num = 0;

static int num_chars1 = 0;
static int num_chars2 = 0;
static int num_chars3 = 0;

/* terminal_open
 * open terminal driver
 * Inputs: filename
 * Outputs: 0
 * Side Effects: none
 */
int terminal_open(const uint8_t* filename)
{
    return 0;
}

/* terminal_close
 * close terminal
 * Inputs: fd - file descriptor
 * Outputs: 0
 * Side Effects: none
 */
int terminal_close(int32_t fd)
{
    return 0;
}

/* terminal_read
 * Description: reads data from keyboard buffer, block interrupts until enter key is pressed
 * Inputs: fd - file descriptor
 *         buf - pointer to buffer where data will be stored
 *         n_bytes - max bytes to read
 * Outputs: bytes read (success), or -1 (failure)
 */
int terminal_read(int32_t fd, void* buf, int32_t n_bytes)
{
    if (buf == NULL)
    {
        return -1;
    }
    int i;
    int bytes_read;
    enter_flag = 0;         // initialize terminal_arr[cur_terminal_num].enter_flag to zero
    terminal_arr[cur_terminal_num].enter_flag = 0;

    globalKeyboardFlag[terminal_idx] = 1;

    while((!terminal_arr[cur_terminal_num].enter_flag) || (cur_terminal_num != terminal_idx)) {}   // block until enter key is pressed (terminal_arr[cur_terminal_num].enter_flag = 1)
    cli();
    terminal_arr[cur_terminal_num].enter_flag = 0;
    bytes_read = 0;

    // use 3 sixed buffer to save which terminals are locked
    
    // if n_bytes (input) is less than BUF_SIZE, loop until n_bytes-1
    // otherwise, loop until BUF_SIZE
    if (n_bytes < BUF_SIZE)
    {
        for (i = 0; i < n_bytes; i++)
        {
            *((int8_t*)buf+i) = char_buf[cur_terminal_num][i];     // store current byte into read buffer
            char_buf[cur_terminal_num][i] = 0x00;                 // clear char_buf
            bytes_read++;                       //increment counter
        }
    } else 
    {
        for (i = 0; i < BUF_SIZE; i++)
        {
            *((int8_t*)buf+i) = char_buf[cur_terminal_num][i];    // store current byte into read buffer
            char_buf[cur_terminal_num][i] = 0x00;                 // clear char_buf
            bytes_read++;                       //increment counter
        }
    }
    for (i = 0; i < BUF_SIZE; i++)
    {
        char_buf[cur_terminal_num][i] = '\0';
    }
    enter_flag = 0;     //  set terminal_arr[cur_terminal_num].enter_flag back to zero

    globalKeyboardFlag[terminal_idx] = 0;

    sti();  
    return bytes_read;
}

/* terminal_write
 * Description: writes data from buffer to temrinal
 * Inputs: fd - file descriptor
 *         buf - pointer to buffer where data will be stored
 *         n_bytes - max bytes to read
 * Outputs: bytes read (success), or -1 (failure)
 * Side Effects: none
 */
int terminal_write(int32_t fd, const void* buf, int32_t n_bytes){
    cli();
    if((buf == NULL) || (n_bytes<0)){
        return -1;
    }
    int counter;
    int i;
    char c;
    for (i = 0; i < n_bytes; i++){
        c = *((int8_t*)buf+i);  // iterate through read buffer to character c
        // do not print NULL terminator
        if (c != '\0'){
            // int a = terminal_idx;
            // print current byte
            putc(c);
            // increment counter
            counter++;
        }
    
    }
    sti();
    return counter;

    // int count_bytes;    // counter of bytes that are copied
    // int i;
    // char c;

    // count_bytes = 0;

    // // parameter validation
    // if (buf == NULL)
    // {
    //     return -1;
    // }
    // cli();

    // // iterate over argument buffer
    // for (i = 0; i < n_bytes; i++)
    // {
    //     c = *((int8_t*)buf+i);  // iterate through read buffer to character c
    //     // do not print NULL terminator
    //     if (c != '\0')
    //     {
    //         // print current byte
    //         putc(c);
    //         // increment counter
    //         count_bytes++;
    //     }
    // }
    // sti();
    // // return byte successfully copied
    // return count_bytes;
}

/* keyboard_input
 * Description: called whenever the keyboard handler prints character, 
 *              set store keyboard inputs to char_buf and update terminal_arr[cur_terminal_num].enter_flag and num_chars
 * Inputs: new_char - character that is pressed in keyboard
 * Outputs: none
 * Side Effects: update char_buf, num_chars, and terminal_arr[cur_terminal_num].enter_flag, let terminal_read to fill out buf based on char_buf
 */
void keyboard_input(uint8_t new_char) {
    cli();
    // if enter is pressed
    if (new_char == '\n') {
        num_chars = 0;      // clear char_buf so number of chars in char_buf is zero
        enter_flag = 1;     // let terminal_read to fill out buf based on char_buf
        terminal_arr[cur_terminal_num].enter_flag = 1;
    }
    // if backspace is pressed
    else if(new_char == '\b') {
        if (num_chars > 0 && num_chars <= BUF_SIZE) {
            char_buf[cur_terminal_num][num_chars-1] = 0x00;       // clear last element in char_buf
            num_chars --;                       // decrement num_chars by 1
        }
    }
    // if other character is pressed
    else {
        if (num_chars < BUF_SIZE) {
            char_buf[cur_terminal_num][num_chars] = new_char;     // append that char to char_buf
            num_chars ++;                       // increment num_chars by 1
        }
    }
}

/* void terminal_switch_buffer(int index)
 * Description: keep track of number of characters typed for each terminal
 * Inputs: int index
 * Outputs: none
 */
void terminal_switch_buffer(int index) {
    if (cur_terminal_num == 0) {
        num_chars1 = num_chars;
    }
    else if (cur_terminal_num == 1) {
        num_chars2 = num_chars;
    }
    else {
        num_chars3 = num_chars;
    }
    // cur_terminal_num = index;
    if (index == 0) {
        num_chars = num_chars1;
    }
    else if (index == 1) {
        num_chars = num_chars2;
    }
    else {
        num_chars = num_chars3;
    }
}
