#include <file_t.h>

//#include <boost/algorithm/string.hpp>

int sfs_create (char const *path, mode_t mode, int /*out*/ *file_handle)
{
    printk (DEBUG  "create %s \n",path);
    auto p = ROOT.find(path);
    if(p>=0)
	return -EEXIST;
     
    aos::list<aos::string> strs;
    boost::split(strs, path, boost::is_any_of("/")); 
    strs.erase(strs.begin());
    auto e = strs.end();
   
    int parent;
    if(strs.size()==1)
	parent=0;
    else
	parent=ROOT.find(strs.begin(), --e);

    if(!hd.blocks[parent].file.isDirectory()){ 
	printk (DEBUG  "create 2 \n");
	return -ENOTDIR;
    }

    aos::list<uint32_t> free_index=hd.search(2*sizeof(block_t));
    if(free_index.size() !=2)
	return -EEXIST;
    
    hd.blocks[free_index.front()].change_type<file_t>(*strs.rbegin(),mode,file_type::F,free_index.back(),false);
    hd.blocks[free_index.back()].change_type<inode_t>();


    hd.blocks[parent].file.add(free_index.front());
    /*f->fh*/ *file_handle = free_index.front();
    hd.blocks[free_index.back()].inode.usecount++;	

    return 0;
    
}
