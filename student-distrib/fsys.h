#ifndef FSYS_H
#define FSYS_H

#include "lib.h"
#include "syscall.h"

#define FILENAME_LEN            32 // max. number of char. for each filename
#define BOOTBLOCK_RESERVED      52 // reserved # bytes for boot block
#define DIR_ENTRIES             63 // # dir. entries in boot block including directory
#define DENTRY_RESERVED         24 // reserved # bytes for denry
#define INODE_DATA_BLOCK        1023 // total # of data blocks

// struct for dentry
typedef struct {
    uint8_t file_name[FILENAME_LEN];
    uint32_t filetype;
    uint32_t inode_num;
    uint8_t reserved[DENTRY_RESERVED];
} dentry_t;

// struct for inode
typedef struct {
    uint32_t length;
    uint32_t data_block_num[INODE_DATA_BLOCK];
} inode_t;

// struct for boot block
typedef struct {
    uint32_t dir_count;
    uint32_t inode_count;
    uint32_t data_count;
    uint8_t reserved[BOOTBLOCK_RESERVED];
    dentry_t direntries[DIR_ENTRIES];
} boot_block_t;

// pointers for boot block, dentry, inode, data_block
boot_block_t* boot_block_ptr;
dentry_t* dentry_ptr;
inode_t* inode_ptr;
uint32_t* data_block_ptr;

// find index of file inside boot_block dir. entries call read_dentry_by_index fn.
int32_t read_dentry_by_name (const uint8_t* fname, dentry_t* dentry);
// find dentry of the file, copy over the dentry
int32_t read_dentry_by_index (uint32_t index, dentry_t* dentry);
// read file and fill up buffer with data
int32_t read_data (uint32_t inode, uint32_t offset, uint8_t* buf, uint32_t length);

// get starting addr. of filesystem.img, set it to boot_block_ptr and create boot_block_struct
void fs_init();
// save filename(will be revised later for fd)
int32_t fs_open(const uint8_t* filename);
// close file
int32_t fs_close(int32_t fd);
// read file data fill the buffer(n_bytes)
int32_t fs_read(int32_t fd, void* buf, int32_t n_bytes);
// file system write (unused read only filesystem)
int32_t fs_write(int32_t fd, const void* buf, int32_t n_bytes);

// open directory
int32_t dir_open(const uint8_t* filename);
// close directory
int32_t dir_close(int32_t fd);
// read files filename by filename, including "." (only read one file name at a time)
int32_t dir_read();
// directory write (unused read only filesystem)
int32_t dir_write(int32_t fd, const void* buf, int32_t n_bytes);

#endif

