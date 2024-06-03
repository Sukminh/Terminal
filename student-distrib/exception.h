// Checkpoint 1 New File Created
#ifndef EXCEPTION_H
#define EXCEPTION_H

#include "idt.h"

void divide(void);
void reserve(void);
void nmi(void);
void breakpoint(void);
void overflow(void);
void bound(void);
void opcode(void);
void dna(void);
void doubleFault(void);
void coprocessor(void);
void tssStack(void);
void segment(void);
void stack(void);
void generalProtection(void);
void pageFault(void);
void fpu(void);
void alignmentCheck(void);
void machineCheck(void);
void simd(void);

#endif
