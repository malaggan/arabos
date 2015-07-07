#include <file_t.h>
#include <ata.h>

//#include <boost/algorithm/string.hpp>

int sfs_create (char const *path, mode_t mode, int /*out*/ *file_handle)
{
	printk (DEBUG  "create %s \n",path);
	//printk (DEBUG  "create root name:%s\n",(ROOT.file.name).c_str());
	auto p = ROOT.file.find(path);
	if(p>=0)
		return -EEXIST;

	auto strs=ROOT.file.split(aos::string<128>{path},aos::string<2>{"/"});
	//printk (DEBUG  "create find str.end :%s\n",(*strs.begin()).c_str());
	//strs.erase(strs.begin());
	auto e = strs.end();

	int parent;
	if(strs.size()==1)
		parent=0;
	else
		parent=ROOT.file.find(strs.begin(), --e);
	printk (DEBUG  "create parent id :%d  ",parent);
	printk (DEBUG  "create find str size :%d  ",strs.size());
	// printk (DEBUG  "create ROOT.name:%s\n",(ROOT.file.name).c_str());
	//printk (DEBUG  "create find str.end :%s\n",(*strs.end()).c_str());
	//printk (DEBUG  "create find str size :%d\n",str.size());

	// file_t *h=(new file_t("",0777,file_type::D,1,false));;
	block_t ftparent;
	convert(ftparent,parent);
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
	convert_to_write(ft,free_index.front());
	block_t it;
	it.change_type<inode_t>();
	convert_to_write(it,free_index.back());
	ftparent.file.add(free_index.front());
	block_t itparent;
	convert(itparent,ftparent.file.inode);
	printk (DEBUG  "create index files in inode of parent size %d \n",itparent.inode.index_file.size());
	/*f->fh */ *file_handle = free_index.front();
	it.inode.usecount++;

	return 0;

}
