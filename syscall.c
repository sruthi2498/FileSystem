#include "all_include.h"
#include "disk.h"
#include "write_to_log.h"
#include "syscall.h"
#include "initialise.h"

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include <unistd.h>


/*
syscall_format 
	- Creates a new filesystem on the disk, destroying any data already present
	- Sets aside ten percent of the blocks for inodes 
	- clears the inode table
	- writes the superblock 
	- Returns one on success, zero otherwise. Note that formatting a filesystem does not cause it to be mounted. Also, an attempt to format an already-mounted disk should do nothing and return failure. */
int syscall_format()
{
	
	ResetLogFile();
	LogWrite("System Format\n");
	
	if(disk_init("memory_file.dat",NUMBER_OF_BLOCKS	)!=1){
		LogWrite("Disk init failed");
		return 0;
	}
	int superret=init_superblock();
	if(superret!=1) return 0;
	syscall_debug();
	int inoderet=initialise_empty_inodes();
	if(inoderet!=1) return 0;
	
	return 1;
}

/*
syscall_debug 
	-Report on how the inodes and blocks are organized. 
*/
void syscall_debug()
{
	union syscall_block block;

	disk_read(0,block.data);

	printf("\nsuperblock:\n");
	printf("    %d blocks\n",block.super.nblocks);
	printf("    %d inode blocks\n",block.super.ninodeblocks);
	printf("    %d inodes\n",block.super.ninodes);
}

/*
syscall_mount 
	- Examine the disk for a filesystem
	- read the superblock
	- build a free block bitmap
		(Each time that an SimpleFS filesystem is mounted, the system must build a new free block bitmap from scratch by scanning through all of the inodes and recording which blocks are in use)
	- prepare the filesystem for use
	-Return one on success, zero otherwise
*/
int syscall_mount()
{
	union syscall_block block;
	disk_read(0,block.data);
	printf("\n%d\n",block.super.magic);
	if(block.super.magic!=0xf0f03410){
		LogWrite("Disk not correct\n");
		return 0;
	}
	LogWrite("Disk is valid\n");
	// int k=0; //for every actual inode
	// for(int i=1;i<=NUMBER_OF_INODE_BLOCKS;i++){ //for every inode block
	// 	disk_read(i,block.data);
	// 	for(int j=0;j<INODES_PER_BLOCK;j++){//for every inode in the block
			
	// 		k++;
	// 	}
	// }
	LogWrite("Free Block Bitmap created\n");
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

