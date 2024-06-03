// Checkpoint 1 New File
#ifndef _IDT_H
#define _IDT_H

#ifndef ASM
#include "lib.h"
#include "exception.h"
#include "x86_desc.h"
#include "handlers.h"
#include "syscall_linkage.h"

// PIC interrupt IDT index
#define PIT_IDT 0x20
// keyboard interrupt IDT index
#define KEYBOARD_IDT 0x21
// rtc interrupt IDT index
#define RTC_IDT 0x28
// system call interrupt IDT index
#define SYSCALL_IDT 0x80
// number internal exceptions implemented
#define NUM_EXCEPTIONS 20

// populates the idt table and fills it to idt_desc_ptr
void idt_init(void);
// prints exception message
extern void exception_handler(int vect);
// executes System Call
extern void system_call();
// test division by zero exception
extern void divideZeroException();

#endif
#endif
