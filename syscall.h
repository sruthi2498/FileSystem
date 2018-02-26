#ifndef syscall_H
#define syscall_H

void syscall_debug();
int  syscall_format(int reset);
int  syscall_mount();

struct syscall_inode ReadInode(int inumber);

int  syscall_create_Inode();
int  syscall_delete_Inode( int inumber );
int  syscall_getsize();

int  syscall_read( int inumber, char *data, int length, int offset );
int  syscall_write( int inumber, const char *data, int length, int offset );

int syscall_initialise_file_info(int inode_num);
int syscall_create_default_dir(inode_num);

#endif
