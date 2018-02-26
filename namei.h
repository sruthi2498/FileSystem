int namei(const char *path);
int read_component(char * path, char * buffer);
int dir_entry_exists(struct syscall_inode *curr_dir_inode, char *buffer);
int read_component(char * path, char *buffer, int path_offset);