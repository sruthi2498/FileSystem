int init_superblock();

int initialise_empty_inodes(int reset);
void inode_atttributes_given_inodenumber(int inodenumber);
void inode_atttributes_given_inode(struct syscall_inode Inode);


int calculate_block_for_inode(int inodenumber);
int calculate_offset_in_block(int inodenumber,int blocknum); 

void initialise_homeDir();

