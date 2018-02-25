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

}