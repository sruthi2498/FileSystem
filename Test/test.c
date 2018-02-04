#define FUSE_USE_VERSION 31

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#ifdef linux
/* For pread()/pwrite()/utimensat() */
#define _XOPEN_SOURCE 700
#endif

#include <stdlib.h>
#include <fuse.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <dirent.h>
#include <errno.h>
#include "test.h"
#include <sys/time.h>
#ifdef HAVE_SETXATTR
#include <sys/xattr.h>
#endif

#define O_WRITE(flags) ((flags) & (O_RDWR | O_WRONLY))
/*
Compile with:
gcc -Wall test.c `pkg-config fuse3 --cflags --libs` -o test
*/

const char *filename;
const char *filecontent;
char * inter_path; 

static void *fs_buf;
static size_t fs_size;


static int fs_getattr(const char *path, struct stat *stbuf,
			 struct fuse_file_info *fi)
{
	printf("\nthis is hello getattr\n");	
	(void) fi;
	int res = 0;
	stbuf->st_uid = getuid();
	stbuf->st_gid = getgid();
	stbuf->st_atime = stbuf->st_mtime = time(NULL);

	memset(stbuf, 0, sizeof(struct stat));
	if (strcmp(path, "/") == 0) {
		stbuf->st_mode = S_IFDIR | 0755;
		stbuf->st_nlink = 2;
	} else if (strcmp(path+1, filename) == 0) {
		stbuf->st_mode = S_IFREG | 0777;
		stbuf->st_nlink = 1;
		stbuf->st_size = strlen(filecontent);
	} else
		res = -ENOENT;

	return res;
}

static int fs_readdir(const char *path, void *buf, fuse_fill_dir_t filler,
    off_t offset, struct fuse_file_info *fi,enum fuse_readdir_flags flags) {

	(void) offset;
	(void) fi;
	(void) flags;
	 if (strcmp(path, "/") != 0)
		return -ENOENT;

	filler(buf, ".", NULL, 0, 0);
	filler(buf, "..", NULL, 0, 0);
	filler(buf,filename, NULL, 0, 0);

	return 0;
}

static int fs_open(const char *path, struct fuse_file_info *fi)
{
	printf("\nthis is hello open\n");	
	if (strcmp(path+1, filename) != 0)
		return -ENOENT;


	return 0;
}



static int fs_resize(size_t new_size)
{
	void *new_buf;

	if (new_size == fs_size)
		return 0;

	new_buf = realloc(fs_buf, new_size);
	if (!new_buf && new_size)
		return -ENOMEM;

	if (new_size > fs_size)
		memset(new_buf + fs_size, 0, new_size - fs_size);

	fs_buf = new_buf;
	fs_size = new_size;

	return 0;
}

static int fs_expand(size_t new_size)
{
	if (new_size > fs_size)
		return fs_resize(new_size);
	return 0;
}



static int fs_do_write(const char *buf, size_t size, off_t offset)
{
	printf("\nin fioc do write");	
	if (fs_expand(offset + size))
		return -ENOMEM;

	memcpy(filecontent, buf, size);

	return size;
}

static int fs_write(const char *path, const char *buf, size_t size,
		      off_t offset, struct fuse_file_info *fi)
{
	printf("\nin fioc write");	
	(void) fi;
	// if (fs_file_type(path) != 2)
	// 	return -EINVAL;

	printf("\nabout to do stuff");
	return fs_do_write(buf, size, offset);

}

static int fs_read(const char *path, char *buf, size_t size, off_t offset,
		      struct fuse_file_info *fi)
{
	printf("\nthis is hello read\n");	
	size_t len;
	(void) fi;
	if(strcmp(path+1, filename) != 0)
		return -ENOENT;

	len = strlen(filecontents);
	if (offset < len) {
		if (offset + size > len)
			size = len - offset;
		memcpy(buf, filecontents + offset, size);
	} else
		size = 0;
	
	return size;
}

static int fs_mkdir(const char *path, mode_t mode)
{
	int res;

	res = mkdir(path, mode);
	if (res == -1)
		return -errno;

	return 0;
}

static void *fs_init(struct fuse_conn_info *conn,
			struct fuse_config *cfg)
{
	printf("\nthis is hello init\n");
	(void) conn;
	cfg->kernel_cache = 1;
	return NULL;
}

static struct fuse_operations fs_oper = {
	.init       = fs_init,
	.getattr	= fs_getattr,
	.readdir	= fs_readdir,
	.open       = fs_open,
	.read		= fs_read,
	.write      = fs_write,
	.mkdir      = fs_mkdir,
};

int main(int argc, char *argv[]){
	struct fuse_args args = FUSE_ARGS_INIT(argc, argv);
	filename = strdup("file1");
	filecontent =strdup( "I'm the content of the only file available there\n");

	return fuse_main(args.argc, args.argv, &fs_oper, NULL);
}