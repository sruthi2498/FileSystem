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
	//Return error if directory doesnt exist

  LogWrite("fs_getattr");
	int inodenum ;
	struct valid_inode_path getinode=namei(path);
	inodenum=getinode.valid_inode;
    stbuf = syscall_lstat(inodenum);
        
    return 0;
	
}

static int fs_readdir(const char *path, void *buf, fuse_fill_dir_t filler,
    off_t offset, struct fuse_file_info *fi) {

	
}

static int fs_open(const char *path, struct fuse_file_info *fi)
{
    
    //printf("\naaaaa \n%d\n",fi->mode_t);
    int ret_val = file_open(path, fi->flags);
    if (ret_val<0) {
    	fprintf(stderr,"%d\n",errno);
    	return -errno;
    }
    fi->fh = ret_val;
	return 0;
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
	LogWrite("called mkdir");
	  if (dir_mkdir(path) < 0) {
    	return -errno;
    }
return 0;
}

static int fs_rmdir(const char *path) {
  LogWrite("called rmdir");
	 if (dir_rmdir(path) < 0) {
    	return -errno;
    }
return 0;
}



static struct fuse_operations fs_oper = {
	.getattr	= fs_getattr,
	//.readdir	= fs_readdir,
	.open       = fs_open,
	//.read		= fs_read,
	//.write      = fs_write,
	.mkdir      = fs_mkdir,
	.rmdir      = fs_rmdir,
};

int main(int argc, char *argv[]){

	int filesysteminitret=initialise_my_filesystem();
	if(filesysteminitret==0){
		LogWrite("File system could not be initialised\n");
	}
	LogWrite("Going to return fuse_main\n");
	
	int fd=file_open("/abc.txt",O_CREAT);
	int inode=syscall_find_inodenum_for_fd(fd);
	struct syscall_inode Inode=ReadInode(inode);

	int b1=Inode.direct[1];
	int b2=Inode.direct[2];
	int b3=Inode.direct[3];

	char * temp=(char *)malloc(sizeof(char)*(DISK_BLOCK_SIZE+1));
	char * temp3=(char *)malloc(sizeof(char)*(DISK_BLOCK_SIZE+1));
	strcpy(temp,"A wiki is run using wiki software, otherwi known as a wiki engine//. A wiki engine is a type of content management system, but it differs from most other such systems, including blog software, in that the content is created without any defined owner or leader, and wikis have little implicit structure, allowing structure to emerge according to the needs of the users.[2] There are dozens of different wiki engines in use, both standalone and part of other software, such as bug tracking systems. Some wiki engines are open source, whereas others are proprietary. Some permit control over different functions (levels of access); for example, editing rights may permit changing, adding or removing material. Others may permit access without enforcing access control. Other rules may be imposed to organize content.The online encyclopedia project Wikipedia is by far the most popular wiki-based website, and is one of the most widely viewed sites of any kind in the world, having been ranked in the top ten since 2007.[3] Wikipedia is not a single wiki but rather a collection of hundreds of wikis, one for each language. There are tens of thousands of other wikis in use, both public and private, including wikis functioning as knowledge management resources, notetaking tools, community websites and intranets. The English-language Wikipedia has the largest collection of articles; as of September 2016, it had over five million articles. Ward Cunningham, the developer of the first wiki software, WikiWikiWeb, originally described it as the simplest online database that could possibly work.[4] Wiki (pronounced [ˈwiki][note 1]) is a Hawaiian word meaning quick.[5][6][7]A wiki enables communities of editors and contributors to write documents collaboratively. All that people require to contribute is a computer, Internet access, a web browser and a basic understanding of a simple markup language (e.g., HTML). A single page in a wiki website is referred to as a wiki page, while the entire collection of pages, which are usually well-interconnected by hyperlinks, is the wik. A wiki is essentially a database for creating, browsing, and searching through information. A wiki allows non-linear, evolving, complex and networked text, while also allowing for editor argument, debate and interaction regarding the content and formatting.[9] A defining characteristic of wiki technology is the ease with which pages can be created and updated. Generally, there is no review by a moderator or gatekeeper before modifications are accepted and thus lead to changes on the website. Many wikis are open to alteration by the general public without requiring registration of user accounts. Many edits can be made in real-time and appear almost instantly online. However, this feature facilitates abuse of the system. Private wiki servers require user authentication to edit pages, and sometimes even to read them. Maged N. Kamel Boulos, Cito Maramba and Steve Wheeler write that the open wikis produce a process of Social Darwinism.  'Unfit' sentences and sections are ruthlessly culled, edited and replaced if they are not considered 'fit', which hopefully results in the evolution of a higher quality and more relevant page. While such openness may invite 'vandalism' and the posting of untrue information, this same openness also makes it possible to rapidly correct or restore a 'quality' wiki page.[10]A defining characteristic of wiki technology is the ease with which pages can be created and updated. Generally, there is no review by a moderator or gatekeeper before modifications are accepted and thus lead to changes on the website. Many wikis are open to alteration by the general public without requiring registration of user accounts. Many edits can be made in real-time and appear almost instantly online. However, this feature facilitates abuse of the system. Private wiki sery the general public without requiring registration of user accounts. Many edits can be made iery the general public without requiring registration of user accounts. Many edits can be made iedits can be made i");
	strcpy(temp3,"A wiki is run using wiki software");
	disk_write(b1, temp);
	disk_write(b2, temp3);
	

	Inode.size=strlen(temp)+strlen(temp3);

	union syscall_block Inode_Block;
	Inode_Block.inode[Inode.offset_in_block]=Inode;
	disk_write(Inode.blocknum,&Inode_Block);
	syscall_display_current_file_info();
	inode_atttributes_given_inodenumber(inode);
	// union syscall_block B;
	// char * t=malloc(sizeof(char)*4096);
	// disk_read(31,t);
	// printf("read : %s\n",t);



	/*char *temp2=(char *)malloc(sizeof(char)*DISK_BLOCK_SIZE);
	int r=file_read(fd, temp2, 20,4092);
	if(r<=0){
		printf("\n0 bytes read\n");
	}
	else printf("\n\nREAD : \n\t %s\n",temp2);*/

	// int r=file_close(fd);

	// fd=file_open("/abc.txt",O_RDWR);
	// printf("\nOpened with O_RDWR\n");
	// inode_atttributes_given_inodenumber(inode);

	// char * temp4="THIS IS GOING TO BE WRITEN";
	// r=file_write(fd, temp4,Inode.size-1 );
	// if(r<=0){
	// 	printf("\n0 bytes written\n");
	// }
	return fuse_main(argc, argv, &fs_oper, NULL);
}
