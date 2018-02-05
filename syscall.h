#ifndef syscall_H
#define syscall_H

void syscall_debug();
int  syscall_format();
int  syscall_mount();


int  syscall_create();
int  syscall_delete( int inumber );
int  syscall_getsize();

int  syscall_read( int inumber, char *data, int length, int ofsyscallet );
int  syscall_write( int inumber, const char *data, int length, int ofsyscallet );

#endif
