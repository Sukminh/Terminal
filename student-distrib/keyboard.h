#ifndef KEYBOARD_H
#define KEYBOARD_H

#include "types.h"
#include "lib.h"

// ASCII VALUES
#define BS          0x08 // ASCII value for BS
#define ENTR        0x28 // ASCII value for ENTR
#define KEY_IRQ     1
#define KEY_DP      0x60
#define MAX_SCAN    58
#define L           0x26
#define MAX_BUFFER  128
#define TAB         15

//special keys
#define LEFT_SHIFT          0x2A
#define LEFT_RELEASE        0xAA
#define RIGHT_SHIFT         0x36
#define RIGHT_RELEASE       0xB6
#define CTRL                0x1D
#define CTRL_RELEASE        0x9D
#define CAPS                0x3A
#define ALT                 56
#define ALT_RELEASE         0xB8
#define F1                  0x3B
#define F2                  0x3C
#define F3                  0x3D


//letter bounds (left side)
#define Q                   0x10
#define A                   0x1E
#define Z                   0x2C

//letter bounds (right side)
#define P                   0x19
// L is taken care of above
#define M                   0x32

extern int cur_terminal;
extern void init_key(void);
extern void keyboard_handler(void);
void clear_buffer(char* buf, int buffer_counter);
void keyboard_switch_buffer(int index);

extern int flag_alt1;
extern int flag_alt2;
extern int flag_alt3;

#endif


