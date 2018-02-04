
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
		printf("ERROR: blocknum (%d) is negative!\n",blocknum);
		abort();
	}

	if(blocknum>=nblocks) {
		printf("ERROR: blocknum (%d) is too big!\n",blocknum);
		abort();
	}

	if(!data) {
		printf("ERROR: null data pointer!\n");
		abort();
	}
}

void disk_read( int blocknum, char *data )
{
	LogWrite("Reading from disk\n");
	sanity_check(blocknum,data);

	fseek(diskfile,blocknum*DISK_BLOCK_SIZE,SEEK_SET);

	if(fread(data,DISK_BLOCK_SIZE,1,diskfile)==1) {
		nreads++;
	} else {
		printf("ERROR: couldn't access simulated disk: %s\n",strerror(errno));
		char * error=strdup("ERROR: couldn't access simulated disk: ");
		char * errorno=strdup(strerror(errno));
		strcat(error,errorno);
		strcat(error,"\n");
		LogWrite(error);
		abort();
	}
}

void disk_write( int blocknum, const char *data )
{
	LogWrite("Writing to disk\n");
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
	printf("\nNumber of blocks %d\nNumber of reads %d\nNumber of writes %d\n",nblocks,nreads,nwrites);

}

int main(){

	ResetLogFile();
	int disk_init_return = disk_init("memory_file.dat",5);
	disk_attributes();
	char * string="aaaa";
	//disk_write(0, string );
	disk_read(0,string);
	disk_attributes();
	disk_close();
}
