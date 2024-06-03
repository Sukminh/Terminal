#ifndef _SYSCALL_H
#define _SYSCALL_H

// #include "types.h"
#include "rtc.h"
#include "paging.h"
#include "terminal.h"
#include "fsys.h"
// #include "x86_desc.h"
// #include "lib.h"
// #include "syscall.S"
#include "scheduler.h"

#define MAX_FD           8              // max number of fd
#define MAX_TASK         6              // max number of tasks
#define EIGHT_MB    0x800000       // EIGHT_MB in hex
#define FOUR_MB     0x400000       // FOUR_MB in hex
#define EIGHT_KB_SIZE    0x2000         // EIGHT_KB_SIZE in hex
#define USER_ESP         0x8400000      // 132MB(1 MB = 1048576 bytes) in hex

// struct for ops_table_t
typedef struct {
    int32_t (*sys_read)(int32_t fd, void* buf, int32_t n_bytes);
    int32_t (*sys_write)(int32_t fd, const void* buf, int32_t n_bytes);
    int32_t (*sys_open)(const uint8_t* filename);
    int32_t (*sys_close)(int32_t fd);
} ops_table_t;

ops_table_t ops_null;   /* null     */
ops_table_t ops_rtc;    /* rtc      */
ops_table_t ops_dir;    /* directory*/
ops_table_t ops_file;   /* file     */
ops_table_t ops_stdin;  /* in       */
ops_table_t ops_stdout; /* out      */

// struct for fd_t
typedef struct {
    ops_table_t* ops_table_ptr; 
    uint32_t inode;
    uint32_t file_pos;
    uint32_t flag;
} fd_t;

// struct for terminal arr.
typedef struct {
    uint32_t parent_pid;
    uint32_t youngest_pid;
    uint32_t flag;
    int enter_flag;
} terminal_t;

extern uint32_t process_arr[6];
extern uint32_t pid;
extern int fsysOffset;
extern int system_exception_halt_flag;
extern terminal_t terminal_arr[3];

// struct for pcb_t
// we use this struct to save necessary information for current program
typedef struct{
    // process id of the pcb struct
    uint8_t pid;
    // fd array of the current program
    fd_t fd_array[MAX_FD];
    // parent process id of the current program
    uint8_t parent_pid;
    // parent file name of the current program
    uint8_t* parent_file_name;
    // save esp, ebp for halt back to parent shell, also use them to transit to next scheduled program
    uint32_t esp;
    uint32_t ebp;
    // save terminal index of the where current program is running
    int terminal;
    // save required rtc rate value for current program
    uint32_t rtc_val;
} pcb_t;

//halting system interrupts
int32_t system_halt (uint8_t status);

//executing system interrupts
int32_t system_execute (const uint8_t* parsed_command);

//call corresponding read for each fd values
int32_t system_read (int32_t fd, void* buf, int32_t nbytes);

//call corresponding write for each fd values
int32_t system_write (int32_t fd, const void* buf, int32_t nbytes);

//call corresponding open for each fd values
int32_t system_open (const uint8_t* filename);

//call corresponding close for each fd values
int32_t system_close (int32_t fd);

//copy argument buffer to buf from the input for nbytes characters
int32_t system_getargs (uint8_t* buf, int32_t nbytes);

//map the page from screen_start to video memory location
int32_t system_vidmap (uint8_t** screen_start);

int32_t system_set_handler (int32_t signum, void* handler_address);

int32_t system_sigreturn (void);

// // extern void syscall_handler();
// extern void syscall_handler(void);

//init. corresponding(file type) function for sys_read/write/open/close
void fsys_init();

//helper fn. to get pcb from corresponding process in kernel stk.
pcb_t* get_pcb(uint32_t pid);

//return -1 for null read values
int32_t null_read(int32_t fd, void* buf, int32_t nbytes);

//return -1 for null write values
int32_t null_write(int32_t fd, const void* buf, int32_t nbytes);

//return -1 for null open values
int32_t null_open(const uint8_t* fname);

//return -1 for null close values
int32_t null_close(int32_t fd);

#endif
