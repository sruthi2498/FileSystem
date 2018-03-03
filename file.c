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
	//Find node from pathname
		//Check if file does not exist
		struct valid_inode_path path_inode;
		path_inode = namei(pathname);
		int new_inode_num=path_inode.valid_inode;
		if(path_inode.found > 0){
			printf("Inode of path name %s is  %d\n", pathname, path_inode.valid_inode);
			LogWrite("path_inode.found!!\n");
		}
		else{
			if(!(oflag & O_CREAT)){
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
		if(fd<0 || fd>MAX_FD)return 0;

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
	LogWrite("file_open successful\n");
	return fd;
}

int file_close(int fd){

	if(fd<0 || fd>MAX_FD){
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

	if(fd<0 || fd>MAX_FD){
		LogWrite("Invalid fd\n");
		return 0;
	}
	//get file table entry from user file descriptor;
	int fd_pointer=Open_file_table.fd_entry[fd].fd_pointer;
	struct file_table_entry my_file_table_entry=file_table_entries[fd_pointer];

	//check file accessibility
	//USE my_file_table_entry.mode to check for access

	//set parameters in u area for user address, byte count, I/O to user;
	size_t remaining_bytes, bytes_to_be_copied, read_bytes;
	off_t current_offset_in_block;

	//get inode from file table;
	int inode_num=syscall_find_inodenum_for_fd(fd);
	struct syscall_inode Inode=ReadInode(inode_num);
	if(inode_num<0)return 0;
	union syscall_block Inode_Block;

	int inode_blocknum= calculate_block_for_inode(inode_num);

	//set byte offset in u area from file table offset;
	size_t file_table_offset=my_file_table_entry.file_offset;

	size_t current_size_of_file=syscall_size_of_file_for_inodenum(inode_num);
	if(current_size_of_file<0){
		return 0;
	}

	// request to read at position bigger than size of file
	if(offset>current_size_of_file){
		LogWrite("request to read at position bigger than size of file\n");
		return 0;
	}

	read_bytes = 0;
	remaining_bytes = nbyte; //remaining to be read
	

	int block_num_pos,actual_block_num;
	union syscall_block Block;

	char * temp_buf;

	int copied=0;
	//while (count not satisfied)
	while(remaining_bytes>0){
		//convert file offset to disk block (algorithm bmap);
		current_offset_in_block = offset % DISK_BLOCK_SIZE ;
		printf("current_offset_in_block %d\n",current_offset_in_block);

		block_num_pos=syscall_blocknum_for_offset(offset);
		printf("block_num_pos %d\n",block_num_pos);

		 actual_block_num=Inode.direct[block_num_pos];
		 printf("actual_block_num %d\n",actual_block_num);
		//calculate offset into block, number of bytes to read;
		if (remaining_bytes == nbyte) {
			//finish an offset in the first block to be read
			bytes_to_be_copied = syscall_min(DISK_BLOCK_SIZE - current_offset_in_block, remaining_bytes);
		}
		else {
			// general case
				// read an entire block if remaining_bytes are more than a block
				// else read only remaining bytes
			bytes_to_be_copied = syscall_min(DISK_BLOCK_SIZE, remaining_bytes);
		}

		//if (number of bytes to read is 0)
			/* trying to read end of file */
			//break; /* out of loop */
		if(remaining_bytes<=0){
			break;
		}
		//read block (algorithm breada if with read ahead, algorithm
		//bread otherwise);
		disk_read(actual_block_num,&Block);
	
		//copy data from system buffer to user address;
		syscall_read(Block.data,strlen(Block.data),current_offset_in_block,temp_buf);
		printf("temp_buf %s\n",temp_buf);

		memcpy(buf+copied, temp_buf, strlen(temp_buf)+1);
		printf("actual buf %s\n",buf);
		//update u area fields for file byte offset, read count, address to write into user space;
		copied=copied+strlen(temp_buf);
		remaining_bytes=remaining_bytes-copied;
		offset=offset+copied;		//CHANGE ACCESS TIME IN STAT

		Inode.size=copied;
		Inode_Block.inode[Inode.offset_in_block]=Inode;
		disk_write(inode_blocknum,&Inode_Block);

	}
	
	//update file table offset for next read;
	//return (total number of bytes read);
	return copied;


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
