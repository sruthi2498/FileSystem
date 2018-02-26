#include "all_include.h"
// #include "disk.h"
// #include "write_to_log.h"
// #include "syscall.h"
// #include "initialise.h"
// #include "dir.h"
// #include "file.h"

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include <unistd.h>


/*create superblock*/
int init_superblock(){

	union syscall_block block;
	block.super.magic=DISK_MAGIC;
	block.super.nblocks=NUMBER_OF_BLOCKS;
	block.super.ninodeblocks= NUMBER_OF_INODE_BLOCKS ;
	block.super.ninodes=NUMBER_OF_BLOCKS * INODES_PER_BLOCK;

	disk_write(0,block.data);
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
Set all datablocks to -1 in free_block_bitmap
*/
initialise_free_block_bitmap(){
	for(int i=DATABLOCK_START; i<NUMBER_OF_BLOCKS; i++){
		free_block_bitmap[i] = 0;
	}
}

/*
Go through all inodes
Caculate block number and offset for them
add it to i-list
*/
int initialise_empty_inodes(int reset){
	for(int i=0;i<NUMBER_OF_INODES;i++){
		i_list[i].isvalid = 0;
		i_list[i].blocknum=calculate_block_for_inode(i);
		i_list[i].offset_in_block=calculate_offset_in_block(i,i_list[i].blocknum);
		if(reset){ //set all data blocks to -1
			for(int j=0;j<INODES_PER_BLOCK;j++){//for every inode in the block
				i_list[i].direct[j]=-1;
			}
		}
		i_list[i].size=(int)sizeof(i_list[i]);


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
	printf("    size         : %d\n    ",Inode.size);

	for(int i=0;i<POINTERS_PER_INODE;i++){
		printf("%d ",Inode.direct[i]);
	}
	printf("\n");

}

void inode_atttributes_given_inode(struct syscall_inode Inode){
	printf("\nInode:\n");

	printf("    blocknum     : %d\n",Inode.blocknum);
	printf("    offset       : %d\n",Inode.offset_in_block);
	printf("    isvalid      : %d\n",Inode.isvalid);
	printf("    size         : %d\n    ",Inode.size);

	for(int i=0;i<POINTERS_PER_INODE;i++){
		printf("%d ",Inode.direct[i]);
	}
	printf("\n");
}

void initialise_homeDir(){

	LogWrite("Creating home dir...\n");
	printf("in initialise_homeDir\n");
	//Create an inode
	int curr_inode_num = syscall_create_Inode();
	if(curr_inode_num < 0){
		LogWrite("Home directory inode creation failed\n");
	}
	else{
		printf("Home directory inode initialised\n");
		LogWrite("Created node successfully for home directory\n");
	}

	//Initialise the file/dir data block with file information	
	if(syscall_initialise_file_info(curr_inode_num) < 0){
		LogWrite("Initialising file info failed\n");
		return -1;
	}
	else{
		LogWrite("Stat file info initialised successfully\n");
	}


	//Initialise the directory datablock with directory entries (. , .. by default - both have the same inode number in home directory)
	printf("Calling create default dir\n");
	if(syscall_create_default_dir(curr_inode_num) < 0){
		LogWrite("Creating default directories . and .. failed\n");
		return -1;
	}
	else{
		LogWrite("Default directories created successfully\n");
	}

	LogWrite("Created home directory successfully\n");
	//CURR_ROOT_INODE_NUM = curr_inode_num;
	LogWrite("Current root inode updated\n");
	return 1;
}


int main(){

	int choice;
	int trial=1;

	do{
		printf("\nDo you want to reset the file system?\n\tType 1 to Reset\n\tType 0 to continue with old file system\n\t(Note that all files will be lost)\n");
		scanf("%d",&choice);
		if(choice!=1 && choice!=0) 
			printf("\nInvalid Choice Try again\n\tType 1 to Reset\n\tType 0 to continue with old file system\n");
		else trial =0;
	}while(trial==1);

	if(choice==1){
		printf("\nReset File system");
		/*
		here argument to syscall_format is 1, indicating reset
		*/
		int formatret=syscall_format(1);
		if(formatret!=1){
			LogWrite("Syscall Format failed");
		}
	}
	else {
		printf("\nOld File System");
		int formatret=syscall_format(0);
		if(formatret!=1){
			LogWrite("Syscall Format failed");
		}
	}
	disk_attributes();
	syscall_mount();
	initialise_free_block_bitmap();
	printf("initialising home directory ... \n");
	initialise_homeDir();
	syscall_create_Inode();
	//syscall_delete_Inode(134);
	
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
