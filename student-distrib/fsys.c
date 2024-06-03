#include "fsys.h"

// NOTE
// 1. each data block's size is 4kB

// struct global variables
boot_block_t* boot_block_struct;
inode_t* cur_inode_struct;

// temporary global variable for filename(will be revised for Checkpoint 3.3 as we implement fd)
uint8_t* cur_file;

// initialize file system
/* void fs_init(unsigned int filesys_start);
 * Inputs: unsigned int filesys_start = starting addr. of filesystem.img (absolute block numbers, 4kB per block)
 * Return Value: none
 * Function: get starting addr. of filesystem.img, set it to boot_block_ptr and create boot_block_struct */
void fs_init(unsigned int filesys_start) {
    // get starting addr. of filesystem.img (absolute block numbers, 4kB per block)
    boot_block_ptr = (boot_block_t*)filesys_start;
    // init. boot_block struct
    boot_block_struct = boot_block_ptr;
}

// file system(kernel used)
/* int32_t read_dentry_by_name (const uint8_t* fname, dentry_t* dentry);
 * Inputs: const uint8_t* fname = name of the file
 *             dentry_t* dentry = pointer to empty dentry to be filled
 * Return Value: 0 on success, -1 on failure
 * Function: find index of file inside boot_block dir. entries call read_dentry_by_index fn. */
int32_t read_dentry_by_name (const uint8_t* fname, dentry_t* dentry){
    // find index of fname in boot block, 'i' will be used as index of the boot_block directory entry index
    int i;
    // use file_exist_flag to check if file exists in boot block
    int file_exist_flag = 0;

    // loop ends at max dir. count if not file isn't found before
    for (i=0; i<boot_block_struct->dir_count; i++){
        // compare string using strncmp, strncmp returns 0 if two strings are equal
        // use 32 bytes for comparision as max # of characters in filename is 32 bytes
        if (strncmp((const int8_t*)fname, (const int8_t*)((boot_block_struct->direntries[i]).file_name), 32) == 0){
            // if file has found, turn the flag on
            file_exist_flag = 1;
            break;
        }
    }

    // non-existent file, return -1
    if (file_exist_flag == 0){
        return -1;
    }

    // Call read_dentry_by_index(), populate the dentry parameter -> file name, file type, inode number
    return read_dentry_by_index(i, dentry);
}
/* void* memset(void* s, int32_t c, uint32_t n);
 * Inputs:    uint32_t index = index of the file inside dir entries.
 *          dentry_t* dentry = pointer to empty dentry to be filled
 * Return Value: 0 on success, -1 on failure
 * Function: find dentry of the file, copy over the dentry */
int32_t read_dentry_by_index (uint32_t index, dentry_t* dentry){
    // invalid index, return -1
    // check by if index(which is directory index) is greater than total dir. count
    if (index > boot_block_struct->dir_count){
        return -1;
    }

    // fill in the dentry_t block with the file name, file type, and inode number, 24B reserved for the file 
    // dentry = &boot_block_struct->direntries[index];
    *dentry = boot_block_struct->direntries[index];
    // memcpy(dentry, &(boot_block_struct->direntries[index]), sizeof(dentry));

    // return 0 on success
    return 0;
}
/* int32_t read_data (uint32_t inode, uint32_t offset, uint8_t* buf, uint32_t length)
 * Inputs:    uint32_t inode = inode number of the file
 *           uint32_t offset = offset, start pos. of each file
 *              uint8_t* buf = buffer to be filled w/ characters
 *           uint32_t length = number of bytes to be read
 * Return Value: number of bytes actually read, or -1 on failure, 0 if EOF reached
 * Function: read file and fill up buffer with data */
int32_t read_data (uint32_t inode, uint32_t offset, uint8_t* buf, uint32_t length){
    // (boot_block_ptr+4096), 0th index of inode, each inode has size of 4kB(4096)
    // set particular inode to the global cur_inode_struct
    cur_inode_struct = (inode_t*)((int)boot_block_ptr + 4096 + 4096*inode);

    // length of the file
    int file_length;
    // index of the data_block_num of inode, use this index to find the actual data block # in the data blocks, 4096(4kB) size of block
    int inode_index;
    // offset of the data(byte readable) in the corresponding cur. data block
    int data_block_offset;
    // start of the cur. data block
    uint8_t* cur_data_block_start_addr;
    // indexing
    int i;
    // src will save current address of the source inside corresponding data block
    uint8_t* src;

    // length of the file
    file_length = cur_inode_struct->length;
    // index of the data_block_num of inode struct, use this index to find the actual data block # in the data blocks
    // 4096(4kB) size of block
    inode_index = (offset/4096);
    // offset of the data(byte readable) in the corresponding cur. data block
    // 4096(4kB) size of block
    data_block_offset = offset % 4096;

    // if invalid inode number return -1
    if (inode >= (boot_block_struct->inode_count)){
        return -1;
    }

    // ! for Checkpoint 3.2 only,
    // if it requires to setup corresponding size of file size, set the max
    // if (length > file_length){
    //     length = file_length;
    // }
    
    // loop until either reached EOF(End of File) or required read_data length has reached
    for (i=0; i<length; i++){
        // cur. reading byte index + given offset, compare with file_length will tell whether EOF has reached or not
        // A return value of 0 to indicate EOF
        // return the remaining length to be read               
        if ((i + offset) >= file_length){
            return i;
        }

        // if a bad data block number is found within the file bounds of the given inode, the function should also return -1
        if ((cur_inode_struct->data_block_num)[inode_index] > INODE_DATA_BLOCK){
            return -1;
        }

        // start of the cur. data block, 4096(4kB) size of block
        // (uint8_t*)((int)boot_block_ptr + 4096 + 4096*(boot_block_struct->inode_count) to offset boot block and all inodes
        // 4096*(cur_inode_struct->data_block_num[inode_index] will bring to start of current data block
        cur_data_block_start_addr = (uint8_t*)((int)boot_block_ptr + 4096 + 4096*(boot_block_struct->inode_count) + 4096*(cur_inode_struct->data_block_num[inode_index]));
        // update source address
        src = (uint8_t*)(cur_data_block_start_addr + data_block_offset);
        // copy the addr. of each data(by byte) in data block to corresponding buffer
        // memcpy(buf+i+offset, src, 1);
        memcpy(buf+i, src, 1);

        // cur. memcopy has completed, thus prepare for next copy
        data_block_offset = data_block_offset + 1;
        // if cur. data_block_offset has reached end of cur. data block, reset it to 0 as we will go to next data_block
        // increment inode_index which is # data block in inode struct
        if ((data_block_offset%4096) == 0){
            data_block_offset = 0;
            inode_index = inode_index + 1;
        }
    }

    // returning the number of bytes read and placed in the buffer
    if (length < file_length){
        return length;
    }
    return file_length;
}

// directory system calls
/* int32_t dir_open(const uint8_t* filename);
 * Inputs: const uint8_t* filename = pointer to memory
 * Return Value: 0 on success
 * Function: open directory */
int32_t dir_open(const uint8_t* filename){
    // opens a diretory file
    // (will be revised later since dir. is specified for Checkpoint 3.2)

    // since flat directory system, directory is always located at direntries[0]
    // thus if it's not if (strncmp(boot_block_struct->direntries[0]->file_name, ".", 32) != 0), not directory
    // if (strncmp(boot_block_struct->direntries[0]->file_name, ".", 32) != 0){
    //     return -1;
    // }

    return 0;
}
/* int32_t dir_close(int32_t fd);
 * Inputs: int32_t fd = file descriptor
 * Return Value: 0 on success
 * Function: close directory */
int32_t dir_close(int32_t fd){
    // probably does nothing
    return 0;
}
/* int32_t dir_read(int32_t fd, void* buf, int32_t n_bytes);
 * Inputs: fd, buf (filename of each directory), n_bytes (number of bytes to read)
 * Return Value: count (length of file in directory)
 * Function: read files filename by filename, including "." (only read one file name at a time) */
int32_t dir_read(int32_t fd, void* buf, int32_t n_bytes){
    // read files filename by filename, including "." (only read one file name at a time)
    // read_dentry_by_index: index is NOT inode number

    // num_files will have total files(incl. directory '.') count
    int num_files;
    // for loop indexing use
    int i;
    // int j;
    // variable to save file size
    // uint32_t file_size;
    // variable to save looping dentry
    dentry_t dentry;

    // num_files will have total files(incl. directory '.') count
    num_files = (boot_block_struct->dir_count);
    // for (i=0; i<num_files; i++){
    //     // save cur. dentry
    //     dentry = boot_block_struct->direntries[i];

    //     // loop file name until 32 since max characters # is 32
    //     printf("file_name: ");
    //     for (j=0; j<32; j++){
    //         putc(dentry.file_name[j]);
    //     }

    //     // print file type of each file
    //     printf(", file_type: %d", dentry.filetype);

    //     // print file size, and to print file size it requires corresponding inode of each file
    //     // find corresponding inode of each file
    //     // 4096(4kB), size of each block in filesystem.img
    //     cur_inode_struct = (inode_t*)((int)boot_block_ptr + 4096 + (4096*(dentry.inode_num)));
    //     file_size = cur_inode_struct->length;
    //     printf(", file_size: %d", file_size);
    //     printf("\n");
    // }

    // actual file length
    int32_t count = 0;

    // get current pcb from pid
    pcb_t* cur_pcb = get_pcb(pid);

    // get index of the file to be read
    uint32_t dir_file_idx = cur_pcb->fd_array[fd].file_pos;

    // read all files in the "." directory
    if (dir_file_idx >= num_files) {
        return 0;
    }

    // get current file's dentry
    dentry = boot_block_struct->direntries[dir_file_idx];
    // count holds actual length of the file read
    for (i=0; i<n_bytes; i++) {
        if (i < FILENAME_LEN) {
            ((uint8_t*)buf)[i] = dentry.file_name[i];
            count++;
        }
        else {
            ((uint8_t*)buf)[i] = '\0';
        }
    }
    // incrementing file position by 1 to get the file index of the directory
    cur_pcb->fd_array[fd].file_pos = cur_pcb->fd_array[fd].file_pos + 1;
    return count;
}

/* int32_t dir_write(int32_t fd, void* buf, int32_t n_bytes);
 * Inputs: int32_t fd = file descriptor
 *          void* buf = buffer to be filled(unused read only filesystem)
 *         uint32_t n = number of bytes to write(unused read only filesystem)
 * Return Value: -1(unused read only filesystem)
 * Function: (unused read only filesystem) */
int32_t dir_write(int32_t fd, const void* buf, int32_t n_bytes){
    // should do nothing
    return -1;
}

// file system (file system calls)
/* int32_t fs_open(const uint8_t* filename);
 * Inputs: const uint8_t* filename = file to be opened
 * Return Value: 0 on success
 * Function: save filename(will be revised later for fd) */
int32_t fs_open(const uint8_t* filename){
    // save filename(will be revised later for fd)
    // cur_file = (uint8_t*)filename;

    // initialize temporary structres
    // (will be revised later since fd isn't implemented for Checkpoint 3.2)

    // If the named file does not exist or no descriptors are free, the call returns -1
    // (will be revised later since fd isn't implemented for Checkpoint 3.2)
    return 0;
}
/* int32_t fs_close(int32_t fd);
 * Inputs: int32_t fd = file descriptor
 * Return Value: 0 on success
 * Function: close file */
int32_t fs_close(int32_t fd){
    // undo fs_open
    // (will be revised later since fd isn't implemented for Checkpoint 3.2)

    // close system call closes the specified file descriptor and makes it available for return from later calls to open
    // You should not allow the user to close the default descriptors (0 for input and 1 for output)
    // (will be revised later since fd isn't implemented for Checkpoint 3.2)

    return 0;
}
/* int32_t fs_read(int32_t fd, void* buf, int32_t n_bytes);
 * Inputs: int32_t fd = file descriptor
 *          void* buf= buffer to be filled w/ characters
 *         int32_t n_bytes = number of bytes to be read
 * Return Value: number of bytes actually read, or -1 on failure, 0 if EOF reached
 * Function: read file data fill the buffer(n_bytes) */
int32_t fs_read(int32_t fd, void* buf, int32_t n_bytes){
    // read_dentry_flag will save return value from read_dentry
    // int32_t read_dentry_flag;
    // dentry_t dentry_loc;
    int32_t read_byte;

    // index var.
    // int i;

    // call read_dentry_by_name to copy dentry_loc
    // read_dentry_flag = read_dentry_by_name(cur_file, &dentry_loc);
    // if (read_dentry_flag == -1){
    //     // non-existent file or invalid index
    //     return -1;
    // }

    // get current pcb from pid
    pcb_t* cur_pcb = get_pcb(pid);

    // get current position of the file to start read from
    uint32_t file_content_idx = cur_pcb->fd_array[fd].file_pos;

    // use saved dentry_loc and read_data(), reads n_bytes of data from file into buf
    // second argument, offset is set to 0 now, but will be fixed later w/ counter
    // to save last read position or offset
    //int offset = fileDescriptor.file_pos;
    read_byte = read_data(cur_pcb->fd_array[fd].inode, file_content_idx, buf, n_bytes);

    // update file position of the cur file
    cur_pcb->fd_array[fd].file_pos = cur_pcb->fd_array[fd].file_pos + read_byte;

    // for each char in buffer, print out the char by char if it's not null
	// for (i=0; i<n_bytes; i++){		//specify 40000 for # of bytes to read as max # of char don't exceed 40000
	// 	// if it's null, skip
	// 	if (((uint8_t*)buf)[i] == '\0'){
	// 		continue;
	// 	}
	// 	// print out char
	// 	putc(((uint8_t*)buf)[i]);

	// }

    // actual file length
    // int32_t count = 0;

    // // read all files in the "." directory
    // if (dir_file_idx >= num_files) {
    //     return 0;
    // }

    // // get current file's dentry
    // dentry = boot_block_struct->direntries[dir_file_idx];
    // for (i=0; i<n_bytes; i++) {
    //     if (i < FILENAME_LEN) {
    //         ((uint8_t*)buf)[i] = dentry.file_name[i];
    //         count++;
    //     }
    //     else {
    //         ((uint8_t*)buf)[i] = '\0';
    //     }
    // }
    // cur_pcb->fd_array[fd].file_pos = cur_pcb->fd_array[fd].file_pos + 1;
    // return count;

    return read_byte;
}
/* int32_t fs_write(int32_t fd, const void* buf, int32_t n_bytes);
 * Inputs: int32_t fd = file descriptor
 *         const void* buf = buffer to be filled(unused read only filesystem)
 *         int32_t n_bytes = number of bytes to write(unused read only filesystem)
 * Return Value: -1(unused read only filesystem)
 * Function: (unused read only filesystem) */
int32_t fs_write(int32_t fd, const void* buf, int32_t n_bytes){
    // should do nothing
    return -1;
}

