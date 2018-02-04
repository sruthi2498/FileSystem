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

#include "disk.h"
#include "write_to_log.h"
#include "syscall.h"

#define syscall_MAGIC           0xf0f03410
#define INODES_PER_BLOCK   128
#define POINTERS_PER_INODE 5
#define POINTERS_PER_BLOCK 1024

#define DISK_BLOCK_SIZE  4096// each block
#define DISK_MAGIC 0xdeadbeef 

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
	int indirect;
};

union syscall_block {
	struct syscall_superblock super;
	struct syscall_inode inode[INODES_PER_BLOCK];
	int pointers[POINTERS_PER_BLOCK];
	char data[DISK_BLOCK_SIZE];
};


