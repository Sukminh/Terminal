#ifndef _SCHEDULER_H
#define _SCHEDULER_H

#include "lib.h"

/* Refered Source */
/* https://wiki.osdev.org/Programmable_Interval_Timer#Channel_0 */
/* http://www.osdever.net/bkerndev/Docs/pit.htm */
#define PIT_IRQ 0x00 // PIT IS FIRST INDEX IN HW IRQ
#define HZ 50   // set HZ to 50 
#define INPUT_CLOCK 1193180 // input clock of 1.19MHz (1193180Hz)
#define CMD_REG 0x43 // Command Register located at 0x43
#define CMD_BYTE 0x36 // 0b 0011 0110 (refer below)
/*CNTR - Counter # (0-2)
RW - Read Write mode
(1 = LSB, 2 = MSB, 3 = LSB then MSB)
Mode - See right table, 3 is Square Wave Mode
BCD - (0 = 16-bit counter,
1 = 4x BCD decade counters)*/

// To set channel 0's Data register, we need to select counter 0 and some modes in the Command register first.

#define CHANNEL_0 0x40 // Data register for each of the timer's 3 channels at 0x40, 0x41, and 0x42 respectively, using first channel

/* Handle RTC interrupts */
void pit_init();
extern void pit_handler();
extern void schedule();

extern int terminal_idx;
extern int pit_counter;

#endif /* _SCHEDULER_H */
