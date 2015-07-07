#include <file_t.h>

int sfs_chmod (const char *path, mode_t mode)
{
	int  index_file=ROOT.file.find(path);
	printk(DEBUG "In chmod \n");
	if(index_file==-1)
		return -EEXIST;
	block_t ft;
	convert(ft,index_file);
	block_t it;
	convert(it,ft.file.inode);
	if(!/*hd.blocks[hd.blocks[index_file].get<file_t>().inode].get<inode_t>()*/it.inode.check_permission(0/*uid*/, 0/*gid*/))
		return -EACCES;
	/*hd.blocks[index_file].get<file_t>()*/ft.file.mode=mode;
	convert_to_write(ft,index_file);
	return 0;
}
