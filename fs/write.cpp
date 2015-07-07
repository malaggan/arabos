#include <file_t.h>

int sfs_write (const char UNUSED *path, const char  *buf, size_t  size, off_t  offset,
               int file_handle) // NOTE file_handle was f->fh , and f was fuse_file_info
{
	printf(DEBUG"write buf %s\n",buf );
	aos::string<128> temp;

	int file= file_handle;
	block_t fileobj;
	convert(fileobj,file);
	block_t inode;
	convert(inode,fileobj.file.inode);
	if(file==-1)
		return -ENOENT;

	{
		//aos::unique_lock<aos::mutex> lk(hd.blocks[file].get<file_t>().mutex);
		// hd.blocks[file].get<file_t>().cond.wait(lk,[&]{return ! (hd.blocks[hd.blocks[file].get<file_t>().inode].get<inode_t>().readcount > 0 &&
		//							 hd.blocks[hd.blocks[file].get<file_t>().inode].get<inode_t>().writecount > 0);});
	}
	inode.inode.writecount++;
	inode.inode.mtime = aos::now();

	aos::static_vector<uint32_t,8> free_index=hd.search(size);
	//the file is empty
	printf(DEBUG"write search result:%d\n",free_index.size() );
	if (inode.inode.index_file.empty())
	{
		temp=buf;
		printf(DEBUG"write empty:%d\n",free_index.size() );

		for(unsigned int i=0;i<free_index.size();i++)
		{
			printf(DEBUG"write empty loop:%d\n",free_index[i]);

			block_t dataobj;
			dataobj.change_type<data_t>(); // WARN: was "free_index.begin()++", but that doesn't make sense
			//auto index = *(free_index.begin()); // WARN: was "*(free_index.begin()++)", but that doesn't make sense
			auto target = dataobj.data.data;
			aos::copy_n(temp.begin()+i*490,size,aos::back_inserter(dataobj.data.data));
			printf(DEBUG "data size:%d\n", dataobj.data.data.size());
			convert_to_write(dataobj,free_index[i]);
		}

		fileobj.file.add(free_index);
		convert(inode,fileobj.file.inode);
		inode.inode.size=size;
	}

	else
	{
		aos::vector<char>  data;
		block_t freeobj;
		freeobj.tag=block_type::free;
		for(int i=0;i<inode.inode.index_file.size();i++)
			convert_to_write(freeobj,inode.inode.index_file[i]);

		inode.inode.index_file.clear();
		convert_to_write(inode,fileobj.file.inode);
		data.insert(data.end(), buf, buf+size);
		printk(DEBUG "data in write:%d\n",data.size() );
		aos::static_vector<uint32_t,8> free_index_2=hd.search(data.size());
		auto free_ind=free_index_2.begin();
		unsigned int free_index_size=free_index_2.size();

		for(unsigned int i=0;i<free_index_size;i++)
		{
			//hd.blocks[*free_ind].change_type<data_t>();
			block_t dataobj;
			dataobj.change_type<data_t>();
			aos::copy_n(data.begin()+(i*490),size,aos::back_inserter(dataobj.data.data));
			//aos::copy(data.begin()+(i*496),data.begin()+((i+1)*496),aos::back_inserter(dataobj.data.data));
			convert_to_write(dataobj,free_index_2[i]);
		}

		fileobj.file.add(free_index_2);
		convert(inode,fileobj.file.inode);
		inode.inode.size=size;
	}
	{
		//aos::lock_guard<aos::mutex> guard{hd.blocks[file].get<file_t>().mutex};
		inode.inode.writecount--;
	}
	convert_to_write(inode,fileobj.file.inode);
	convert_to_write(fileobj,file);
	return size;
}
