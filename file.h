int file_open(char * pathname,int oflag);

int file_close(int fd);

ssize_t file_read(int fd, void *buf, size_t nbyte, off_t offset);
size_t file_write(int fd, const void *buf, size_t nbyte, off_t offset);