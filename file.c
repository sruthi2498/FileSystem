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

    struct syscall_inode Inode=ReadInode(new_inode_num);

	// if (type of open specifies truncate file)
		//free all file blocks (algorithm free);
	if(oflag & O_TRUNC)
		syscall_free_datablock_for_inode(new_inode_num);
	if (oflag & O_WRONLY) {
		// Check for write permissions here
		file_table_entries[index_in_file_table_entry].mode=O_WRONLY;
	}
	else if (oflag & O_RDWR) {
		// Check for read/write permissions here
		file_table_entries[index_in_file_table_entry].mode=O_RDWR;
	}
	else {
		file_table_entries[index_in_file_table_entry].mode=O_RDONLY;
	}

	if (oflag & O_APPEND) {
		if (file_table_entries[index_in_file_table_entry].mode==O_WRONLY || 
			file_table_entries[index_in_file_table_entry].mode==O_RDWR) {
			
			size_t current_size_of_file=Inode.size;
			file_table_entries[index_in_file_table_entry].file_offset=current_size_of_file;

		}
		else {
			errno = EACCES;
			return -1;
		}
	}
	else {
		file_table_entries[index_in_file_table_entry].file_offset=0;

	}	
	printf("\nmode of file %d\n",file_table_entries[index_in_file_table_entry].mode);
	printf("\n\t(rd %d, wr %d rdwr %d,a %d)\n",O_RDONLY,O_WRONLY,O_RDWR,O_APPEND);
	//return (file descriptor);

		//CHANGE ACCESS TIME IN STAT
		struct stat my_stat;
		union syscall_block Inode_Block;
		my_stat=syscall_find_stat_for_inodenum(new_inode_num);
		clock_gettime(CLOCK_REALTIME, &my_stat.st_atim);
		int r=syscall_write_stat_to_disk(my_stat,new_inode_num);
		Inode_Block.inode[Inode.offset_in_block]=Inode;
		int inode_blocknum= calculate_block_for_inode(new_inode_num);
		disk_write(inode_blocknum,&Inode_Block);

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
	LogWrite("File closed successfully\n");
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
	if(my_file_table_entry.mode==O_WRONLY){
		LogWrite("Write only file, READ FAILED\n");
		return 0;
	}

	//set parameters in u area for user address, byte count, I/O to user;
	size_t bytes_to_be_copied, read_bytes;
	int remaining_bytes;
	off_t current_offset_in_block;

	//get inode from file table;
	int inode_num=syscall_find_inodenum_for_fd(fd);
	struct syscall_inode Inode=ReadInode(inode_num);
	if(inode_num<0)return 0;
	union syscall_block Inode_Block;

	int inode_blocknum= calculate_block_for_inode(inode_num);

	//set byte offset in u area from file table offset;
	size_t file_table_offset=my_file_table_entry.file_offset;
	//printf("offset %d size %d",offset,Inode.size);
	size_t current_size_of_file=Inode.size;
	if(current_size_of_file<0){
		return 0;
	}
	// request to read at position bigger than size of file
	if(offset>current_size_of_file){
		LogWrite("request to read at position bigger than size of file\n");
		return 0;
	}

	read_bytes = 0;
	remaining_bytes = (int)nbyte; //remaining to be read
	

	int block_num_pos,actual_block_num;
	union syscall_block Block;

	char * temp_buf;
	int r;
	int copied=0;
	struct stat my_stat;
	//while (count not satisfied)
	while(remaining_bytes>=0){
		//printf("\n---------\nremaining bytes %d\n",remaining_bytes);
		//convert file offset to disk block (algorithm bmap);
		current_offset_in_block = offset % DISK_BLOCK_SIZE ;
		//printf("current_offset_in_block %d\n",current_offset_in_block);

		block_num_pos=syscall_blocknum_for_offset(offset);
		//printf("block_num_pos %d\n",block_num_pos);

		 actual_block_num=Inode.direct[block_num_pos];
		// printf("actual_block_num %d\n",actual_block_num);
		//calculate offset into block, number of bytes to read;
		if (remaining_bytes == nbyte){
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
		temp_buf=malloc(sizeof(char)*(bytes_to_be_copied+1));
		//copy data from system buffer to user address;
		r=syscall_read(Block.data,bytes_to_be_copied,current_offset_in_block,temp_buf);
		if(r<1){
			LogWrite("Read failed\n");
			return copied;
		}
		//printf("temp_buf %s\n",temp_buf);
	

		memcpy(buf+copied, temp_buf, strlen(temp_buf)+1);
		//printf("actual buf %s\n",buf);

		//update u area fields for file byte offset, read count, address to write into user space;
		copied=copied+strlen(temp_buf);
		//printf("copied %d\n",copied);
		remaining_bytes=remaining_bytes-copied;
		//printf("new remaining bytes %d\n",remaining_bytes);
		offset=offset+copied;		

		//CHANGE ACCESS TIME IN STAT
		my_stat=syscall_find_stat_for_inodenum(inode_num);
		clock_gettime(CLOCK_REALTIME, &my_stat.st_atim);
		r=syscall_write_stat_to_disk(my_stat,inode_num);
		Inode_Block.inode[Inode.offset_in_block]=Inode;
		disk_write(inode_blocknum,&Inode_Block);


	}
	//printf("\noffset after read %d",offset);
	//update file table offset for next read;
	my_file_table_entry.file_offset=offset;
	file_table_entries[fd_pointer]=my_file_table_entry;

	//return (total number of bytes read);
	return copied;


}


size_t file_write(int fd, void *buf, off_t offset){


	if(fd<0 || fd>MAX_FD){
		LogWrite("Invalid fd\n");
		return 0;
	}

	//get file table entry from fd
	int fd_pointer=Open_file_table.fd_entry[fd].fd_pointer;
	struct file_table_entry my_file_table_entry=file_table_entries[fd_pointer];

	//Check file flags
	//check file accessibility
	//USE my_file_table_entry.mode to check for access
	if(my_file_table_entry.mode!=O_WRONLY || my_file_table_entry.mode!=O_RDWR){
		LogWrite("Read only file, WRITE FAILED\n");
		return 0;
	}
	
	//Set byte_count
	//set parameters in u area for user address, byte count, I/O to user;
	size_t bytes_to_be_copied, read_bytes;
	int remaining_bytes;
	off_t current_offset_in_block;

	//Get mode from filetable


	//get inode from file table;
	int inode_num=syscall_find_inodenum_for_fd(fd);
	struct syscall_inode Inode=ReadInode(inode_num);
	if(inode_num<0)return 0;
	union syscall_block Inode_Block;
	int current_size_of_file=Inode.size;
	if(my_file_table_entry.mode==O_APPEND){
		offset=current_size_of_file;
	}


	int inode_blocknum= calculate_block_for_inode(inode_num);

	//set byte offset in u area from file table offset;
	size_t file_table_offset=my_file_table_entry.file_offset;

	size_t  nbyte=strlen(buf);
	remaining_bytes = (int)nbyte; //remaining to be wriiten
	
	int block_num_pos,actual_block_num;
	union syscall_block Block;

	char * temp_buf;
	int r;
	int copied=0;
	struct stat my_stat;

	//Writing to offset while more data is left
	while(remaining_bytes>=0){
		//printf("\n---------\nremaining bytes %d\n",remaining_bytes);
		//convert file offset to disk block (algorithm bmap);
		//Call bmap to find block and block offset corresponding to this offset
		current_offset_in_block = offset % DISK_BLOCK_SIZE ;
		//printf("current_offset_in_block %d\n",current_offset_in_block);

		block_num_pos=syscall_blocknum_for_offset(offset);
		//printf("block_num_pos %d\n",block_num_pos);

		actual_block_num=Inode.direct[block_num_pos];
		// printf("actual_block_num %d\n",actual_block_num);
		
		//calculate offset into block, number of bytes to read;
		if (remaining_bytes == nbyte) {
			bytes_to_be_copied = syscall_min(DISK_BLOCK_SIZE - current_offset_in_block, remaining_bytes);
		}
		else {
			bytes_to_be_copied = syscall_min(DISK_BLOCK_SIZE, remaining_bytes);
		}

		//if (number of bytes to read is 0)
			/* trying to read end of file */
			//break; /* out of loop */
		if(remaining_bytes<=0){
			break;
		}

		memcpy(temp_buf,buf+copied, strlen(temp_buf)+1);

		//Read the block
		//read block (algorithm breada if with read ahead, algorithm
		//bread otherwise);
		disk_read(actual_block_num,&Block);

		//Write from calculated offset till end of block
		r=syscall_write(Block.data,bytes_to_be_copied,current_offset_in_block,temp_buf);
		if(r<1){
			LogWrite("Write failed\n");
			return 0;
		}
		//printf("temp_buf %s\n",temp_buf);
		// write the buffer block to disk
		disk_write(actual_block_num,&Block);

		//update u area fields for file byte offset, read count, address to write into user space;
		copied=copied+bytes_to_be_copied;
		//printf("copied %d\n",copied);
		remaining_bytes=remaining_bytes-copied;
		//printf("new remaining bytes %d\n",remaining_bytes);
		offset=offset+copied;		

		//CHANGE MODIFICATION TIME IN STAT
		my_stat=syscall_find_stat_for_inodenum(inode_num);
		clock_gettime(CLOCK_REALTIME, &my_stat.st_mtim);
		r=syscall_write_stat_to_disk(my_stat,inode_num);
		Inode.size=copied;
		Inode_Block.inode[Inode.offset_in_block]=Inode;
		disk_write(inode_blocknum,&Inode_Block);


	}
	//return byte_count;
	return copied;


}
