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


/*create superblock*/
int init_superblock(){

	union syscall_block block;
	block.super.magic= DISK_MAGIC;
	block.super.nblocks=NUMBER_OF_BLOCKS;
	block.super.ninodeblocks= NUMBER_OF_INODE_BLOCKS ;
	block.super.ninodes=NUMBER_OF_BLOCKS * INODES_PER_BLOCK;

	disk_write(0, block.data);
	LogWrite("Superblock initialised\n");
	return 1;

}

// Given an inode, calculate the block number
int calculate_block_for_inode(int inodenumber){

	/* superblock= block 0
	   inode blocks = NUMBER_OF_INODE_BLOCKS
	   				(block 1 to block NUMBER_OF_INODE_BLOCKS)
	   1 block has INODES_PER_BLOCK  */

	if(inodenumber<0 || inodenumber>NUMBER_OF_INODES){
		LogWrite("\nInvalid Inode number");
		return -1;
	}
	int block_num=inodenumber/INODES_PER_BLOCK  + 1;
	//adding 1 since the first inode block is block 1
	return block_num;
}

/*Given inode number and blocknumber, calculate offset
	Blocks 1 to 26 are set aside for inodes
	These blocks are arrays of inodes
	To find the index of this inode in the array, offset is required
*/
int calculate_offset_in_block(int inodenumber,int blocknum){
	if(inodenumber<0 || inodenumber>NUMBER_OF_INODES){
		printf("\ninode %d",inodenumber);
		LogWrite("Invalid Inode number\n");
		return -1;
	}
	if(blocknum<0 || blocknum>NUMBER_OF_BLOCKS-1){
		LogWrite("Invalid Block number\n");
		return -1;
	}
	/* first inode in block
		subtract 1 since inodes start from block 1
	*/		
	int first_inode_in_block=(blocknum-1)*INODES_PER_BLOCK;
	int offset=inodenumber - first_inode_in_block;
	return offset;

}

/*
Go through all inodes
Caculate block number and offset for them
add it to i-list
*/
int initialise_empty_inodes(){
	for(int i=0;i<NUMBER_OF_INODES;i++){
		i_list[i].isvalid=0;
		i_list[i].blocknum=calculate_block_for_inode(i);
		i_list[i].offset_in_block=calculate_offset_in_block(i,i_list[i].blocknum);

	}
	union syscall_block block;
	int k=0; //for every actual inode
	for(int i=1;i<=NUMBER_OF_INODE_BLOCKS;i++){ //for every inode block
		for(int j=0;j<INODES_PER_BLOCK;j++){//for every inode in the block
			block.inode[j]=i_list[k];
			k++;
		}
		disk_write(i,block.data);
		
	}
	LogWrite("Completed Initialising empty inodes\n");
	return 1;
}


void inode_atttributes_given_inodenumber(int inodenumber){

	/* superblock= block 0
	   inode blocks = 26 block 1 to block 26
	*/
	printf("\ninodenumber %d",inodenumber);
	if(inodenumber<=0 || inodenumber>NUMBER_OF_INODES){
		LogWrite("\nInvalid Inode number");
		return;
	}

	int blocknum=calculate_block_for_inode(inodenumber);
	int offset=calculate_offset_in_block(inodenumber,blocknum);

	union syscall_block block;
	disk_read(blocknum,block.data);
	struct syscall_inode Inode;
	Inode=block.inode[offset];
	printf("\nInode:\n");
	printf("    inode number : %d\n",inodenumber);
	printf("    blocknum     : %d\n",Inode.blocknum);
	printf("    offset       : %d\n",Inode.offset_in_block);
	printf("    isvalid      : %d\n",Inode.isvalid);
	printf("    size         : %d\n",Inode.size);
	printf("	change		 : %s\n",Inode.i_ctime);

	for(int i=0;i<POINTERS_PER_INODE;i++){
		printf("%d ",Inode.direct[i]);
	}
	printf("\n");

}



int main(){

	int formatret=syscall_format();
	if(formatret!=1){
		LogWrite("Syscall Format failed");
	}
	disk_attributes();
	syscall_mount();

	syscall_create();
	syscall_create();
	syscall_delete(2);
	syscall_delete(1);
	syscall_create();
	inode_atttributes_given_inodenumber(1);
	
	disk_attributes();

	// inode_atttributes_given_inodenumber(676);
	// //syscall_inode_atttributes(146);
	// //disk_write(0,"aaa");
	// // char * temp=(char *)malloc(sizeof(char)*DISK_BLOCK_SIZE);
	// // char * str=(char *)malloc(sizeof(char)*5);
	// // strcpy(str,"aaaa");
	// // disk_write(34, str);

	// // disk_read(34,temp);
	// // printf("\nread %s\n",temp);
	// // free(temp);
	// // union syscall_block block;
	// // disk_read(1,&block.inode[0]);
	// // printf("\nblock %d offfset %d\n",block.inode[0].blocknum,block.inode[0].offset_in_block);
	// // disk_read(1,&block.inode[1]);
	// // printf("\nblock %d offfset %d\n",block.inode[0].blocknum,block.inode[0].offset_in_block);

	// disk_close();
	
	return 1;
}
