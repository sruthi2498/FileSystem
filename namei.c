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

int namei(const char *path){
	LogWrite("looking up path");

	//Check for validity of path
	if(path == NULL || path[0] != '/'){
		LogWrite("Invalid pathname");
		return -1;
	}
	
	//Initialize current directory
	int dir_inode_num = ROOT_INODE_NUMBER;
	struct syscall_inode curr_dir;
	curr_dir = ReadInode(dir_inode_num);
	
	//Read each pathname component into a buffer
	int i = 1;
	while((path[i] != NULL) ){

		//Read componenent into buffer (max size for filename is 20)
		char* buffer[20];
		int read_char = read_component(path, &buffer);
		if(read_char < 0){
			LogWrite("Could not resolve name\n");
		}
		
		//Read entries from dir datablock in curr_dir inode (either through dirent structure helper function or stringReads)

		//Strcmp current directory entries with buffer

			//If found, update dir_inode_num


		//Further look at Unix Maurice book

	}

}

int read_component(char * path, char * buffer, int path_offset){
	int i = path_offset;
	int j = 0;
	while((path[i] != '\\') && (path[i] != NULL)){
			buffer[j] = path[i];
			j++;
			i++;
			if(j>19){
				LogWrite("ERR: File name is too long\n");
				return(-1);
			}
		}
	return i;

}