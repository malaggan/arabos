#include <printf.h>
#include <string.h>
#include <debug.h>
#include <stl/iterator.hh>
#include <file_t.h>
#include <ecma48.h>

void identify_drive();
void readline(char* buf, int max);
char* alloc_page();

void print_monitor_help()
{
	printf("This is the ArOS monitor shell.\n"
	       "Commands:\n"
	       "alloc_page - allocates a new page\n"
	       "h - shows this screen\n"
	       "quit - quits the monitor shell\n"
	       "pst - print stack trace (of the monitor itself)");

}

aos::static_vector<aos::string<20>,10> dirs;
int sfs_create (char const *path, mode_t mode, int /*out*/ *file_handle);
int sfs_rename (const char *path, const char *newpath);
int sfs_link (const char *oldpath, const char *newpath);
int sfs_symlink(char const *target,char const *linkpath);
int sfs_open (const char *path, int /*out*/ *file_handle);
int sfs_read (const char *path, char *buf, size_t size, off_t offset, int file_handle);
int sfs_write (const char *path, const char  *buf, size_t  size, off_t  offset,int file_handle);
int sfs_readlink (const char *path, char *buf/*, size_t size*/);
int sfs_release (const char *path, int file_handle);
int sfs_truncate (const char *path, off_t size);
int sfs_chmod (const char *path, mode_t mode);
int sfs_chown (const char *path, uid_t uid, gid_t gid);
int sfs_rmdir(const char *path);
int sfs_unlink (const char *path);
int sfs_mkdir(const char *path , mode_t mode);
aos::static_vector<aos::string<20>,10> sfs_readdir (const char *path);
int sfs_ls(char const  *path);
int sfs_rmdir(const char *path);
int sfs_unlink (const char *path);


void monitor()
{
	char cmd[100];

	identify_drive();
	printf("abdelrhman test\n");
	ROOT.change_type<file_t>("/",0777,file_type::D,1,false);
	convert_to_write(ROOT,0);
	convert(ROOT,0);
	printk (DEBUG  "cppman ROOT.name:%s\n",ROOT.file.name.c_str());

	printf("The MONITOR ! - type h for help");

	for(;;)
	{
		printf("\n%% ");
		*cmd=0;
		readline(cmd,99);

		int fh;//file handle
		char bufwrite[100]="TTT";//buffer of write
		char bufread[100]={0};//buffer for read
		char buflink[100]={0};//buffer for symlinks

		// handle the command
		if(!strncmp(cmd,"alloc_page",10))
			printf("0x%x\n",alloc_page());
		else if(!strncmp(cmd,"h",1))
			print_monitor_help();
		else if(!strncmp(cmd,"pst",3))
			print_stack_trace();
		else if(!strncmp(cmd,"quit",4))
			return;
		else if(!strncmp_prefix(cmd,"create"))
			sfs_create(ROOT.file.split(cmd," ")[1].c_str(),0777,&fh);
		else if(!strncmp_prefix(cmd,"rename"))
		{
			auto strs=ROOT.file.split(cmd," ");
			sfs_rename(strs[1].c_str(),strs[2].c_str());
		}
		else if(!strncmp_prefix(cmd,"hardlink"))
		{
			auto strs=ROOT.file.split(cmd," ");
			sfs_link(strs[1].c_str(),strs[2].c_str());
		}
		else if(!strncmp_prefix(cmd,"symlink"))
		{
			auto strs=ROOT.file.split(cmd," ");
			sfs_symlink(strs[1].c_str(),strs[2].c_str());
		}
		else if(!strncmp_prefix(cmd,"open"))
		{
			sfs_open(ROOT.file.split(cmd," ")[1].c_str(),&fh);
			printk (DEBUG  "cppman file handle:%d\n",fh);
		}
		else if(!strncmp_prefix(cmd,"read"))
		{
			printk (DEBUG  "cppman bufread:%s\n",bufread);
			sfs_read(ROOT.file.split(cmd," ")[1].c_str(),bufread,1,0,fh);
			printk (DEBUG  "cppman file handle:%d\n",fh);
			printk (DEBUG  "cppman bufread:%s\n",bufread);
		}
		else if(!strncmp_prefix(cmd,"write"))
		{
			printk (DEBUG  "cppman bufwrite:%s\n",bufwrite);
			sfs_write(ROOT.file.split(cmd," ")[1].c_str(),bufwrite,3,0,fh);
			printk (DEBUG  "cppman file handle:%d\n",fh);
			printk (DEBUG  "cppman bufwrite:%s\n",bufwrite);
		}
		else if(!strncmp_prefix(cmd,"rlink"))
		{
			printk (DEBUG  "cppman buflink:%s\n",buflink);
			sfs_readlink(ROOT.file.split(cmd," ")[1].c_str(),buflink);
			printk (DEBUG  "cppman buflink:%s\n",buflink);
		}
		else if(!strncmp_prefix(cmd,"mkdir"))
			sfs_mkdir(ROOT.file.split(cmd," ")[1].c_str(),0777);
		else if(!strncmp_prefix(cmd,"ls"))
			sfs_ls(ROOT.file.split(cmd," ")[1].c_str());
		else if(!strncmp_prefix(cmd,"rmdir"))
			sfs_rmdir(ROOT.file.split(cmd," ")[1].c_str());
		else if(!strncmp_prefix(cmd,"unlink"))
			sfs_unlink(ROOT.file.split(cmd," ")[1].c_str());
		else if(!strncmp_prefix(cmd,"clean"))
		{
			block_t block;
			block.tag=block_type::free;
			for(int i=1;i<100;i++)
				convert_to_write(block,i);           //////clean hard disk except root ////////////////
			block_t iROOT;
			iROOT.change_type<inode_t>();
			convert_to_write(iROOT,1);
			printk (DEBUG  "cppman iROOT files size:%d\n",iROOT.inode.index_file.size());
		}
		else if(!strncmp_prefix(cmd,"status")) {
			for(int i=0;i<10;i++) {
				block_t p ;
				convert(p,i);
				if(p.tag==block_type::free) {
					printf("block :"WHITE"%d ",i);
					printf("type:"WHITE"free \n");
				}
				else if(p.tag==block_type::inode) {
					printf("block :"BLUEB"%d ",i );printf("type:"BLUEB"inode");
					printf(" {inode size index :%d}\n",p.inode.index_file.size() );
				}
				else if(p.tag==block_type::file) {
					printf("block :"REDB"%d " ,i);printf("type:"REDB"file");
					printf(" {file name:%s}\n",p.file.name.c_str());
				}
				else if(p.tag==block_type::data) {
					printf("block :"GREENB"%d " ,i);printf("type:"GREENB"data");
					printf(" {first char in data:%c}\n",p.data.data[0]);
				}
				else
					printf(DEBUG"UNDECLERED BLOCK\n" );
			}
		}
		else
			printf("unrecognized montior command\n");
	}
}
