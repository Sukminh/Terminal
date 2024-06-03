#include "syscall.h"
// #include "idt.h"

//terminal array
terminal_t terminal_arr[3];

//process array to check running process(es), 0-5
uint32_t process_arr[6] = {0, 0, 0, 0, 0, 0};
// current executed pid
uint32_t pid = 0;
// parent of currently executed pid
uint32_t parent_pid;
// parent pid of currently executed pid
// parent filename
uint8_t* parent_filename;

int system_exception_halt_flag = 0;

int fsysOffset = 0;
// arguments from user
uint8_t arg[FILENAME_LEN];

int32_t system_fd;

/* void fsys_init()
 * Inputs: None
 * Return Value: None
 * Function: initialize open/close/read/write systems 
 * init. corresponding(file type) function for sys_read/write/open/close
*/
void fsys_init() {
    //read, write, open, close for null
    ops_null.sys_read = &null_read;
    ops_null.sys_write = &null_write;
    ops_null.sys_open = &null_open;
    ops_null.sys_close = &null_close;

    //read, write, open, close for rtc
    ops_rtc.sys_read = &rtc_read;
    ops_rtc.sys_write = &rtc_write;
    ops_rtc.sys_open = &rtc_open;
    ops_rtc.sys_close = &rtc_close;

    //read, write, open, close for dir
    ops_dir.sys_read = &dir_read;
    ops_dir.sys_write = &dir_write;
    ops_dir.sys_open = &dir_open;
    ops_dir.sys_close = &dir_close;

    //read, write, open, close for fs
    ops_file.sys_read = &fs_read;
    ops_file.sys_write = &fs_write;
    ops_file.sys_open = &fs_open;
    ops_file.sys_close = &fs_close;

    //read, write, open, close for stdin
    ops_stdin.sys_read = &terminal_read;
    ops_stdin.sys_write = &null_write;
    ops_stdin.sys_open = &terminal_open;
    ops_stdin.sys_close = &terminal_close;

    //read, write, open, close for stdout
    ops_stdout.sys_read = &null_read;
    ops_stdout.sys_write = &terminal_write;
    ops_stdout.sys_open = &terminal_open;
    ops_stdout.sys_close = &terminal_close;
}


/* int32_t system_execute (const uint8_t* parsed_command)
 * Inputs: const uint8_t* parsed_command
 * Return Value: -1/0
 * Function: executing system interrupts
*/
int32_t system_execute (const uint8_t* parsed_command) {
    cli();
    // index for loop
    int i;
    int j;
    // current file's pcb
    pcb_t* pcb;

    // string for either entire arg, command or first part of the arg
    // uint8_t* parsed_command;
    // dentry var. to copy the dentry
    dentry_t dentry;
    // char buffer to read first 4 bytes of file
    uint8_t elf_buffer[4];
    // read_dentry_flag will save return value from read_dentry
    int32_t read_dentry_flag;
    // executable_flag to check file is executable
    int executable_flag = 1;
    // var. for current inode struct
    inode_t* cur_inode_struct;
    // variable to save file size
    uint32_t file_size;
    // char buffer to read 23-27(4 bytes) of file
    uint32_t instruction_buffer;
    // full number of processes flag
    int max_process_running_flag = 1;
    // command buffer 
    uint8_t command[FILENAME_LEN];

    // clear both command and argument buffer first
    for (i=0; i<FILENAME_LEN; i++){
        command[i] = '\0';
        arg[i] = '\0';
    } 
    // check if the command to execute is invalid(aka NULL) then return -1
    if (parsed_command == NULL){
        return -1;
    }

    // index of space character in parsed command
    int space_index = -1;
    // flag that indicates if the loop hits space character
    int spaceflag=0;
    // get command buffer from parsed_command
    for (i=0; i<FILENAME_LEN; i++){
        if(((uint32_t)i)<strlen((const int8_t*)parsed_command) && spaceflag==0){
            if (parsed_command[i] == ' '){
                space_index = i+1;
                spaceflag=1;
                command[i] = '\0';
            }
            else{
                command[i] = (uint8_t)parsed_command[i];
            }
        }
        else{
            command[i] = '\0';
        }
    }
    // number of blank spaces between command and argument
    int blank_index = 0;
    // check if parsed command length is greater than max file length of 32
    if (strlen((const int8_t*)parsed_command) - space_index > FILENAME_LEN) {
        return -1;
    }
    // get argument buffer from parsed_command

    // check if length of the argument is over 32
    if (strlen((const int8_t*)parsed_command) - space_index > FILENAME_LEN) {
        return -1;
    }
    
    for (j=space_index; j<strlen((const int8_t*)parsed_command); j++) {
        // skip all spaces between command and argument
        if (parsed_command[j] != ' ') {
            arg[j-space_index-blank_index] = (uint8_t)parsed_command[j];
        }
        else{
            blank_index++;
        }
    }
    // puts(command);
    // putc('\n');
    // check if it's executable
    // call read_dentry_by_name to copy dentry of the parsed file name
    read_dentry_flag = read_dentry_by_name(command, &dentry);
    // check if the flag is -1 which is same as fail
    if (read_dentry_flag == -1){
        // non-existent file or invalid index return -1
        return -1;
    }

    // use saved dentry and read_data(), reads n_bytes of data from file into buf
    // second argument(to save last read position or offset), offset is set to 0 now, but will be fixed later w/ counter
    // read 4 bytes from the beginning of the file
    read_data(dentry.inode_num, 0, elf_buffer, 4);

    // in order to check executable, need to check first 4 bytes of the file given, if it's different, turn the executable_flag off(to 0) so that execute stops
    // 0x7F is 'DEL' in ASCII
    if (elf_buffer[0] != 0x7F){
        executable_flag = 0;
    }
    // 0x45 is 'E' in ASCII
    if (elf_buffer[1] != 0x45){
        executable_flag = 0;
    }
    // 0x4C is 'L' in ASCII
    if (elf_buffer[2] != 0x4C){
        executable_flag = 0;
    }
    // 0x46 is 'F' in ASCII
    if (elf_buffer[3] != 0x46){
        executable_flag = 0;
    }
    // if executable_flag is 0, return -1(fail)
    if (executable_flag == 0){
        return -1;
    }
    
    // since the file is now executable, before setting up prog. paging, check if it's possible to run additional task
    for (i=0; i<MAX_TASK; i++){
        if (process_arr[i] == 0){
            pid = i;
            process_arr[i] = 1;
            //maximum nuumber of process running
            max_process_running_flag = 0;
            break;
        }
    }

    //check if process array is filled
    if (max_process_running_flag == 1){
        puts("Maximum process limit exceeded. Please quit some programs. \n");
        // return -1(fail)
        return 0;
    }

    // save parent pid from prev. system_execute()
    // (J)
    // parent_pid = pid;
    parent_pid = terminal_arr[terminal_idx].youngest_pid;
    terminal_arr[terminal_idx].parent_pid = parent_pid;

    // now set up program paging
    // pid is index of task(shell: 0, ls: 1 ...)
    // directory[32].addr = EIGHT_MB + FOUR_MB * pid;
    pageSetup(pid);
    // pageSetup(terminal_arr[terminal_idx].youngest_pid);

    // find file size corresponding to inode of each file
    // 4096(4kB), size of each block in filesystem.img
    cur_inode_struct = (inode_t*)((int)boot_block_ptr + 4096 + (4096*(dentry.inode_num)));
    file_size = cur_inode_struct->length;
    // give 0x08048000(file image mem. location 0x08000000 w/ offset for file data 0x00048000) as a pointer to buffer, read as much as file_size
    read_data(dentry.inode_num, 0, (uint8_t*)0x08048000, file_size);

    // Create PCB
    // (J)
    pcb = get_pcb(pid);
    // pcb = get_pcb(terminal_arr[terminal_idx].youngest_pid);

    pcb->parent_pid = parent_pid;
    // pcb->parent_pid = terminal_arr[terminal_idx].parent_pid;

    // save parent filename for halt
    pcb->parent_file_name = (uint8_t*)command;
    // pcb->terminal = cur_terminal_num;

    // update terminal's latest process id into youngest_pid
    terminal_arr[terminal_idx].youngest_pid = pid;

    // (!)store the parent task’s PCB pointer in the child task’s PCB so that when the child program calls halt, you are able to return control to the parent task
    // pcb->parent_pcb_addr = (EIGHT_MB - EIGHT_KB_SIZE*(parent_pid+1));
    // (!)Prepare for Context Switch(find user-level EIP, CS, EFLAGS, ESP, and SS)
    // EIP(loc. of first instruction in cur. file - byte 24-27), 24 is offset, read four bytes from 24 to 27
    read_data(dentry.inode_num, 24, (uint8_t*)&instruction_buffer, 4);
    // (v)CS(given) - KERNEL_CS or USER_CS(privilege level specified by the low 2 bites of the CS register)
        // we should use USER_CS to change from kernel to user code seg.
    // (v)EFLAGS(use pushfl to find)
    // ESP(addr. of user-level stk.) - for the new prog., 132MB to stk.
        // we #define USER_ESP as 132MB
    // (v)User DS(given) or SS - depending on transition to other program or kernel
        // we use USER_DS to transit to other user prog.

    // (!)modify the TSS
    // (!)When a context switch from user space to kernel space happens, the value of esp0 in the tss struct is loaded into the esp register and used as the pointer to the kernel stack
    // subtract by 4 so that it provides right above so that bottom of the stack
    tss.esp0 = EIGHT_MB - EIGHT_KB_SIZE * pid - 4;
    tss.ss0 = KERNEL_DS;

    pcb_t* parent_pcb = get_pcb(pcb->parent_pid);
    if(parent_pcb != NULL){
        //store esp value of parent into pcb->esp
        asm("movl %%esp, %0" : "=r"(parent_pcb->esp));
        //store ebp value of parent into pcb->ebp
        asm("movl %%ebp, %0" : "=r"(parent_pcb->ebp));
    }
    else {
        asm("movl %%esp, %0" : "=r"(pcb->esp));
        //store ebp value of parent into pcb->ebp
        asm("movl %%ebp, %0" : "=r"(pcb->ebp));
    }

    // fsys_init();
    //set fd_array values for stdin
    pcb->fd_array[0].inode = 0;
    pcb->fd_array[0].ops_table_ptr = &ops_stdin; 
    pcb->fd_array[0].file_pos = 0;
    pcb->fd_array[0].flag = 1;

    //set fd_array values for stdout
    pcb->fd_array[1].inode = 0;
    pcb->fd_array[1].ops_table_ptr = &ops_stdout; 
    pcb->fd_array[1].file_pos = 0;
    pcb->fd_array[1].flag = 1;
    
    // 0 - first variable, 1 - second variable, 2 - third variable, 3 - fourth variable, 200 - setting flags that enables interrupt
    asm volatile("cli                       \n\
                  pushl %0                  \n\
                  pushl %1                  \n\
                  pushfl                    \n\
                  popl %%ebx                \n\
                  orl $0x200, %%ebx         \n\
                  pushl %%ebx               \n\
                  pushl %2                  \n\
                  pushl %3                  \n\
                  "
                  :
                  : "r"(USER_DS), "r"(USER_ESP), "r"(USER_CS), "r"(instruction_buffer)
                  : "eax", "ebx"
                 );

    asm volatile ("iret");
    asm volatile ("execute_return:          \n\
                    leave                   \n\
                    ret"   
                );
    // system_halt()
    return 0;
}



/* pcb_t* get_pcb(uint32_t pid)
 * Inputs: uint32_t pid
 * Return Value: pcb struct
 * Function: helper fn. to get pcb from corresponding process in kernel stk.
*/
pcb_t* get_pcb(uint32_t pid){
    //return value of pcb calculated
    return (pcb_t*)(EIGHT_MB - EIGHT_KB_SIZE*(pid+1));
}


/* int32_t system_halt (uint8_t status)
 * Inputs: uint8_t status
 * Return Value: 0
 * Function: halting system interrupts
*/
int32_t system_halt (uint8_t status) {
    cli();
    // index i
    int i;

    // avoid truncating status into 8 byte 
    uint32_t status_v2 = status;

    // call halt if exception is called during program
    if (system_exception_halt_flag == 1){
        // 256 states program terminated by exception
        status_v2 = 256;
        system_exception_halt_flag = 0;
    }

    // get current pcb using pid value
    // (J)
    // pcb_t* cur_pcb = get_pcb(pid);
    pcb_t* cur_pcb = get_pcb(terminal_arr[terminal_idx].youngest_pid);

    if (terminal_arr[terminal_idx].youngest_pid == 0){
        process_arr[0] = 0;
        system_execute((const uint8_t*)"shell");
    }
    else if (terminal_arr[terminal_idx].youngest_pid == 1){
        process_arr[1] = 0;
        system_execute((const uint8_t*)"shell");
    }
    else if (terminal_arr[terminal_idx].youngest_pid == 2){
        process_arr[2] = 0;
        system_execute((const uint8_t*)"shell");
    }
    // if (pid == 0){
    //     asm("pushl %ebp");
    //     asm("movl %esp, %ebp");
    //     asm("leave");
    //     asm("ret");
    // }

    // --------------------------------------------------------------------------------------------------------------------------------------
    // restore parent paging
    // call page setup w/ parent pid
    // pid is index of task(shell: 0, ls: 1 ...)
    // directory[32].addr = EIGHT_MB + FOUR_MB * pid;
    pageSetup(terminal_arr[terminal_idx].parent_pid);
    // pageSetup(cur_pcb->parent_pid);
    // --------------------------------------------------------------------------------------------------------------------------------------
    // close any relevant FDs
    // clear out data in child FD arr.
    for (i=0; i<MAX_FD; i++){
        cur_pcb->fd_array[i].flag = 0;
    }

    // set 0 to halting process in the arr.
    // (J)
    process_arr[terminal_arr[terminal_idx].youngest_pid] = 0;
    // process_arr[pid] = 0;

    // set global pid to parent pid
    // pid = cur_pcb->parent_pid;
    // parent_pid = get_pcb(pid)->parent_pid;
    // (J)
    parent_pid = get_pcb(terminal_arr[terminal_idx].youngest_pid)->parent_pid;
    // --------------------------------------------------------------------------------------------------------------------------------------
    // Jump to execute return
    // return addr. is saved at syscall_linkage.S when we are executing child task eg. execute(ls), when we call execute(ls)
    // return addr. is automatically saved in parent(eg. shell's stack kernel), so we save this return addr. inside child's PCB
    // so we can go back to "Restore registers" inside syscall_linkage.S at the time when we called execute(ls)
    // then IRET will take us to Shell as we never excuted child process(eg. ls)
        // !key idea, in execute, save this cur. %esp(guessing this will be corresponding to return addr.) inside PCB

    // !change tss.esp0 using parent pid
    // (!)modify the TSS
    // (!)When a context switch from user space to kernel space happens, the value of esp0 in the tss struct is loaded into the esp register and used as the pointer to the kernel stack
    // subtract by 4 so that it provides right above so that bottom of the stack
    tss.esp0 = EIGHT_MB - EIGHT_KB_SIZE * (cur_pcb->parent_pid) - 4;
    // tss.esp0 = EIGHT_MB - EIGHT_KB_SIZE * (parent_pid);
    tss.ss0 = KERNEL_DS;
    uint32_t esp, ebp;
    //restore parent esp value
    // esp = cur_pcb->esp;
    // esp = get_pcb(terminal_arr[terminal_idx].parent_pid)->esp;
    esp = get_pcb(terminal_arr[terminal_idx].parent_pid)->esp;

    // restore parent ebp value 
    // ebp = cur_pcb->ebp;
    // ebp = get_pcb(terminal_arr[terminal_idx].parent_pid)->ebp;
    ebp = get_pcb(terminal_arr[terminal_idx].parent_pid)->ebp;
    putc('\n');

    // call system_close()
    system_close(system_fd);

    // update lastest pid of cur. scheduled terminal to the next latest
    terminal_arr[terminal_idx].youngest_pid = terminal_arr[terminal_idx].parent_pid;
    terminal_arr[terminal_idx].parent_pid = get_pcb(terminal_arr[terminal_idx].parent_pid)->parent_pid;
    
    // use saved return_addr(from Shell linkage call)
    // 0 - first variable, 1 - second variable, 2 - third variable
    sti();
    asm volatile ("movl %0, %%esp       \n\
                movl %1, %%ebp         \n\
                movl %2, %%eax         \n\
                jmp execute_return      \n\
                " 
                : 
                : "r"(esp), "r"(ebp), "r"((uint32_t) status_v2)
                :   "eax"
                );

    return 0;
}

/* int32_t null_read(int32_t fd, void* buf, int32_t nbytes)
 * Inputs: int32_t fd, void* buf, int32_t nbytes
 * Return Value: -1
 * Function: return -1 for null read values
*/
int32_t null_read(int32_t fd, void* buf, int32_t nbytes){
    return -1;
}

/* int32_t null_write(int32_t fd, const void* buf, int32_t nbytes)
 * Inputs: int32_t fd, const void* buf, int32_t nbytes
 * Return Value: -1
 * Function: return -1 for null write values
*/
int32_t null_write(int32_t fd, const void* buf, int32_t nbytes){
    return -1;
}

/* int32_t null_open(const uint8_t* fname)
 * Inputs: const uint8_t* fnamer
 * Return Value: -1
 * Function: return -1 for null open values
*/
int32_t null_open(const uint8_t* fname){
    return -1;
}

/* int32_t null_close(int32_t fd)
 * Inputs: int32_t fd
 * Return Value: -1
 * Function: return -1 for null close values
*/
int32_t null_close(int32_t fd){
    return -1;
}

/* int32_t system_read (int32_t fd, void* buf, int32_t nbytes)
 * Inputs: int32_t fd, void* buf, int32_t nbytes
 * Return Value: -1, 0, offset
 * Function: call corresponding read for each fd values
*/
int32_t system_read (int32_t fd, void* buf, int32_t nbytes) {
    // 0, 7 because fd is one of 0, 1, 2, 3, 4, 5, 6
    if (fd < 0 || fd > 7) {
        // return -1 on failure
        return -1;
    }
    //check if buf is NULL
    if (buf == NULL){
        // return -1 on failure
        return -1;
    }
    //check if number of bytes to read is less than 0
    if (nbytes < 0){
        // return -1 on failure
        return -1;
    }
    system_fd = fd;
    pcb_t* temp = get_pcb(terminal_arr[terminal_idx].youngest_pid);

    //check if fd trying to read is not filled
    if (temp->fd_array[fd].flag == 0){
        // return -1 on failure
        return -1;
    }

    // globalKeyboardFlag[terminal_idx] = 1;

    //calculate offset to be passed into global variable fsysOffset
    int32_t read_return = temp->fd_array[fd].ops_table_ptr->sys_read(fd, buf, nbytes);

    // if (((uint8_t*)buf)[0] == 'p'){
    //     ops_stdin.sys_read = &null_read;
    // }

    return read_return;

    // int32_t offset = temp->fd_array[fd].ops_table_ptr->sys_read(fd, buf, nbytes);
    // temp->fd_array[fd].file_pos = temp->fd_array[fd].file_pos + offset;
    // fsysOffset = temp->fd_array[fd].file_pos;
    // fsysOffset = 1;
}


/* int32_t system_write (int32_t fd, const void* buf, int32_t nbytes)
 * Inputs: int32_t fd, const void* buf, int32_t nbytes
 * Return Value: -1, nbytes
 * Function: call corresponding write for each fd values
*/
int32_t system_write (int32_t fd, const void* buf, int32_t nbytes) {
    // 0, 7 because fd is one of 0, 1, 2, 3, 4, 5, 6
    if (fd < 0 || fd > 7) {
        // return -1 on failure
        return -1;
    }
    //check if buf is NULL
    if (buf == NULL){
        // return -1 on failure
        return -1;
    }
    //check if bytes trying to write is less than 0
    if (nbytes < 0){
        // return -1 on failure
        return -1;
    }

    //calculate pcb using pid current pid value 
    pcb_t* temp = get_pcb(terminal_arr[terminal_idx].youngest_pid);

    //check if fd value trying to write is empty 
    if (temp->fd_array[fd].flag == 0){
        // return -1 on failure
        return -1;
    }

    // check in case where rtc write function fails(-1)
    if(temp->fd_array[fd].ops_table_ptr->sys_write(fd, buf, nbytes) == -1) {
        // return -1 on failure
        return -1;
    }

    return nbytes;
}

/* int32_t system_open (const uint8_t* filename) 
 * Inputs: const uint8_t* filename
 * Return Value: -1, fd
 * Function: call corresponding open for each fd values
*/
int32_t system_open (const uint8_t* filename) {
    dentry_t sys_dentry;
    //flag to check if fd is full
    int max_fd_elements_flag = 1;

    //check if filename is NULL
    if (filename == NULL){
        // return -1 on failure
        return -1;
    }

    int32_t flag = read_dentry_by_name(filename, &sys_dentry);
    // check if read dentry failed
    if (flag == -1) {
        // return -1 on failure
        return -1;
    }

    int i;

    //get current pcb using current pid value 
    pcb_t* temp = get_pcb(terminal_arr[terminal_idx].youngest_pid);

    // index from 2 in the fd array since stdin, stdout open are handled in excute, and 2 is first fd except terminal
    for (i = 2; i < MAX_FD; i++){
        if (temp->fd_array[i].flag == 0){
            temp->fd_array[i].inode = sys_dentry.inode_num;
            temp->fd_array[i].file_pos = 0;
            temp->fd_array[i].flag = 1;
            max_fd_elements_flag = 0;
            break;
        }
    }

    //check if file descriptor array is full
    if (max_fd_elements_flag == 1){
        // puts("File descriptor array is full. \n");
        // return -1 on failure
        return -1;
    }

    //check if filetype is rtc
    if (sys_dentry.filetype == 0) {
        temp->fd_array[i].ops_table_ptr = &ops_rtc; 
        // check if sys open is valid
        if(temp->fd_array[i].ops_table_ptr->sys_open(filename) == -1) {
            // return -1 on failure
            return -1;
        }
        return i;
    }
    //check if filetype is dir
    else if (sys_dentry.filetype == 1) {
        temp->fd_array[i].ops_table_ptr = &ops_dir; 
        // check if sys open is valid
        if(temp->fd_array[i].ops_table_ptr->sys_open(filename) == -1) {
            // return -1 on failure
            return -1;
        }
        return i;
    }
    //check if filetype is file
    else{
        temp->fd_array[i].ops_table_ptr = &ops_file; 
        // check if sys open is valid
        if(temp->fd_array[i].ops_table_ptr->sys_open(filename) == -1) {
            // return -1 on failure
            return -1;
        }
        return i;
    }
}


/* int32_t system_close (int32_t fd)
 * Inputs: int32_t fd
 * Return Value: -1/0
 * Function: call corresponding close for each fd values
*/
int32_t system_close (int32_t fd) {
    // 0, 7 because fd is one of 2, 3, 4, 5, 6 except stdin, stdout
    if (fd < 2 || fd > 7) {
        return -1;
    }

    //get current pcb using current pid value
    pcb_t* temp = get_pcb(terminal_arr[terminal_idx].youngest_pid);

    //check if current fd can be closed 
    if (temp->fd_array[fd].flag == 0){
        // return -1 on failure
        return -1;
    }

    temp->fd_array[fd].ops_table_ptr->sys_close(fd);

    // empty fd element in fd arr.
    temp->fd_array[fd].flag = 0;

    return 0;
}

/* int32_t system_getargs (uint8_t* buf, int32_t nbytes)
 * Inputs: uint8_t* buf, int32_t nbytes
 * Return Value: -1 on fail / 0 on success
 * Function: copy argument buffer to buf from the input for nbytes characters
*/
int32_t system_getargs (uint8_t* buf, int32_t nbytes) {
    // check if buf is invalid
    if(buf == NULL){
        return -1;
    }

    // check if argument buffer is empty
    if(arg[0] == NULL) {
        return -1;
    }

    // counter index
    int i;

    // clear buf before copying
    for(i=0; i<nbytes; i++) {
        buf[i] = '\0';
    }

    // copy argument buffer to buf from the input
    for(i=0; i<nbytes; i++) {
        if (i < FILENAME_LEN) {
            buf[i] = arg[i];
        }
        else {
            buf[i] = '\0';
        }
    }
    return 0;
}

/* int32_t system_vidmap (uint8_t** screen_start)
 * Inputs: uint8_t** screen_start
 * Return Value: -1 on fail / 0 on success
 * Function: map the page from screen_start to video memory location
*/
int32_t system_vidmap (uint8_t** screen_start) {
    // check if screen_start is invalid (0x8000000 = 128 MB, 0x83FFFFF = 132 MB)
    if (screen_start == (uint8_t**)NULL || screen_start < (uint8_t**)0x8000000 || screen_start > (uint8_t**)0x83FFFFF){
        return -1;
    }
    // setup pages for mapping video memory
    pageSetup_vidmap();
    // map the screen_start location to (0x8400000 = 132 MB)
    *screen_start = (uint8_t*)0x8400000;

    return 0;
}
int32_t system_set_handler (int32_t signum, void* handler_address) {
    return 0;
}
int32_t system_sigreturn (void) {
    return 0;   
}
