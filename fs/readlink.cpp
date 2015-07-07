#include <file_t.h>

int sfs_readlink (const char *path, char *buf/*, size_t size*/)
{
	printk (DEBUG  "%s:%d %s\n",__FUNCTION__,__LINE__, path);
	int index_file= ROOT.file.find(path);

	if(index_file<0)
		return -EEXIST;
	block_t ft;
	convert(ft,index_file);
	block_t it;
	convert(it,ft.file.inode);
	block_t dt;
	convert(dt,it.inode.index_file.front());
	aos::copy_n(dt.data.data.begin(),dt.data.data.size(),buf);
	return 0;
}
