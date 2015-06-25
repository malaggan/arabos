#include <file_t.h>

// #include <boost/algorithm/string.hpp>
// #include <libgen.h>

int sfs_symlink(char const *target,char const *linkpath)
{
    printk(DEBUG "%s:%d target = %s, linkpath = %s\n",__FUNCTION__,__LINE__,target,linkpath);

    if(ROOT.find(linkpath)>0)
	return EEXIST;

    auto symlink_index = hd.search(sizeof(block_t)*3);

    hd.blocks[*(symlink_index.begin())].change_type<file_t>(basename(strdup(linkpath)), 0777,file_type::F,
							    //*(++symlink_index.begin())
							    *(symlink_index.begin()) // WARN: additio does not make sense here
							    , true);
    hd.blocks[
	// *(++symlink_index.begin())
	*(symlink_index.begin()) // WARN: addition does not make sense here
	].change_type<inode_t>();
    hd.blocks[symlink_index.back()].change_type<data_t>();
    hd.blocks[symlink_index.front()].get<file_t>().add(symlink_index.back());

    aos::copy(target, target+strnlen(target,10000), aos::back_inserter(hd.blocks[symlink_index.back()].get<data_t>().data));
    
    int parent = ROOT.find(dirname(strdup(linkpath)));

    if(parent==-1)
    	return -1;

    hd.blocks[parent].get<file_t>().add(symlink_index.front());
    
    return 0;
}
