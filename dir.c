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
	dir_info = 	namei(name);

	if(dir_info.found < 0){
		printf("Directory %s does not exist \n", name);
		LogWrite("Directory does not exist\n");
		return -1;
	}

	int inodenum;
	inodenum = dir_info.valid_inode;

	syscall_delete_Inode(inodenum);
	return 0;



}
