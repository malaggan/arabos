#include <file_t.h>

int sfs_release (const char *path, int file_handle)// struct fuse_file_info *f)
{
	printk (DEBUG  "release %s\n", path);
	int result=-ENOENT;
	int file= file_handle;//f->fh;
	auto block = read_block(file);
	int inod_sec=block.file.inode;
	block.read(block.file.inode);
	if(file){
		block.inode.usecount--;
		block.write(inod_sec);
		printk (DEBUG  "release end%s\n", path);
		return 0;
	}
	return result;
}
