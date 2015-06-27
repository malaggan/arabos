#include <file_t.h>

int sfs_chmod (const char *path, mode_t mode)
{
	int  index_file=ROOT.find(path);
	printk(DEBUG "In chmod \n");
	if(index_file==-1)
		return -EEXIST;

	if(!hd.blocks[hd.blocks[index_file].get<file_t>().inode].get<inode_t>().check_permission(0/*uid*/, 0/*gid*/))
		return -EACCES;
	hd.blocks[index_file].get<file_t>().mode=mode;
	return 0;
}
