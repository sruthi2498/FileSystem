
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>

#include "all_include.h"
#include "disk.h"
#include "write_to_log.h"
#include "syscall.h"



int disk_init( const char *filename, int n ) 
{
	LogWrite("Initialising disk\n");
	diskfile = fopen(filename,"r+");
	if(!diskfile) diskfile = fopen(filename,"w+");
	if(!diskfile) return 0;

	ftruncate(fileno(diskfile),n*DISK_BLOCK_SIZE);
	/* ftruncate = truncate diskfile to given size
		here size= n*DISK_BLOCK_SIZE
		In our system, n= 256 (number of blocks)
					  DISK_BLOCK_SIZE = 4kB
	   fileno = returns the file descriptor number of the indicated file
	*/


	nblocks = n;  //number of blocks
	nreads = 0;
	nwrites = 0;

	return 1;
}

int disk_size()
{
	return nblocks;
}

static void sanity_check( int blocknum, const void *data )
{
	
	if(blocknum<0) {
		printf("ERROR: blocknum is negative! : %d\n",blocknum);
		LogWrite("ERROR: blocknum is negative\n");
		abort();
	}

	if(blocknum>=nblocks) {
		printf("ERROR: blocknum (%d) is too big!\n",blocknum);
		LogWrite("ERROR: blocknum is too big\n");
		abort();
	}

	if(!data) {
		printf("ERROR: null data pointer!\n");
		LogWrite("ERROR: null data pointer\n");
		abort();
	}
	LogWrite("Completed sanity_check\n");
}

void disk_read( int blocknum, char *data )
{
	
	sanity_check(blocknum,data);
	//data=malloc(sizeof(char)*DISK_BLOCK_SIZE);
	fseek(diskfile,blocknum*DISK_BLOCK_SIZE,SEEK_SET);
	if(fread(data,DISK_BLOCK_SIZE,1,diskfile)==1) {
		nreads++;
	} 
	else {
		printf("ERROR: couldn't access simulated disk: %s\n",strerror(errno));
		char * error=strdup("ERROR: couldn't access simulated disk: ");
		char * errorno=strdup(strerror(errno));
		strcat(error,errorno);
		strcat(error,"\n");
		LogWrite(error);
		abort();
	}
	LogWrite("Reading from disk completed\n");
}

void disk_write( int blocknum, const char *data )
{
	
	sanity_check(blocknum,data);

	fseek(diskfile,blocknum*DISK_BLOCK_SIZE,SEEK_SET);

	int write_ret=fwrite(data,DISK_BLOCK_SIZE,1,diskfile);
	if(write_ret==1) {
		nwrites++;
	} else {
		//printf("Wrote %d\n",write_ret);
		printf("ERROR: couldn't access simulated disk: %s\n",strerror(errno));
		char * error=strdup("ERROR: couldn't access simulated disk: ");
		char * errorno=strdup(strerror(errno));
		strcat(error,errorno);
		strcat(error,"\n");
		LogWrite(error);
		abort();
	}
	LogWrite("Writing to disk completed\n");
}

void disk_close()
{
	LogWrite("Closing Disk\n");
	if(diskfile) {
		printf("%d disk block reads\n",nreads);
		printf("%d disk block writes\n",nwrites);
		fclose(diskfile);
		diskfile = 0;
	}
}

void disk_attributes(){
	printf("\nNumber of blocks %d\nNumber of reads %d\nNumber of writes %d\nSize of block %d\n",nblocks,nreads,nwrites,DISK_BLOCK_SIZE);

}

