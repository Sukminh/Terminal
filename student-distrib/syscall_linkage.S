#define ASM     1
#include "x86_desc.h"
// #include "syscall.h"

.align 4
syscall_table:
    .long 0x0, system_halt, system_execute, system_read, system_write, system_open, system_close, system_getargs, system_vidmap, system_set_handler, system_sigreturn

.globl syscall_handler
.align 4
syscall_handler:
    # Push register values to stack 
    pushl   %ebx
    pushl   %ecx
    pushl   %edx
    pushl   %esp
    pushl   %ebp
    pushl   %esi
    pushl   %edi
    pushfl

    # Push three arguments
    pushl %edx
    pushl %ecx
    pushl %ebx

    # Verify syscall number
    cmpl    $0, %eax        
    jz      syscall_err
    cmpl    $10, %eax           # compare eax (index of system call) to 10 (1-10)
    ja      syscall_err

    # Call syscall
    call    *syscall_table(, %eax, 4)   # 4 bytes per entry in table
    jmp     syscall_leave

syscall_err:
    movl    $-1, %eax       # Return -1 as error

# Pop all registers 
syscall_leave:
    addl    $12, %esp       # mov stack pointer down by 3 regs(%edx, %ecx, %ebx) each 4 bytes 
    popfl
    popl    %edi
    popl    %esi
    popl    %ebp
    popl    %esp
    popl    %edx
    popl    %ecx
    popl    %ebx
    iret
