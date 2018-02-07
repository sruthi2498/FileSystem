#define FUSE_USE_VERSION 31

#if __STDC_VERSION__ >= 199901L
#define _XOPEN_SOURCE 600
#else
#define _XOPEN_SOURCE 500
#endif /* __STDC_VERSION__ */

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

#include "disk.h"
#include "write_to_log.h"
#include "syscall.h"
#include "initialise.h"


#define DISK_MAGIC         0xf0f03410
#define INODES_PER_BLOCK   128
#define POINTERS_PER_INODE 4
#define POINTERS_PER_BLOCK 1024

#define DISK_BLOCK_SIZE  4096// each block
#define DISK_MAGIC 0xdeadbeef 

#define NUMBER_OF_BLOCKS 256
#define NUMBER_OF_INODE_BLOCKS 26
#define NUMBER_OF_INODES NUMBER_OF_INODE_BLOCKS*INODES_PER_BLOCK

static FILE *diskfile;
static int nblocks=0;
static int nreads=0;
static int nwrites=0;


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
	struct timespec	i_atime;	//last access time
	struct timespec	i_mtime;	//last modified time
	struct timespec	i_ctime;	//last time inode was modified
};

union syscall_block {
	struct syscall_superblock super;
	struct syscall_inode inode[INODES_PER_BLOCK];
	int pointers[POINTERS_PER_BLOCK];
	char data[DISK_BLOCK_SIZE];
};

struct syscall_inode i_list[NUMBER_OF_INODES];

int free_block_bitmap[NUMBER_OF_BLOCKS];