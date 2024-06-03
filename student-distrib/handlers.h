// Checkpoint 1 New File Created
#ifndef HANDLERS_H
#define HANDLERS_H
#include "keyboard.h"
#include "rtc.h"

// handle PIT interrputs
extern void handle_pit(void);
// handle keyboard interrputs
extern void handle_keyboard(void);
// handle rtc
extern void handle_rtc(void);

#endif
