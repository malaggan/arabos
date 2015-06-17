#include <file_t.h>

int sfs_release (const char *path, int file_handle)// struct fuse_file_info *f)
{
    printk (DEBUG  "release %s\n", path);
    int result=-ENOENT;
    int file= file_handle;//f->fh;
    if(file){
	hd.blocks[hd.blocks[file].get<file_t>().inode].get<inode_t>().usecount--;
	printk (DEBUG  "release end%s\n", path);
	return 0;
    }
    return result;
}
