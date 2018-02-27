#define FUSE_USE_VERSION 31

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif


#include <fuse.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <fcntl.h>
#include <stddef.h>
#include <assert.h>
#include <time.h>

#include "write_to_log.h"
#include "disk.h"
//#include "namei.h"
#include "syscall.h"
#include "dir.h"
#include "file.h"
#include "initialise.h"

#define syscall_MAGIC           0xf0f03410
#define INODES_PER_BLOCK   128
#define POINTERS_PER_INODE 4
#define POINTERS_PER_BLOCK INODES_PER_BLOCK * POINTERS_PER_INODE
#define MAX_FD 20

#define DISK_BLOCK_SIZE  4096// each block
#define DISK_MAGIC 0xdeadbeef 

#define NUMBER_OF_BLOCKS 256
#define NUMBER_OF_INODE_BLOCKS 26
#define NUMBER_OF_INODES NUMBER_OF_INODE_BLOCKS*INODES_PER_BLOCK
#define DATABLOCK_START NUMBER_OF_INODE_BLOCKS+1



#define ROOT_INODE_NUMBER 0

#define MAX_DIR_ENTRIES 170		// size of block divided by size of directory entry = 4096 / 24
//POSIX MACROS
//#define S_ISDIR 2

static FILE *diskfile;
static int nblocks=0;
static int nreads=0;
static int nwrites=0;

//Directory entry structure
struct syscall_dirent{
	char entry_name[MAX_FD];
	int inode_num;
};

//Stat file information structure
struct syscall_stat {
	int			st_mode;
	int			st_ino;
	int			st_dev;
	int			st_rdev;
	int			st_nlink;
	int			st_uid;
	int 		st_gid;
	int			st_size;
	struct timespec	st_atim;
	struct timespec	st_mtim;
	struct timespec st_ctim;
	int 		st_blksize;
	int 		st_blocks;
};



//DISK STRUCTURES
struct syscall_superblock {
	int magic;
	int nblocks;
	int ninodeblocks;
	int ninodes;
};

struct syscall_inode {
	int isvalid;
	int size;
	int direct[POINTERS_PER_INODE];
	int blocknum;
	int offset_in_block;
};

union syscall_block {
	struct syscall_superblock super;
	struct syscall_inode inode[INODES_PER_BLOCK];
	int pointers[POINTERS_PER_BLOCK];
	char data[DISK_BLOCK_SIZE];
	struct syscall_dirent dir_entries[MAX_DIR_ENTRIES];
	struct syscall_stat stat_info;
};



//FILE STRUCTURES
//Contains file information like file status flags, current file offset, vnode pointer
struct file_table_entry{
	//file status flags

	//current file offset
	//int file_offset;

	//inode num
	int inode_num;
}free_file_table_entries [MAX_FD];


//Contains file descriptor with pointer to file table entry
struct file_desc{
	int fd;
	struct file_table_entry *fd_pointer;
};

//Contains an array of file descriptors and pointers to file table entries
struct open_file_table{
	struct file_desc fd_entry[MAX_FD];
	int count_used_file_descriptors;
};

//list of free file descriptors
int free_file_desc[MAX_FD];

//Free inodes bitmap
struct syscall_inode i_list[NUMBER_OF_INODES];

//Free block bitmap
int free_block_bitmap[NUMBER_OF_BLOCKS];