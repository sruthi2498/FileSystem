int init_superblock();

int initialise_empty_inodes();
void inode_atttributes_given_inodenumber(int inodenumber);

int calculate_block_for_inode(int inodenumber);
int calculate_offset_in_block(int inodenumber,int blocknum); 


