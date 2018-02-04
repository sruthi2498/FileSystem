#include "all_include.h"
#include "syscall.h"
#include "disk.h"

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include <unistd.h>


int syscall_format()
{
	return 0;
}

void syscall_debug()
{
	union syscall_block block;

	disk_read(0,block.data);

	printf("superblock:\n");
	printf("    %d blocks\n",block.super.nblocks);
	printf("    %d inode blocks\n",block.super.ninodeblocks);
	printf("    %d inodes\n",block.super.ninodes);
}

int syscall_mount()
{
	return 0;
}

int syscall_create()
{
	return 0;
}

int syscall_delete( int inumber )
{
	return 0;
}

int syscall_getsize( int inumber )
{
	return -1;
}

int syscall_read( int inumber, char *data, int length, int ofsyscallet )
{
	return 0;
}

int syscall_write( int inumber, const char *data, int length, int ofsyscallet )
{
	return 0;
}
