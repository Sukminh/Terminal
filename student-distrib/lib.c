/* lib.c - Some basic library functions (printf, strlen, etc.)
 * vim:ts=4 noexpandtab */

#include "lib.h"

#define VIDEO       0xB8000             //starting address of video memroy 
#define VIDEO1      0xB9000             //starting address of video memroy 
#define VIDEO2      0xBA000             //starting address of video memroy 
#define VIDEO3      0xBB000             //starting address of video memroy 

#define NUM_COLS    80                  //number of columns
#define NUM_ROWS    25                  //number of rows
#define ATTRIB      0x7                 //ATTRIB val
#define FOUR_KB     4096                //4kb in byte is 4096

// static int cursorx[cur_terminal_num];
// static int cursory[cur_terminal_num];

int flag_terminal = 0;
int cursorx[3] = {0, 0, 0};
int cursory[3] = {0, 0, 0};
int prev_flag_buffer[3] = {0};
// int prev_flag;      // flag used for indicating if backspace should go to previous line or not
static char* video_mem = (char *)VIDEO;
// char* video_mem = (char *)VIDEO;
// int cur_terminal_num = 1;

int prev_terminal = 0;

/* void terminal_switch(int index);
 * Inputs: index
 * Return Value: none
 * Function: switch terminal based on index  */
void terminal_switch(int index){
    // suppose we switch from current_terminal to target_terminal
    // other stuff

    // update_video_memory_paging(current_terminal)
    // Copy from video memory to background buffer of current_terminal
    // Load background buffer of target_terminal into video memory 
    // update_video_memory_paging(get_owner_terminal(current_pid))
    
    // other stuff
    // cursorx[cur_terminal_num] = cursorx[cur_terminal_num];
    // cursory[cur_terminal_num] = cursory[cur_terminal_num];

    // one to one video mapping (b8 to b8 mapping); cur_terminal_num + 1 because it is zero-indexed
    memcpy((uint8_t*)(VIDEO + FOUR_KB * (cur_terminal_num+1)), (uint8_t*)VIDEO, FOUR_KB);
    // clear();
    // check if index is 0 and previous terminal is not equal to index
    if (index == 0 && (prev_terminal != index)){
        // video_mem = (char*)VIDEO;
        prev_terminal = 0;
        cur_terminal_num = 0;
        // map b8000 + 4kb to video memory
        memcpy((uint8_t*)VIDEO, (uint8_t*)(VIDEO + FOUR_KB * (index+1)), FOUR_KB);
    }
    // check if index is 1 and previous terminal is not equal to index
    else if (index == 1 && (prev_terminal != index)){
        // video_mem = (char*)VIDEO2;
        prev_terminal = 1;
        cur_terminal_num = 1;
        // map b8000 + 8kb to video memory
        memcpy((uint8_t*)VIDEO, (uint8_t*)(VIDEO + FOUR_KB * (index+1)), FOUR_KB);
    }
    // check if index is 2 and previous terminal is not equal to index
    else if (index == 2 && (prev_terminal != index)){
        // video_mem = (char*)VIDEO3;
        prev_terminal = 2;                  // set pre_terminal to 2 (variable to store current terminal number for the next terminal switch)
        cur_terminal_num = 2;               // looking at terminal 2
        // map b8000 + 12kb to video memory
        memcpy((uint8_t*)VIDEO, (uint8_t*)(VIDEO + FOUR_KB * (index+1)), FOUR_KB);
    }

    // cursorx[cur_terminal_num] = cursorx[cur_terminal_num];
    // cursory[cur_terminal_num] = cursory[cur_terminal_num];
    update_cursor(cursorx[cur_terminal_num], cursory[cur_terminal_num]);

    // update_video_memory_paging(current_terminal)
    // pageSetup(terminal_arr[cur_terminal_num-1].youngest_pid);
}



/* void clear(void);
 * Inputs: void
 * Return Value: none
 * Function: Clears video memory */
void clear(void) {
    //map one to one
    pageTable[184].page_addr = 184;
    
    // clear entire screen
    int32_t i;
    for (i = 0; i < NUM_ROWS * NUM_COLS; i++) {
        *(uint8_t *)(video_mem + (i << 1)) = 0x00;
        *(uint8_t *)(video_mem + (i << 1) + 1) = ATTRIB;
    }
    // set cursor to top left corner
    cursorx[cur_terminal_num] = 0;
    cursory[cur_terminal_num] = 0;
    // update cursor
    update_cursor(cursorx[cur_terminal_num], cursory[cur_terminal_num]);
    // for (i = 0; i < ((NUM_ROWS/2) * NUM_COLS); i++) {
    //     if (
    //         (i < 80 && i > 70) || 
    //         (i < 160 && i > 150) ||
    //         (i < 240 && i > 230)
    //         ){
    //         *(uint8_t *)(video_mem + (i << 1)) = ' ';
    //         *(uint8_t *)(video_mem + (i << 1) + 1) = 0xEE;
    //     }
    //     else if (
    //         (i == 134 || i == 215 || i == 296 || i == 217 || i == 138) /*i > 70) || 
    //         (i < 160 && i > 150) ||
    //         (i < 240 && i > 230)*/
    //         ){
    //         *(uint8_t *)(video_mem + (i << 1)) = ' ';
    //         *(uint8_t *)(video_mem + (i << 1) + 1) = 0x00;
    //     }else{
    //         *(uint8_t *)(video_mem + (i << 1)) = ' ';
    //         *(uint8_t *)(video_mem + (i << 1) + 1) = 0xBB;
    //     }
        
    // }

    // for (i = ((NUM_ROWS/2) * NUM_COLS); i < (NUM_ROWS * NUM_COLS); i++) {
    //     *(uint8_t *)(video_mem + (i << 1)) = ' ';
    //     *(uint8_t *)(video_mem + (i << 1) + 1) = 0x22;
    // }
    
}

/* Standard printf().
 * Only supports the following format strings:
 * %%  - print a literal '%' character
 * %x  - print a number in hexadecimal
 * %u  - print a number as an unsigned integer
 * %d  - print a number as a signed integer
 * %c  - print a character
 * %s  - print a string
 * %#x - print a number in 32-bit aligned hexadecimal, i.e.
 *       print 8 hexadecimal digits, zero-padded on the left.
 *       For example, the hex number "E" would be printed as
 *       "0000000E".
 *       Note: This is slightly different than the libc specification
 *       for the "#" modifier (this implementation doesn't add a "0x" at
 *       the beginning), but I think it's more flexible this way.
 *       Also note: %x is the only conversion specifier that can use
 *       the "#" modifier to alter output. */
int32_t printf(int8_t *format, ...) {

    /* Pointer to the format string */
    int8_t* buf = format;

    /* Stack pointer for the other parameters */
    int32_t* esp = (void *)&format;
    esp++;

    while (*buf != '\0') {
        switch (*buf) {
            case '%':
                {
                    int32_t alternate = 0;
                    buf++;

format_char_switch:
                    /* Conversion specifiers */
                    switch (*buf) {
                        /* Print a literal '%' character */
                        case '%':
                            putc('%');
                            break;

                        /* Use alternate formatting */
                        case '#':
                            alternate = 1;
                            buf++;
                            /* Yes, I know gotos are bad.  This is the
                             * most elegant and general way to do this,
                             * IMHO. */
                            goto format_char_switch;

                        /* Print a number in hexadecimal form */
                        case 'x':
                            {
                                int8_t conv_buf[64];
                                if (alternate == 0) {
                                    itoa(*((uint32_t *)esp), conv_buf, 16);
                                    puts(conv_buf);
                                } else {
                                    int32_t starting_index;
                                    int32_t i;
                                    itoa(*((uint32_t *)esp), &conv_buf[8], 16);
                                    i = starting_index = strlen(&conv_buf[8]);
                                    while(i < 8) {
                                        conv_buf[i] = '0';
                                        i++;
                                    }
                                    puts(&conv_buf[starting_index]);
                                }
                                esp++;
                            }
                            break;

                        /* Print a number in unsigned int form */
                        case 'u':
                            {
                                int8_t conv_buf[36];
                                itoa(*((uint32_t *)esp), conv_buf, 10);
                                puts(conv_buf);
                                esp++;
                            }
                            break;

                        /* Print a number in signed int form */
                        case 'd':
                            {
                                int8_t conv_buf[36];
                                int32_t value = *((int32_t *)esp);
                                if(value < 0) {
                                    conv_buf[0] = '-';
                                    itoa(-value, &conv_buf[1], 10);
                                } else {
                                    itoa(value, conv_buf, 10);
                                }
                                puts(conv_buf);
                                esp++;
                            }
                            break;

                        /* Print a single character */
                        case 'c':
                            putc((uint8_t) *((int32_t *)esp));
                            esp++;
                            break;

                        /* Print a NULL-terminated string */
                        case 's':
                            puts(*((int8_t **)esp));
                            esp++;
                            break;

                        default:
                            break;
                    }

                }
                break;

            default:
                putc(*buf);
                break;
        }
        buf++;
    }
    return (buf - format);
}

/* int32_t puts(int8_t* s);
 *   Inputs: int_8* s = pointer to a string of characters
 *   Return Value: Number of bytes written
 *    Function: Output a string to the console */
int32_t puts(int8_t* s) {
    register int32_t index = 0;
    while (s[index] != '\0') {
        putc(s[index]);
        index++;
    }
    return index;
}

/* void putc(uint8_t c);
 * Inputs: uint_8* c = character to print
 * Return Value: void
 *  Function: Output a character to the console for the program running*/
void putc(uint8_t c) {
    // set idx to scheduled process
    int idx = terminal_idx;
    // check if flag_terminal is set to 1
    if (flag_terminal == 1) {
        // set idx to the current terminal number user is looking at 
        idx = cur_terminal_num;
    }

    // if enter key or carriage return key is pressed
    if(c == '\n' || c == '\r') {
        // move screen cursor to next line
        cursory[idx]++;
        cursorx[idx] = 0;
        prev_flag_buffer[cur_terminal_num] = 0;
        
        // scroll down if cursor located in last line wants to go to the next line 
        if (cursory[idx] >= NUM_ROWS) {
            int i = 0;
            for (i = 0; i < (NUM_ROWS-1) * NUM_COLS; i++) {
                *(uint8_t *)(video_mem + (i << 1)) = *(uint8_t *)(video_mem + ((i+NUM_COLS) << 1));
                *(uint8_t *)(video_mem + (i << 1) + 1) = ATTRIB;
            }
            for (i=(NUM_ROWS-1) * NUM_COLS; i < NUM_ROWS * NUM_COLS; i++) {
                *(uint8_t *)(video_mem + (i << 1)) = 0x00;                      //set value to 0 for last line
                *(uint8_t *)(video_mem + (i << 1) + 1) = ATTRIB;
            }
            cursory[idx]--;
        }
    }
    // if backspace key is pressed
    else if (c == '\b'){
        // if cursor is in left edge, do not decrement cursorx[cur_terminal_num]
        if (cursorx[idx] != 0) {
            cursorx[idx]--;
        }
        else{
            // if cursor is in left edge and backspace should go to previous line,
            if (prev_flag_buffer[cur_terminal_num] == 1) {
                cursory[idx]--;
                cursorx[idx] = NUM_COLS-1;
                prev_flag_buffer[cur_terminal_num] = 0;  // set prev_flag back to zero
            }
        }
        // draw it out in terminal
        int i = 0;
        for (i = 0; i < NUM_ROWS * NUM_COLS; i++) {
        *(uint8_t *)(video_mem + ((NUM_COLS * cursory[idx] + cursorx[idx]) << 1)) = 0x00;
        
            *(uint8_t *)(video_mem + ((NUM_COLS * cursory[idx] + cursorx[idx]) << 1) + 1) = ATTRIB;      //i
        }
        cursory[idx] = (cursory[idx] + (cursorx[idx] / NUM_COLS)) % NUM_ROWS;
        cursorx[idx] %= NUM_COLS;
    } 
    else {
        // draw new character to terminal
        int i = 0;
        for (i = 0; i < NUM_ROWS * NUM_COLS; i++) {
            *(uint8_t *)(video_mem + ((NUM_COLS * cursory[idx] + cursorx[idx]) << 1)) = c;
            *(uint8_t *)(video_mem + ((NUM_COLS * cursory[idx] + cursorx[idx]) << 1) + 1) = ATTRIB;     
        }
        
        cursorx[idx]++; // move cursor to the right
        // if cursor reaches right edge, move it to the next line
        if (cursorx[idx] >= NUM_COLS) {
            cursory[idx]++;
            prev_flag_buffer[cur_terminal_num] = 1;  // set prev_flag to one, so backspace can go to previous line
            // do vertical scrolling if cursor hits the last line
            if (cursory[idx] >= NUM_ROWS) {
                for (i = 0; i < (NUM_ROWS-1) * NUM_COLS; i++) {
                    *(uint8_t *)(video_mem + (i << 1)) = *(uint8_t *)(video_mem + ((i+NUM_COLS) << 1));
                    *(uint8_t *)(video_mem + (i << 1) + 1) = ATTRIB;
                }
                for (i=(NUM_ROWS-1) * NUM_COLS; i < NUM_ROWS * NUM_COLS; i++) {
                    *(uint8_t *)(video_mem + (i << 1)) = ' ';
                    *(uint8_t *)(video_mem + (i << 1) + 1) = ATTRIB;
                }
                cursory[idx]--;
            }
        }
        cursorx[idx] %= NUM_COLS;
        cursory[idx] = (cursory[idx] + (cursorx[idx] / NUM_COLS)) % NUM_ROWS;
    }
    // update_cursor(cursorx[cur_terminal_num], cursory[cur_terminal_num]); // update cursor location
    update_cursor(cursorx[cur_terminal_num], cursory[cur_terminal_num]);

    // if (c == '\b'){
    //     cursorx[cur_terminal_num]--;
    //     cursory[cur_terminal_num] = (cursory[cur_terminal_num] + (cursorx[cur_terminal_num] / NUM_COLS)) % NUM_ROWS;
    //     cursorx[cur_terminal_num] %= NUM_COLS;
    //     int i = 0;
    //     for (i = 0; i < NUM_ROWS * NUM_COLS; i++) {
    //     *(uint8_t *)(video_mem + ((NUM_COLS * cursory[cur_terminal_num] + cursorx[cur_terminal_num]) << 1)) = c;
        
    //         *(uint8_t *)(video_mem + ((NUM_COLS * cursory[cur_terminal_num] + cursorx[cur_terminal_num]) << 1) + 1) = i;      //ATTRIB
    //     }
    //     //update_cursor();
    //     //cursorx[cur_terminal_num]--;
    //     //cursory[cur_terminal_num] = (cursory[cur_terminal_num] + (cursorx[cur_terminal_num] / NUM_COLS)) % NUM_ROWS;
    //     //cursorx[cur_terminal_num] %= NUM_COLS;
    // }
}


/* void key_putc(uint8_t c);
 * Inputs: uint_8* c = character to print
 * Return Value: void
 *  Function: Output a character to the console for the keyboard imput*/
void key_putc(uint8_t c) {
    //whatever it is pointing to before
    cli();
    // set flag_terminal to 1 --> allow user to type into terminal being displayed 
    flag_terminal= 1;
    // store current video memory address to temp variable; 184 = address 0xb8000 >> 12
    uint32_t temp = pageTable[184].page_addr;

    //map one to one
    pageTable[184].page_addr = 184;

    //flushtlb
    flush_TLB();

    putc(c);
    // print character to screen and set flag_terminal to 0 --> allow background processes to write to respective terminals
    flag_terminal = 0;
    // map to temp; 184 = address b8000 >> 12
    pageTable[184].page_addr = temp;

    // flushtlb
    flush_TLB();
    sti();
}

/* int8_t* itoa(uint32_t value, int8_t* buf, int32_t radix);
 * Inputs: uint32_t value = number to convert
 *            int8_t* buf = allocated buffer to place string in
 *          int32_t radix = base system. hex, oct, dec, etc.
 * Return Value: number of bytes written
 * Function: Convert a number to its ASCII representation, with base "radix" */
int8_t* itoa(uint32_t value, int8_t* buf, int32_t radix) {
    static int8_t lookup[] = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ";
    int8_t *newbuf = buf;
    int32_t i;
    uint32_t newval = value;

    /* Special case for zero */
    if (value == 0) {
        buf[0] = '0';
        buf[1] = '\0';
        return buf;
    }

    /* Go through the number one place value at a time, and add the
     * correct digit to "newbuf".  We actually add characters to the
     * ASCII string from lowest place value to highest, which is the
     * opposite of how the number should be printed.  We'll reverse the
     * characters later. */
    while (newval > 0) {
        i = newval % radix;
        *newbuf = lookup[i];
        newbuf++;
        newval /= radix;
    }

    /* Add a terminating NULL */
    *newbuf = '\0';

    /* Reverse the string and return */
    return strrev(buf);
}

/* int8_t* strrev(int8_t* s);
 * Inputs: int8_t* s = string to reverse
 * Return Value: reversed string
 * Function: reverses a string s */
int8_t* strrev(int8_t* s) {
    register int8_t tmp;
    register int32_t beg = 0;
    register int32_t end = strlen(s) - 1;

    while (beg < end) {
        tmp = s[end];
        s[end] = s[beg];
        s[beg] = tmp;
        beg++;
        end--;
    }
    return s;
}

/* uint32_t strlen(const int8_t* s);
 * Inputs: const int8_t* s = string to take length of
 * Return Value: length of string s
 * Function: return length of string s */
uint32_t strlen(const int8_t* s) {
    register uint32_t len = 0;
    while (s[len] != '\0')
        len++;
    return len;
}

/* void* memset(void* s, int32_t c, uint32_t n);
 * Inputs:    void* s = pointer to memory
 *          int32_t c = value to set memory to
 *         uint32_t n = number of bytes to set
 * Return Value: new string
 * Function: set n consecutive bytes of pointer s to value c */
void* memset(void* s, int32_t c, uint32_t n) {
    c &= 0xFF;
    asm volatile ("                 \n\
            .memset_top:            \n\
            testl   %%ecx, %%ecx    \n\
            jz      .memset_done    \n\
            testl   $0x3, %%edi     \n\
            jz      .memset_aligned \n\
            movb    %%al, (%%edi)   \n\
            addl    $1, %%edi       \n\
            subl    $1, %%ecx       \n\
            jmp     .memset_top     \n\
            .memset_aligned:        \n\
            movw    %%ds, %%dx      \n\
            movw    %%dx, %%es      \n\
            movl    %%ecx, %%edx    \n\
            shrl    $2, %%ecx       \n\
            andl    $0x3, %%edx     \n\
            cld                     \n\
            rep     stosl           \n\
            .memset_bottom:         \n\
            testl   %%edx, %%edx    \n\
            jz      .memset_done    \n\
            movb    %%al, (%%edi)   \n\
            addl    $1, %%edi       \n\
            subl    $1, %%edx       \n\
            jmp     .memset_bottom  \n\
            .memset_done:           \n\
            "
            :
            : "a"(c << 24 | c << 16 | c << 8 | c), "D"(s), "c"(n)
            : "edx", "memory", "cc"
    );
    return s;
}

/* void* memset_word(void* s, int32_t c, uint32_t n);
 * Description: Optimized memset_word
 * Inputs:    void* s = pointer to memory
 *          int32_t c = value to set memory to
 *         uint32_t n = number of bytes to set
 * Return Value: new string
 * Function: set lower 16 bits of n consecutive memory locations of pointer s to value c */
void* memset_word(void* s, int32_t c, uint32_t n) {
    asm volatile ("                 \n\
            movw    %%ds, %%dx      \n\
            movw    %%dx, %%es      \n\
            cld                     \n\
            rep     stosw           \n\
            "
            :
            : "a"(c), "D"(s), "c"(n)
            : "edx", "memory", "cc"
    );
    return s;
}

/* void* memset_dword(void* s, int32_t c, uint32_t n);
 * Inputs:    void* s = pointer to memory
 *          int32_t c = value to set memory to
 *         uint32_t n = number of bytes to set
 * Return Value: new string
 * Function: set n consecutive memory locations of pointer s to value c */
void* memset_dword(void* s, int32_t c, uint32_t n) {
    asm volatile ("                 \n\
            movw    %%ds, %%dx      \n\
            movw    %%dx, %%es      \n\
            cld                     \n\
            rep     stosl           \n\
            "
            :
            : "a"(c), "D"(s), "c"(n)
            : "edx", "memory", "cc"
    );
    return s;
}

/* void* memcpy(void* dest, const void* src, uint32_t n);
 * Inputs:      void* dest = destination of copy
 *         const void* src = source of copy
 *              uint32_t n = number of byets to copy
 * Return Value: pointer to dest
 * Function: copy n bytes of src to dest */
void* memcpy(void* dest, const void* src, uint32_t n) {
    asm volatile ("                 \n\
            .memcpy_top:            \n\
            testl   %%ecx, %%ecx    \n\
            jz      .memcpy_done    \n\
            testl   $0x3, %%edi     \n\
            jz      .memcpy_aligned \n\
            movb    (%%esi), %%al   \n\
            movb    %%al, (%%edi)   \n\
            addl    $1, %%edi       \n\
            addl    $1, %%esi       \n\
            subl    $1, %%ecx       \n\
            jmp     .memcpy_top     \n\
            .memcpy_aligned:        \n\
            movw    %%ds, %%dx      \n\
            movw    %%dx, %%es      \n\
            movl    %%ecx, %%edx    \n\
            shrl    $2, %%ecx       \n\
            andl    $0x3, %%edx     \n\
            cld                     \n\
            rep     movsl           \n\
            .memcpy_bottom:         \n\
            testl   %%edx, %%edx    \n\
            jz      .memcpy_done    \n\
            movb    (%%esi), %%al   \n\
            movb    %%al, (%%edi)   \n\
            addl    $1, %%edi       \n\
            addl    $1, %%esi       \n\
            subl    $1, %%edx       \n\
            jmp     .memcpy_bottom  \n\
            .memcpy_done:           \n\
            "
            :
            : "S"(src), "D"(dest), "c"(n)
            : "eax", "edx", "memory", "cc"
    );
    return dest;
}

/* void* memmove(void* dest, const void* src, uint32_t n);
 * Description: Optimized memmove (used for overlapping memory areas)
 * Inputs:      void* dest = destination of move
 *         const void* src = source of move
 *              uint32_t n = number of byets to move
 * Return Value: pointer to dest
 * Function: move n bytes of src to dest */
void* memmove(void* dest, const void* src, uint32_t n) {
    asm volatile ("                             \n\
            movw    %%ds, %%dx                  \n\
            movw    %%dx, %%es                  \n\
            cld                                 \n\
            cmp     %%edi, %%esi                \n\
            jae     .memmove_go                 \n\
            leal    -1(%%esi, %%ecx), %%esi     \n\
            leal    -1(%%edi, %%ecx), %%edi     \n\
            std                                 \n\
            .memmove_go:                        \n\
            rep     movsb                       \n\
            "
            :
            : "D"(dest), "S"(src), "c"(n)
            : "edx", "memory", "cc"
    );
    return dest;
}

/* int32_t strncmp(const int8_t* s1, const int8_t* s2, uint32_t n)
 * Inputs: const int8_t* s1 = first string to compare
 *         const int8_t* s2 = second string to compare
 *               uint32_t n = number of bytes to compare
 * Return Value: A zero value indicates that the characters compared
 *               in both strings form the same string.
 *               A value greater than zero indicates that the first
 *               character that does not match has a greater value
 *               in str1 than in str2; And a value less than zero
 *               indicates the opposite.
 * Function: compares string 1 and string 2 for equality */
int32_t strncmp(const int8_t* s1, const int8_t* s2, uint32_t n) {
    int32_t i;
    for (i = 0; i < n; i++) {
        if ((s1[i] != s2[i]) || (s1[i] == '\0') /* || s2[i] == '\0' */) {

            /* The s2[i] == '\0' is unnecessary because of the short-circuit
             * semantics of 'if' expressions in C.  If the first expression
             * (s1[i] != s2[i]) evaluates to false, that is, if s1[i] ==
             * s2[i], then we only need to test either s1[i] or s2[i] for
             * '\0', since we know they are equal. */
            return s1[i] - s2[i];
        }
    }
    return 0;
}

/* int8_t* strcpy(int8_t* dest, const int8_t* src)
 * Inputs:      int8_t* dest = destination string of copy
 *         const int8_t* src = source string of copy
 * Return Value: pointer to dest
 * Function: copy the source string into the destination string */
int8_t* strcpy(int8_t* dest, const int8_t* src) {
    int32_t i = 0;
    while (src[i] != '\0') {
        dest[i] = src[i];
        i++;
    }
    dest[i] = '\0';
    return dest;
}

/* int8_t* strcpy(int8_t* dest, const int8_t* src, uint32_t n)
 * Inputs:      int8_t* dest = destination string of copy
 *         const int8_t* src = source string of copy
 *                uint32_t n = number of bytes to copy
 * Return Value: pointer to dest
 * Function: copy n bytes of the source string into the destination string */
int8_t* strncpy(int8_t* dest, const int8_t* src, uint32_t n) {
    int32_t i = 0;
    while (src[i] != '\0' && i < n) {
        dest[i] = src[i];
        i++;
    }
    while (i < n) {
        dest[i] = '\0';
        i++;
    }
    return dest;
}

/* void test_interrupts(void)
 * Inputs: void
 * Return Value: void
 * Function: increments video memory. To be used to test rtc */
void test_interrupts(void) {
    int32_t i;
    for (i = 0; i < NUM_ROWS * NUM_COLS; i++) {
        video_mem[i << 1]++;
    }
}


/* update_cursor(int x, int y)
 * Inputs: x -- x location of cursor
            y -- y location of cursor
 * Return Value: void
 * Function: updates cursor location based on inputs, x and y */
void update_cursor(int x, int y)
{
	uint16_t pos = y * NUM_COLS + x;
 
	outb(0x0F, 0x3D4);
	outb((uint8_t) (pos & 0xFF), 0x3D5);
	outb(0x0E, 0x3D4);
	outb((uint8_t) ((pos >> 8) & 0xFF), 0x3D5);
}
