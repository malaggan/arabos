#include <file_t.h>

int sfs_release (const char *path, int file_handle)// struct fuse_file_info *f)
{
	printk (DEBUG  "release %s\n", path);
	int result=-ENOENT;
	int file= file_handle;//f->fh;
	block_t block;
	convert(block,file);
	int inod_sec=block.file.inode;
	convert(block,block.file.inode);
	if(file){
		block.inode.usecount--;
		convert_to_write(block,inod_sec);
		printk (DEBUG  "release end%s\n", path);
		return 0;
	}
	return result;
}
