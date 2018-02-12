#include "all_include.h"
#include "disk.h"
#include "write_to_log.h"
#include "syscall.h"
#include "initialise.h"
#include "dir.h"
#include "file.h"

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
int syscall_format(int reset)
{
	
	ResetLogFile();
	LogWrite("System Format\n");
	
	//initialise disk file
	if(disk_init("memory_file.dat",NUMBER_OF_BLOCKS	)!=1){
		LogWrite("Disk init failed");
		return 0;
	}

	//initialise superblock
	int superret=init_superblock();
	if(superret!=1) return 0;

	//view superblock details
	syscall_debug();

	//initialise empty inodes
	int inoderet=initialise_empty_inodes(reset);
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
	if(block.super.magic!=DISK_MAGIC){
		LogWrite("Disk not correct\n");
		LogWrite("Syscall Mount Failed\n");
		return 0;
	}
	LogWrite("Disk is valid\n");

	int k=0; //for every actual inode
	struct syscall_inode Inode;

	for(int i=1;i<=NUMBER_OF_INODE_BLOCKS;i++){ //for every inode block
		disk_read(i,block.data);
		for(int j=0;j<INODES_PER_BLOCK;j++){//for every inode in the block
			Inode = block.inode[j];
			//printf("\ninode %d pointers %d\n",k,POINTERS_PER_INODE);
			/*
			for every data block pointer in the inode
				check if the pointer exists
				if it does, mark that data block in the free block bitmap
			*/
			for(int l=0;l<POINTERS_PER_INODE;l++){
				//printf("%d ",Inode.direct[l]);
				if(Inode.direct[l]!=-1){
					free_block_bitmap[Inode.direct[l]]=1;
				}
			}
			k++;
		}
	}

	LogWrite("Free Block Bitmap created\n");
	return 1;
}

/*
syscall_create 
	- Create a new inode of zero length
	- On success, return the (positive) inumber
	- On failure, return -1
*/
int syscall_create_Inode()
{
	int i=0;
	// find the first non valid inode
	while(i<NUMBER_OF_INODES){
		if(i_list[i].isvalid==0){
			LogWrite("Created new inode\n");
			return i;
		}
	}
	LogWrite("No free inode found\n");
	return -1;
}

/*
Read specified inode from disk
*/
struct syscall_inode ReadInode(int inumber){
	//calculate block number for this inode
	int blocknum= calculate_block_for_inode(inumber);

	//read the specified block
	union syscall_block block;
	disk_read(blocknum,block.data);

	//calculate offset for this inode and block
	int offset= calculate_offset_in_block(inumber,blocknum);


	LogWrite("Read Inode\n");
	return block.inode[offset];
}


/*
syscall_delete 
	- Delete the inode indicated by the inumber
	- Release all data and indirect blocks assigned to this inode 
	- Return them to the free block map
	- On success, return one. On failure, return 0. 
*/
int syscall_delete_Inode( int inumber )
{
	if(inumber<0 || inumber> NUMBER_OF_INODES){
		LogWrite("Unable to delete specified inode\n");
		return 0;
	}
	//read the specified inode
	struct syscall_inode Inode = ReadInode(inumber);
	inode_atttributes_given_inode(Inode);

	//Make changes to the inode

	//for every data block
	for(int i=0;i<POINTERS_PER_INODE;i++){
		if(Inode.direct[i]!=-1){ //if a valid data block exists
			//free the data block in the bitmap
			printf("\ndata block %d before freeing : %d",Inode.direct[i],free_block_bitmap[Inode.direct[i]]);
			free_block_bitmap[Inode.direct[i]]=0;
			printf("\ndata block %d after freeing : %d",Inode.direct[i],free_block_bitmap[Inode.direct[i]]);
			//free the data block in the inode
			Inode.direct[i]=-1;
		}
	}
	Inode.isvalid=0; //make the inode invalid
	Inode.size=(int)sizeof(Inode);

	union syscall_block block;
	//Block and offset for this inode
	int blocknum=Inode.blocknum;
	int offset=Inode.offset_in_block;

	inode_atttributes_given_inode(Inode);

	//Read the specified block
	disk_read(blocknum,block.data);
	//set the inode in the block to the new modified one
	block.inode[offset]=Inode;
	//Write the changes made back to disk
	disk_write(blocknum,block.data);

	LogWrite("Deleted specified inode\n");
	return 1;
}


/*
syscall_getsize
 	-Return the logical size of the given inode, in bytes
 	- On failure, return -1. 
*/
int syscall_getsize( int inumber )
{
	if(inumber<=1 || inumber>NUMBER_OF_INODES){
		LogWrite("Getsize attempted on invalid inode");
		return -1;
	}
	struct syscall_inode Inode;
	Inode=ReadInode(inumber);
	return Inode.size;
}

/* 
syscall_read 
	- Read data from a valid inode
	- Copy "length" bytes from the inode into the "data" pointer, starting at "offset" in the inode. Return the total number of bytes read. The number of bytes actually read could be smaller than the number of bytes requested, perhaps if the end of the inode is reached. If the given inumber is invalid, or any other error is encountered, return 0. 
*/

int syscall_read( int inumber, char *data, int length, int offset )
{
	return 0;
}

int syscall_write( int inumber, const char *data, int length, int offset )
{
	return 0;
}
