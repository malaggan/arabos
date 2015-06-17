#include <file_t.h>

// #include <printk.h>
// #include <algorithm>
// #include <fstream>
// #include<vector>
int sfs_readlink (const char *path, char *buf, size_t size)
{
    printk (DEBUG  "%s:%d %s\n",__FUNCTION__,__LINE__, path);
    int index_file= ROOT.find(path);

    if(index_file<0)
    	return -EEXIST;
    
    aos::copy_n(hd.blocks[hd.blocks[hd.blocks[index_file].get<file_t>().inode].get<inode_t>().index_file.front()].get<data_t>().data.begin()
		,size,buf); 
    return 0;
}
