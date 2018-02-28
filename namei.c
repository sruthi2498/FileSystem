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


/*The current directory is stored in the process u area, and the system
root mode is stored in a global variable.*/

int namei(char *path){
	LogWrite("looking up path ...\n");

	//Check for validity of path
	if((path[0] == NULL) || (path[0] != '/')){
		LogWrite("Invalid pathname\n");
		return -1;
	}

	//Initialize useful variables
	int pathlength = strlen(path);
	int dir_inode_num;
	struct syscall_inode curr_dir;
	int next_inode = ROOT_INODE_NUMBER;

	//Read each pathname component into a buffer
	if(path[1]){
	int i = 1;
		while((path[i] != NULL) && (i<=pathlength) ){

			//Initialize current directory (always root)
			dir_inode_num = next_inode;
			curr_dir = ReadInode(dir_inode_num);

			//Read componenent into buffer (max size for filename is 20)
			char* buffer[20];
			int read_char = read_component(path, &buffer, i);
			//printf("buffer %s ", buffer);
			i += read_char;
			if(read_char < 0){
				LogWrite("Could not resolve name\n");
				return -1;
			}
			

			//Read entries from dir datablock in curr_dir inode (either through dirent structure helper function or stringReads)
			next_inode = dir_entry_exists(curr_dir, buffer);

			//If next_inode is not -1, i.e found, update dir_inode_num

		}
	}
	return next_inode;
	//return next_inode

}

int dir_entry_exists(struct syscall_inode curr_dir_inode, char *buffer){
	
	struct syscall_dirent dir_entry;

	//Read datablock[1] from inode -> datablock 1 is always current directory entries
	union syscall_block block;

	disk_read(curr_dir_inode.direct[1], &block);

	//Create structure to read directory entries
	//Note that 0th entry is number of directory entries
	int num_entries = block.dir_entries[0].inode_num;
	char dir_entry_name[MAX_FD];

	//Loop in array of dirents
	for(int i=1; i<=num_entries; i++){
		strcpy(dir_entry.entry_name, block.dir_entries[i].entry_name);

		//If name exists, return inode
		if(strcmp(buffer, dir_entry.entry_name) == 0){
			return block.dir_entries[i].inode_num;
		}
	}

	return -1;
}


int read_component(char * path, char *buffer, int path_offset){
	int i = path_offset;
	int j = 0;
	while((path[i] != '/') && (path[i] != '\0')){
			buffer[j] = path[i];
			j++;
			i++;
			if(j>19){
				LogWrite("ERR: File name is too long\n");
				return(-1);
			}
		}
	buffer[j]='\0';
	return i;

}