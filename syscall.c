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
	disk_read(0, block.data);
	printf("\n%d\n",block.super.magic);
	if(block.super.magic!=DISK_MAGIC){
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

/* 
syscall_create
	- creates new inode of length = 0
	- returns +ve inode number on success
	- returns 0 on fail
*/
int syscall_create()
{
	int i;
	for(i=0; i<NUMBER_OF_INODES; i++){
		//Find free inode
		if(i_list[i].isvalid == 0){

			//Initialize		
			i_list[i].isvalid = 1;
			i_list[i].size = 0;

			//Set time of creation
			clock_gettime(CLOCK_REALTIME, &i_list[i].i_ctime);


			//Get disk information 
			int blocknumber = calculate_block_for_inode(i);
			int block_offset = calculate_offset_in_block(i,blocknumber);

			//Write inode information to disk 
			disk_write(blocknumber, &i_list[i] );

			//Log creation
			printf("Created inode %d in block %d at time \n", i, i_list[i].blocknum, i_list[i].i_ctime);
			LogWrite("Created inode successfully\n");

			return i;
		}
	}
	LogWrite("No free inodes! Out of space \n");
	return 0;
}

int syscall_delete( int inumber )
{
	if((inumber <= 0)|| (inumber > NUMBER_OF_INODES)){
		LogWrite("Invalid Inode number\n");
	}
	else{
		int i;
		int block_to_be_freed;
		if(i_list[inumber].isvalid == 1){
			
			//Change status to invalid
			i_list[inumber].isvalid = 0;

			//Return data blocks to free block bitmap
			for(i=0; i<POINTERS_PER_INODE; i++){
				block_to_be_freed = i_list[inumber].direct[i];
				if(block_to_be_freed > 0){
					free_block_bitmap[block_to_be_freed] = 0;
					printf("Datablock %d freed\n",block_to_be_freed);
				}
				//Set data block pointers from inode to 0
				i_list[inumber].direct[i] = 0;
			}

			//Log deletion
			printf("Inode %d deleted successfully\n", inumber);
			LogWrite("Inode deleted successfully\n");
			return 1;
		}
		else{
			printf("Inode %d already free\n", inumber);
			LogWrite("Inode was already free\n");
		}
	}

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

