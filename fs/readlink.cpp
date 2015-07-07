#include <file_t.h>

int sfs_readlink (const char *path, char *buf/*, size_t size*/)
{
	printk (DEBUG  "%s:%d %s\n",__FUNCTION__,__LINE__, path);
	int index_file= ROOT.file.find(path);

	if(index_file<0)
		return -EEXIST;
	auto ft = read_block(index_file);
	auto it = read_block(ft.file.inode);
	auto dt = read_block(it.inode.index_file.front());
	aos::copy_n(dt.data.data.begin(),dt.data.data.size(),buf);
	return 0;
}
