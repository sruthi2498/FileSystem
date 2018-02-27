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

int syscall_find_next_free_file_descriptor();
int syscall_initial_filetable();
int syscall_assign_filetable();

int syscall_find_fd_for_inodenum(int inode_num);

#endif
