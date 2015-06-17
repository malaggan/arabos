#include <file_t.h>

int sfs_chmod (const char *path, mode_t mode)
{
    int  index_file=ROOT.find(path);
    printk(DEBUG "In chmod \n");
    if(index_file==-1)
	return -EEXIST;

    if(!hd.blocks[hd.blocks[index_file].get<file_t>().inode].get<inode_t>().check_permission(fuse_get_context()->uid, fuse_get_context()->gid))
	return -EACCES;
    hd.blocks[index_file].get<file_t>().mode=mode;
    return 0;
}
