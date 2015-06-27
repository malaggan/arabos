#include <file_t.h>

// #include <libgen.h>
// #include <stl/iterator>
// #include <boost/algorithm/string.hpp>
// #include <boost/container/detail/config_begin.hpp>
// #include <boost/container/vector.hpp>
// #include <boost/aligned_storage.hpp>

int sfs_rename (const char *path, const char *newpath)
{
    int i_file=ROOT.find(path);
    if(i_file==-1)
	return -ENOENT;
    int parent=ROOT.find(dirname(strdup(path)));

    int newfile=ROOT.find(newpath);
    if(newfile>=0)
	return EEXIST;
    int newparent=ROOT.find(dirname(strdup(newpath)));
    hd.blocks[i_file].get<file_t>().name=basename(strdup(newpath));
    hd.blocks[newparent].get<file_t>().add(i_file);
    //aos::static_vector<uint32_t,1> temp;
    auto tmp=aos::find(hd.blocks[hd.blocks[parent].get<file_t>().inode].get<inode_t>().index_file.begin(),
		       hd.blocks[hd.blocks[parent].get<file_t>().inode].get<inode_t>().index_file.end(),i_file);
    hd.blocks[hd.blocks[parent].get<file_t>().inode].get<inode_t>().index_file.erase(tmp,tmp+1);
  
    return 0;

}
