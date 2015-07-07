#include <file_t.h>

aos::static_vector<aos::string<20>,10> sfs_readdir (const char *path)
{
	printk (DEBUG  "readdir :%s\n",path);

	int parent;
	parent=ROOT.file.find(path);
	aos::static_vector<aos::string<20>,10> temp;
	auto bt = read_block(parent);
	if(!(bt.file.isDirectory()))
		return temp;
	printk (DEBUG  "readdir2 :\n");

	auto it = read_block(bt.file.inode);
	aos::static_vector<uint32_t,94> file_inode=it.inode.index_file;
	printk (DEBUG  "readdir3 the size of file_inode%d:\n",file_inode.size());

	temp.push_back(".");
	temp.push_back("..");
	for(unsigned int i=0;i<file_inode.size();i++){
		auto tr = read_block(file_inode.at(i));
		temp.push_back(tr.file.name);
		printk (DEBUG  "readdir3 the name %s:\n",temp[i].c_str());

	}
	return temp;
}
