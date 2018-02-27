# FileSystem
VFS using Fuse

# To execute
    $make
    $./initialise

# What is FUSE?

    ->VFS and FUSE are related, but not quite the same thing. The main purpose of FUSE is to turn things-that-are-almost-like-files-but-not-quite (such as files on a remote server, or inside a ZIP file) into "real" directories and files. See the roster of FUSE filesystems to get an idea of what this is good for; this hopefully will make it clearer why FUSE beats "plain old files" in a lot of circumstances.
    ->It provides a way to intercept file system calls issued by Linux programs and to redirect
    the program flow into a handler (daemon running as a user-level process) function.
    ->It also helps to mount the file system.
    ->FUSE helps you in writing a virtual file system.
    
# Workflow
    main.c 
        functions start with fs_ 
        Intergrated with fuse (set fuse operations)
        Call syscall functions
    syscall.c
        functions start with syscall_ 
        Do all Inode operations
        Call disk functions
    disk.c
        functions start with disk_
        Do disk reads and writes
        
    
# Files 
    disk.h -Disk emulator
    disk.c -Disk emulator
    syscall.c - System Calls
    syscall.h - System Calls
    initialise.c -Initialise disk and inodes
    initialise.h -Initialise disk and inodes
    log.txt - Log file
    write_to_log.h - Writing to a log file
    write_to_log.c - Writing to a log file

# Functions in each file 
    
    "dir.c"
        -
        -
        -
    "file.c"
        -
        -
        -
    "disk.c"
        -int disk_init( const char *filename, int n ) 
            initialises the disk 
        -int disk_size()
            returns the number of blocks
        -static void sanity_check( int blocknum, const void *data )
            checks for the legality of the disk blocks
        -void disk_read(
         int blocknum, void*data )
            Disk read block by block
        -void disk_write( int blocknum, const void *data )
            Disk write block by block
        -void disk_close()
            closing the disk
        -void disk_attributes(){
            prints all the attributes like nblocks etc.

    "initialise.c"
        -int init_superblock()
            creates superblock
        -int calculate_block_for_inode(int inodenumber)
            Given an inode number calculate block number
        -int calculate_offset_in_block(int inodenumber,int blocknum)
            Given inode number and blocknumber, calculate offset
        -initialise_free_block_bitmap()
            Set all datablocks to -1 in free_block_bitmap
        -int initialise_empty_inodes(int reset)
            Go through all inodes,Caculate block number and offset for them add it to i-list
        -void inode_atttributes_given_inodenumber(int inodenumber)
            print the attributes for that inode number
        -void inode_atttributes_given_inode(struct syscall_inode Inode)
            print the attributes for that inode 
        -void initialise_homeDir()
            Initialising a parent root directory (HOME)
        -int main()
            main driver function
    
    "main.c"
        -static int fs_getattr(const char *path, struct stat *stbuf,struct fuse_file_info *fi)
            get attribute of a file fuse implementation.
        -static int fs_readdir(const char *path, void *buf, fuse_fill_dir_t filler,off_t offset, struct fuse_file_info *fi) 
            read a directory using fuse
        -static int fs_open(const char *path, struct fuse_file_info *fi)
            open a file using fuse
        -static int fs_write(const char *path, const char *buf, size_t size,off_t offset, struct fuse_file_info *fi)
            write into a file using fuse
        -static int fs_read(const char *path, char *buf, size_t size, off_t offset,struct fuse_file_info *fi)
            read a file using fuse
        -static int fs_mkdir(const char *path, mode_t mode)
            make a directory using fuse
        -static int fs_rmdir(const char *path)
            remove a directory
        -static int utime(const char * path, struct utimbuf * timebuf)
            utime using fuse

    "syscall.c"
        -int syscall_format(int reset)
            Creates a new filesystem on the disk, destroying any data already present
        -void syscall_debug()
            Report on how the inodes and blocks are organized
        -int syscall_mount()
            Examine the disk for a filesystem
        -int syscall_create_Inode()
            Create a new inode of zero length
        -int find_free_datablock()
            Find an available free datablock from free_block_bitmap
        -int syscall_initial_filetable()
            initialise all inode_numbers to -1.
        -int syscall_assign_filetable()
            check the first available filetable entry ,assign it and declare it's inode number
        -struct syscall_inode ReadInode(int inumber)
            Read specified inode from disk
        -int syscall_delete_Inode( int inumber )
            Delete the inode indicated by the inumber
        -int syscall_getsize( int inumber )
            Return the logical size of the given inode, in bytes
        -int syscall_find_next_free_file_descriptor()
            go through free_file_desc find next free one
        -int syscall_read( int inumber, char *data, int length, int offset )
            Read data from a valid inode
        -int syscall_write( int inumber, const char *data, int length, int offset )
            Write data to a valid inode
            
