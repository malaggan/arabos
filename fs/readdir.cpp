#include <file_t.h>

aos::static_vector<aos::string<20>,10> sfs_readdir (const char *path)
{
	printk (DEBUG  "readdir :%s\n",path);

	int parent;
	parent=ROOT.file.find(path);
	aos::static_vector<aos::string<20>,10> temp;
	block_t bt;
	convert(bt,parent);
	if(!(bt.file.isDirectory()))
		return temp;
	printk (DEBUG  "readdir2 :\n");

	block_t it;
	convert(it,bt.file.inode);
	aos::static_vector<uint32_t,94> file_inode=it.inode.index_file;
	printk (DEBUG  "readdir3 the size of file_inode%d:\n",file_inode.size());

	temp.push_back(".");
	temp.push_back("..");
	for(unsigned int i=0;i<file_inode.size();i++){
		block_t tr;
		convert(tr,file_inode.at(i));
		temp.push_back(tr.file.name);
		printk (DEBUG  "readdir3 the name %s:\n",temp[i].c_str());
//printk (DEBUG  "readdir3 the name %s:\n",tr.file.name.c_str());

	}
	return temp;
}

#if 0

// #include <libgen.h>
// #include <boost/algorithm/string.hpp>

int sfs_readdir (const char *path, void *buf, fuse_fill_dir_t filler, off_t UNUSED offset)
{

	printk (DEBUG  "readdir :%s\n",path);

	aos::list<aos::string> strs;
	boost::split(strs, path, boost::is_any_of("/"));
	strs.erase(strs.begin());
	auto e = strs.end();
	int parent;
	parent=ROOT.find(path);
	printk (DEBUG  ">>>>>>>>>>>>>>>>>>>>>>>>>>>>readdir :%d\n",parent);
	if(!(hd.blocks[parent].get<file_t>().isDirectory()))
		return -ENOENT;
	printk (DEBUG  ">>>>>>>>>>>>>>>>>>>>>>>>>>>>readdir :%d\n",parent);
	aos::static_vector<uint32_t,94> file_inode=hd.blocks[hd.blocks[parent].get<file_t>().inode].get<inode_t>().index_file;
	filler (buf, ".", nullptr, 0);
	filler (buf, "..", nullptr, 0);

	// for(auto kv :file_inode)
	//  filler(buf,reinterpret_cast<char*>(hd.blocks[(*file_inode.begin()++)].file.name), nullptr, 0);

	for(unsigned int i=0;i<file_inode.size();i++){
		filler(buf,hd.blocks[file_inode.at(i)].get<file_t>().name.c_str(), nullptr, 0);
		printk (DEBUG  "readdir in for loop file_inode[%d]=%d  \n",i,file_inode.at(i));
	}
	/*for(unsigned int i=0,j=*file_inode.begin();i<file_inode.size();i++,j++){
	  filler(buf,hd.blocks[j].get<file_t>().name.c_str(), nullptr, 0);
	  printk (DEBUG  "readdir in for loop file_inode[%d]=%d  \n",i,j);
	  }*/
	return 0;
}

//for(auto& kv : *dir->children)
//  filler(buf, kv.first.c_str(), nullptr, 0);





/*int sfs_readdir (const char *path, void *buf, fuse_fill_dir_t filler, off_t UNUSED offset, struct fuse_file_info UNUSED *fi)
  {

  printk (DEBUG  "readdir \n");

  auto dir= ROOT.file.find(path);

  if(!(dir && dir->isDirectory()))
  return -ENOENT;

  filler (buf, ".", nullptr, 0);
  filler (buf, "..", nullptr, 0);
  for(auto& kv : *dir->children)
  filler(buf, kv.first.c_str(), nullptr, 0);

  return 0;
  }*/
#endif
