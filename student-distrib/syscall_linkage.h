#ifndef _SYSCALL_LINKAGE_H
#define _SYSCALL_LINKAGE_H

#ifndef ASM

#include "syscall.h"

//call system handler
extern void syscall_handler(void);

#endif

#endif
