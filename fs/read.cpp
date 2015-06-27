#include <file_t.h>

// #include <string.h>
// #include <cstring>
// #include <stl/iterator>
// #include <vector>

int sfs_read (const char UNUSED *path, char *buf, size_t size, off_t offset, int file_handle)//struct fuse_file_info *f)
{
	int index=file_handle; //f->fh;
	// file_t file;
	//inode_t inode=hd.blocks[hd.blocks[index].file.inode].inode;
	printk(DEBUG "In READ the index file number1:%d \n",index);
	printk(DEBUG "In read the size of index_files: %d\n",hd.blocks[hd.blocks[index].get<file_t>().inode].get<inode_t>().index_file.size());

	// printk(DEBUG "IN READ the content of the block:%d",hd.blocks[hd.blocks[hd.blocks[index].get<file_t>().inode].get<inode_t>().index_file[0//]].get<data_t>().data.empty());

	/*printk(DEBUG "IN READ the content of the block:%s",hd.blocks[hd.blocks[hd.blocks[index].get<file_t>().inode].get<inode_t>().index_file[0]].get<data_t>().data);*/

	printk(DEBUG "In READ number of blocks are %d\n",hd.blocks[hd.blocks[index].get<file_t>().inode].get<inode_t>().index_file.size());
	aos::vector<char>  data;
	for(unsigned int i =0;
	    i < hd.blocks[hd.blocks[index].get<file_t>().inode].get<inode_t>().index_file.size();
	    i++)
	{
		for(unsigned int j = 0;
		    j<hd.blocks[hd.blocks[hd.blocks[index].get<file_t>().inode].get<inode_t>().index_file[i]].get<data_t>().data.size();
		    j++){
			// printk(DEBUG "In READ 1 the content of buffer:%c\n",hd.blocks[hd.blocks[hd.blocks[index].get<file_t>().inode].get<inode_t>().index_file[i]].get<data_t>().data[j]);
			data.push_back( hd.blocks[hd.blocks[hd.blocks[index].get<file_t>().inode].get<inode_t>().index_file[i]].get<data_t>().data[j]);
		}
	}
	printk(DEBUG "In read fh is %p\n",reinterpret_cast<void*>(index));
	// printk(DEBUG "In READ FILE NAME IS %s\n",file->name.c_str());
	// printk(DEBUG "In READ 1 =%d\n",file->usecount);

	if(index==-1)
		return -ENOENT;


	printk(DEBUG "In READ 1 \n");


	{
		// aos::unique_lock<aos::mutex> lk(hd.blocks[index].get<file_t>().mutex);

		printk(DEBUG "In READ  before wait\n");
		//ROOT.cond.wait(lk,[&]{return !( ( hd.blocks[hd.blocks[index].get<file_t>().inode].get<inode_t>().writecount )> 0 );});
		hd.blocks[hd.blocks[index].file.inode].inode.readcount++;
	}
	printk(DEBUG "In READ after wait\n");
	hd.blocks[hd.blocks[index].get<file_t>().inode].get<inode_t>().atime=aos::now();
	printk(DEBUG "In READ after wait in copy fun\n");



	if (offset > data.size())
		size = 0;
	else if(offset+size > data.size())
		size=data.size()-offset;

	//aos::strncpy(buf,(data),size);
	if(size>0)
		aos::copy(data.begin()+offset,data.end(),buf);

	printk(DEBUG "In READ after wait in copy fun\n");
	printk(DEBUG "In READ the content of the buf*:%s\n",buf);

	//aos::lock_guard<aos::mutex> guard{hd.blocks[index].file.mutex};
	hd.blocks[hd.blocks[index].get<file_t>().inode].get<inode_t>().readcount--;
	printk(DEBUG "In READ end:%d\n",size);
	printk(DEBUG "In READ end\n");

	return size;
}
