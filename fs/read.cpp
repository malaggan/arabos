#include <file_t.h>

int sfs_read (const char UNUSED *path, char *buf, size_t size, off_t offset, int file_handle)//struct fuse_file_info *f)
{
	int index=file_handle; //f->fh;
	auto fileobj = read_block(index);
	auto inode = read_block(fileobj.file.inode);
	printk(DEBUG "In READ the index file number1:%d \n",index);
	printk(DEBUG "In read the size of index_files: %d\n",inode.inode.index_file.size());


	printk(DEBUG "In READ number of blocks are %d\n",inode.inode.index_file.size());
	aos::vector<char>  data;
	for(unsigned int i =0;
	    i < inode.inode.index_file.size();
	    i++)
	{
		printf(DEBUG "HERE index:%d\n" ,inode.inode.index_file[i]);
		auto dataobj = read_block(inode.inode.index_file[i]);
		printf(DEBUG "HERE data size:%d\n" ,dataobj.data.data.size());

		for(unsigned int j = 0;
		    j<dataobj.data.data.size();
		    j++){

			printf(DEBUG "content :%c\n",dataobj.data.data[j] );
			data.push_back( dataobj.data.data[j]);
		}
	}
	//printk(DEBUG "In read fh is %p\n",reinterpret_cast<void*>(index));

	if(index==-1)
		return -ENOENT;


	printk(DEBUG "In READ 1 \n");


	{
		// aos::unique_lock<aos::mutex> lk(hd.blocks[index].get<file_t>().mutex);

		printk(DEBUG "In READ  before wait\n");
		inode.inode.readcount++;
		inode.write(fileobj.file.inode);
	}
	printk(DEBUG "In READ after wait\n");
	inode.inode.atime=aos::now();
	printk(DEBUG "In READ after wait in copy fun\n");



	if (offset > data.size())
		size = 0;
	else if(offset+size > data.size())
		size=data.size()-offset;
	printk(DEBUG "In READ 3\n");
	printk(DEBUG "In READ before the content of the buf*:%s\n",buf);
	if(size>0)
		aos::copy_n(data.begin()+offset,data.size(),buf);

	printk(DEBUG "In READ after wait in copy fun\n");
	printk(DEBUG "In READ the content of the buf*:%s\n",buf);

	//aos::lock_guard<aos::mutex> guard{hd.blocks[index].file.mutex};
	inode.inode.readcount--;
	inode.write(fileobj.file.inode);
	printk(DEBUG "In READ end:%d\n",size);
	printk(DEBUG "In READ end\n");

	return size;
}
