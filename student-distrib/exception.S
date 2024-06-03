// Checkpoint 1 New File Created
#define ASM     1

#define EXCEPTION(name, id)         \
.globl  name                        ;\
.align  4                           ;\
name:                               ;\
    pushfl                          ;\
    pushal                          ;\
    pushl   $id                     ;\
    call    exception_handler       ;\
    addl    $4, %esp                ;\
    popal                           ;\
    popfl                           ;\
    iret

# define Exception functions in Assembly linkage
EXCEPTION(divide, 0x00);
EXCEPTION(reserve, 0x01);
EXCEPTION(nmi, 0x02);
EXCEPTION(breakpoint, 0x03);
EXCEPTION(overflow, 0x04);
EXCEPTION(bound, 0x05);
EXCEPTION(opcode, 0x06);
EXCEPTION(dna, 0x07);
EXCEPTION(doubleFault, 0x08);
EXCEPTION(coprocessor, 0x09);
EXCEPTION(tssStack, 0x0A);
EXCEPTION(segment, 0x0B);
EXCEPTION(stack, 0x0C);
EXCEPTION(generalProtection, 0x0D);
EXCEPTION(pageFault, 0x0E);
// we skip 0x015 because it's Intel Reserved
EXCEPTION(fpu, 0x10);
EXCEPTION(alignmentCheck, 0x11);
EXCEPTION(machineCheck, 0x12);
EXCEPTION(simd, 0x13)






