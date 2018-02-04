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
	
}

static int fs_rmdir(const char *path) {
   
}


static struct fuse_operations fs_oper = {
	.getattr	= fs_getattr,
	.readdir	= fs_readdir,
	.open       = fs_open,
	.read		= fs_read,
	.write      = fs_write,
	.mkdir      = fs_mkdir,
	.rmdir      = fs_rmdir,
};

int main(int argc, char *argv[]){
	ResetLog();
	return fuse_main(argc, argv, &fs_oper, NULL);
}