#include <printf.h>
#include <string.h>
#include <debug.h>
#include <stl/iterator.hh>
#include <file_t.h>
#include <ecma48.h>

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

void monitor()
{
	identify_drive();
	ROOT.change_type<file_t>("/", 0777, file_type::D, 1 /* inode */, false);
	ROOT.write(0);
	ROOT.read(0);

	printf("The MONITOR ! - type h for help");

	for(;;)
	{
		printf("\n%% ");
		char cmd[100] = {0};
		readline(cmd,99);

		int fh;//file handle
		char bufwrite[100] = "TTT";//buffer of write
		char bufread[100] = {0};//buffer for read
		char buflink[100] = {0};//buffer for symlinks

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
			sfs_create(ROOT.file.split(cmd," ")[1].c_str(), 0777, &fh);
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
				block.write(i);           //////clean hard disk except root ////////////////
			block_t iROOT;
			iROOT.change_type<inode_t>();
			iROOT.write(1);
			printk (DEBUG  "cppman iROOT files size:%d\n",iROOT.inode.index_file.size());
		}
		else if(!strncmp_prefix(cmd,"status")) {
			for(int i=0;i<10;i++) {
				block_t p ;
				p.read(i);
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

void status();

void test_fs() {
	device_info_t info = identify_drive();
	size_t total_sectors = info.total_sectors;
	char cmd[100] = {0};

	int fh;//file handle
	char bufwrite[100] = "TTT";//buffer of write
	char bufread[100] = {0};//buffer for read
	char buflink[100] = {0};//buffer for symlinks
#if 0
	// create
	sfs_create("/aaaa", 0777, &fh);
	// rename
	sfs_rename("/aaaa","/bbbb");
	// hardlink
	sfs_link("/aaaa","/bbbb");
	// symlink
	sfs_symlink("/aaaa","/bbbb");
	// open
	sfs_open("/aaaa", &fh);
	printk(DEBUG "cppman file handle: %d\n", fh);
	// read
	printk(DEBUG "cppman bufread: %s\n", bufread);
	sfs_read("/aaaa", bufread, 1, 0, fh);
	printk(DEBUG "cppman file handle: %d\n", fh);
	printk(DEBUG "cppman bufread: %s\n", bufread);
	// write
	printk(DEBUG "cppman bufwrite: %s\n", bufwrite);
	sfs_write("/aaaa", bufwrite, 3, 0, fh);
	printk(DEBUG "cppman file handle: %d\n", fh);
	printk(DEBUG "cppman bufwrite: %s\n", bufwrite);
	// read link

	printk(DEBUG "cppman buflink: %s\n", buflink);
	sfs_readlink("/aaaa", buflink);
	printk(DEBUG "cppman buflink: %s\n",buflink);

	// mkdir
	sfs_mkdir("/aaaa", 0777);
	// ls
	sfs_ls("/aaaa");
	// rmdir
	sfs_rmdir("/aaaa");
	// unlink
	sfs_unlink("/aaaa");
	// clean
	block_t block;
	for(int i = 2; i < total_sectors; i++)
		block.write(i); // clean hard disk except root
	block.change_type<inode_t>(); // inode for the root
	block.write(1);
	printk(DEBUG "cppman iROOT files size: %d\n", iROOT.inode.index_file.size());
#endif

}

void status() {
	for(int i=0; i < /*total_sectors*/ 10; i++) {
		auto p = read_block(i);
		printf("block : ");
		if(p.tag == block_type::free)
			printf(WHITE  "%d " NORMAL "type: " WHITE  "free\n" NORMAL, i);
		else if(p.tag == block_type::inode)
			printf(BLUEB  "%d " NORMAL "type: " BLUEB  "inode"  NORMAL " {inode size index: %d}\n", i, p.inode.index_file.size());
		else if(p.tag == block_type::file)
			printf(REDB   "%d " NORMAL "type: " REDB   "file"   NORMAL " {file name: %s}\n", i, p.file.name.c_str());
		else if(p.tag == block_type::data)
			printf(GREENB "%d " NORMAL "type: " GREENB "data"   NORMAL " {first char in data: %c}\n", i, p.data.data[0]);
		else
			printk(DEBUG "Unknown block type!\n" );
	}
}
