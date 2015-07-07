#include <file_t.h>
#include <ata.h>

//#include <boost/algorithm/string.hpp>

int sfs_create (char const *path, mode_t mode, int /*out*/ *file_handle)
{
	auto p = ROOT.file.find(path);
	if(p >= 0)
		return -EEXIST;

	auto strs=ROOT.file.split(aos::string<128>{path},aos::string<2>{"/"});

	auto e = strs.end();

	int parent;
	if(strs.size()==1)
		parent=0;
	else
		parent=ROOT.file.find(strs.begin(), --e);
	printk (DEBUG  "create parent id :%d  ",parent);
	printk (DEBUG  "create find str size :%d  ",strs.size());

	block_t ftparent = read_block(parent);
	if(ftparent.file.type==file_type::D)
		printk (DEBUG  "create ft.file.isDirectory():   ");
	printk (DEBUG  "create parent name:%s\n",ftparent.file.name.c_str());
	if(!ftparent.file.isDirectory()){
		printk (DEBUG  "create 2 \n");
		return -ENOTDIR;
	}

	auto free_index=hd.search(2*sizeof(block_t));
	printk (DEBUG  "create free size %d \n",free_index.size());
	if(free_index.size() !=2){
		printk (DEBUG  "create 3 \n");
		return -EEXIST;
	}

	printk (DEBUG  "create (((free:%d,free2:%d))) \n",free_index.front(),free_index.back());

//    uint16_t* f=reinterpret_cast<uint16_t*>(&(hd.blocks[free_index.front()].change_type<file_t>(*strs.rbegin(),mode,file_type::F,free_index.back(),false)));
	//  hd.blocks[free_index.back()].change_type<inode_t>();
	//write(f,free_index.front());
	block_t ft;
	ft.change_type<file_t>(*strs.rbegin(),mode,file_type::F,free_index.back(),false);
	ft.write(free_index.front());
	block_t it;
	it.change_type<inode_t>();
	it.write(free_index.back());
	ftparent.file.add(free_index.front());
	block_t itparent = read_block(ftparent.file.inode);
	printk (DEBUG  "create index files in inode of parent size %d \n",itparent.inode.index_file.size());
	*file_handle = free_index.front();
	it.inode.usecount++;

	return 0;

}
