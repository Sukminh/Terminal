#ifndef _TERMINAL_H
#define _TERMINAL_H

#include "types.h"
#include "lib.h"

#define BUF_SIZE 128

/* Initialize variables specific to terminal */
int terminal_open(const uint8_t* filename);

/* set buf to spaces */
void terminal_init(void);

/* reset variables specific to terminal */
int terminal_close(int32_t fd);

/* read keyboard input into buf, block interrupts until enter key is pressed */
int terminal_read(int32_t fd, void* buf, int32_t n_bytes);

/* print everything in buffer (except \0) to screen */
int terminal_write(int32_t fd, const void* buf, int32_t n_bytes);

/* Read kb buf into char buf */
void copy_kb_buff(void* kb_buff, int index, int n_bytes);

/* called whenever the keyboard handler prints character, set store keyboard inputs to char_buf and update enter_flag and num_chars */
void keyboard_input(uint8_t new_char);

void terminal_switch_buffer(int index);

extern int cur_terminal_num;

#endif
