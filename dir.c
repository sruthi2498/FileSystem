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
char *buffer[20];


int dir_readdir(const char *name, void *buffer, fuse_fill_dir_t filler, off_t offset) {

	struct valid_inode_path dir_info;
	dir_info = 	namei(name);

	if(dir_info.found < 0){
		printf("Directory %s not found \n", name);
		LogWrite("Directory not found\n");
		return -1;
	}

	//Get parent directory	
	int parent_inode = dir_info.valid_inode;

	//Read the block containing the inode information
	struct syscall_inode Inode = ReadInode(parent_inode);
	int curr_dirent_block_num = Inode.direct[1];
	LogWrite("Read current directory's data block number\n");

	//Read directory entries datablock
	union syscall_block block;
	disk_read(curr_dirent_block_num, &block);

	//Read every entry in directory
	int num_entries = block.dir_entries[0].inode_num;
	
	for(int i=1; i<=num_entries; i++){
		char buff[20];
		strcpy(buff, block.dir_entries[i].entry_name);

		struct stat stat = {
			.st_dev = 0
		};
		stat.st_ino = block.dir_entries[i].inode_num;

		//If name exists, return inode
		filler(buffer, buff, &stat, 0, 0);
	}


	return 1;

}

/*
dir_mkdir
	- Check if directory already exists
	- Prepare data block
	- Prepare Inode (set inode attributes)
	- Initialise data block
	- Write the new data block
	- Write inode
	- Fetch parent inode
	- Add to parent inode and write it
*/
int dir_mkdir(char * name){
	
	//Check if directory exists
	struct valid_inode_path dir_info;
	dir_info = 	namei(name);

	if(dir_info.found >= 0){
		printf("Directory %s already exists \n", name);
		LogWrite("Directory already exists\n");
		return -1;
	}

	//Get parent directory	
	int parent_inode = dir_info.valid_inode;

	//Create an inode
	int new_inode = syscall_create_Inode();

	//Initialise default directories . and .. for new directory
	syscall_create_default_dir(parent_inode, new_inode);

	//Initialise file info
	syscall_initialise_file_info(new_inode, S_IFDIR);

	//Add entry to parent directory (note: not_found_entry contains the filename)
	syscall_add_entry_dir(parent_inode, dir_info.not_found_entry, new_inode);

	return 1;
	/*
	else{
		syscall_create_Inode();
	}*/
}



/*
dir_rmdir
	- Get its inode
		return fail if it does not exist
	- Check type of file
		cannot remove if its not a dir
	- Check if its empty
		
*/
int dir_rmdir(char * name){

	struct valid_inode_path dir_info;

	//Find inode for filepath
	dir_info = 	namei(name);

	//Check if path was found 
	if(dir_info.found < 0){
		printf("Directory %s does not exist \n", name);
		LogWrite("Directory does not exist\n");
		return -1;
	}

	int inodenum;

	//Get valid inode
	inodenum = dir_info.valid_inode;

	//Delete inode
	syscall_delete_Inode(inodenum);

	printf("Directory %s deleted\n", name);

	return 0;

}
