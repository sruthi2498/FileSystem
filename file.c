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

int file_open(char * pathname,int oflag){

	LogWrite("file_open called\n");
	printf(1,"in file_open %s\n", pathname);
	//Find node from pathname
		//Check if file does not exist
		struct valid_inode_path path_inode;
		path_inode = namei(pathname);
		int new_inode_num=path_inode.valid_inode;
		if(path_inode.found > 0){
			printf("Inode of path name %s is  %d\n", pathname, path_inode.valid_inode);
		}
		else{
			
			if((oflag & O_CREAT)!=1){
				printf("%s not found!\n", path_inode.not_found_entry);
				LogWrite("File not found\n");
				return 0;
			}
			else{
				new_inode_num=syscall_create_Inode();
				if(new_inode_num<0) return 0;
				syscall_initialise_file_info(new_inode_num, S_IFREG);

			}

		}
		LogWrite("Path name resolved to inode number\n");

		int index_in_file_table_entry=syscall_assign_filetable(new_inode_num);
		if(index_in_file_table_entry<0)return 0;

		int fd=syscall_find_next_free_file_descriptor();
		if(fd<0)return 0;

		// allocate file descriptor entry, set pointer to file table entry;
		Open_file_table.fd_entry[fd].fd_pointer=index_in_file_table_entry;
		Open_file_table.fd_entry[fd].fd=fd;

		Open_file_table.count_used_file_descriptors++;
	// allocate file table entry for node, initialize count, offset;
	
		file_table_entries[index_in_file_table_entry].inode_num=new_inode_num;
	
	//CHECK OFFSET (for diff flags)
		file_table_entries[index_in_file_table_entry].file_offset=0;


	// if (type of open specifies truncate file)
		//free all file blocks (algorithm free);
	if(oflag & O_TRUNC)
		syscall_free_datablock_for_inode(new_inode_num);

	//return (file descriptor);

	return fd;
}

int file_close(int fd){

	if(fd<0){
		LogWrite("Invalid fd\n");
		return 0;
	}
	int fd_pointer=Open_file_table.fd_entry[fd].fd_pointer;

	//free file descriptor in free table
	free_file_desc[fd]=0;

	//free inode_num in filetable entry
	file_table_entries[fd_pointer].inode_num=-1;

	return 1;

}

ssize_t file_read(int fd, void *buf, size_t nbyte, off_t offset){

}


size_t file_write(int fd, void *buf, off_t offset){

	//get file table entry from fd

	//Check file flags

	//Set byte_count

	//Get mode from filetable

	//Set byte offset from file table offset

	//Call bmap to find block and block offset corresponding to this offset

	int i = 0;
	//Writing to offset while more data is left

		//Read the block

		//Write from calculated offset till end of block

		// write the buffer block to disk

	//return byte_count;

}
