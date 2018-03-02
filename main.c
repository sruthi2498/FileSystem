/*
Compile with:
gcc -Wall main.c `pkg-config fuse3 --cflags --libs` -o main
*/

#include "all_include.h"


const char *filename;
const char *filecontent;
char * inter_path; 

static void *fs_buf;
static size_t fs_size;


static int fs_getattr(const char *path, struct stat *stbuf,
			 struct fuse_file_info *fi)
{
	
}

static int fs_readdir(const char *path, void *buf, fuse_fill_dir_t filler,
    off_t offset, struct fuse_file_info *fi) {

	
}

static int fs_open(const char *path, struct fuse_file_info *fi)
{
    
    //printf("\naaaaa \n%d\n",fi->mode_t);
    int ret_val = file_open(path, fi->flags);
    if (ret_val<0) {
    	fprintf(stderr,"%d\n",errno);
    	return -errno;
    }
    fi->fh = ret_val;
	return 0;
}


static int fs_write(const char *path, const char *buf, size_t size,
		      off_t offset, struct fuse_file_info *fi)
{


}

static int fs_read(const char *path, char *buf, size_t size, off_t offset,
		      struct fuse_file_info *fi)
{
	
}

static int fs_mkdir(const char *path, mode_t mode)
{
	LogWrite("called mkdir");
	  if (dir_mkdir(path) == 0) {
    	return -errno;
    }
return 0;
}

static int fs_rmdir(const char *path) {

}

static int utime(const char * path, struct utimbuf * timebuf){
	//syscall_utime()
}

static struct fuse_operations fs_oper = {
	//.getattr	= fs_getattr,
	//.readdir	= fs_readdir,
	.open       = fs_open,
	//.read		= fs_read,
	//.write      = fs_write,
	.mkdir      = fs_mkdir,
	//.rmdir      = fs_rmdir,
};

int main(int argc, char *argv[]){

	int filesysteminitret=initialise_my_filesystem();
	if(filesysteminitret==0){
		LogWrite("File system could not be initialised\n");
	}

	LogWrite("Going to return fuse_main\n");
	//file_open("abc.txt");
	return fuse_main(argc, argv, &fs_oper, NULL);
}