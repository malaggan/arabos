#include <file_t.h>

// #include <stl/iterator>
// #include <cstring>
// #include <algorithm>

int sfs_write (const char UNUSED *path, const char  *buf, size_t  size, off_t  offset,
	       int file_handle) // NOTE file_handle was f->fh , and f was fuse_file_info
{
    aos::string<128> temp;

    int file= file_handle;
    
    if(file==-1)
	return -ENOENT;
 
    {
	//aos::unique_lock<aos::mutex> lk(hd.blocks[file].get<file_t>().mutex);
	// hd.blocks[file].get<file_t>().cond.wait(lk,[&]{return ! (hd.blocks[hd.blocks[file].get<file_t>().inode].get<inode_t>().readcount > 0 &&
	//							 hd.blocks[hd.blocks[file].get<file_t>().inode].get<inode_t>().writecount > 0);}); 
    }
    hd.blocks[hd.blocks[file].get<file_t>().inode].get<inode_t>().writecount++;
    hd.blocks[hd.blocks[file].get<file_t>().inode].get<inode_t>().mtime = aos::now();
      
    aos::static_vector<uint32_t,8> free_index=hd.search(size);
    //the file is empty
      
    if (hd.blocks[hd.blocks[file].get<file_t>().inode].get<inode_t>().index_file.empty())
    {
	temp=buf;
	for(unsigned int i=0;i<free_index.size();i++)
	{
	    hd.blocks[*free_index.begin()].change_type<data_t>(); // WARN: was "free_index.begin()++", but that doesn't make sense
	    auto index = *(free_index.begin()); // WARN: was "*(free_index.begin()++)", but that doesn't make sense
	    auto target = hd.blocks[index].get<data_t>().data;
	    aos::copy_n(temp.begin()+i*490,size,aos::back_inserter(target));
	};
	hd.blocks[file].get<file_t>().add(free_index);
        hd.blocks[hd.blocks[file].get<file_t>().inode].get<inode_t>().size=size;
    }
      
    else
    {
	aos::vector<char>  data;
				 
	hd.blocks[hd.blocks[file].get<file_t>().inode].get<inode_t>().index_file.clear();
	data.insert(data.end(), buf, buf+size);
	aos::static_vector<uint32_t,8> free_index_2=hd.search(data.size());
	auto free_ind=free_index_2.begin();
	unsigned int free_index_size=free_index_2.size();
	for(unsigned int i=0;i<free_index_size;i++)
	{
	    hd.blocks[*free_ind].change_type<data_t>();
	    aos::copy(data.begin()+(i*496),data.begin()+((i+1)*496),aos::back_inserter(hd.blocks[*(free_ind++)].get<data_t>().data)); 
	}
	hd.blocks[file].get<file_t>().add(free_index_2);
	hd.blocks[hd.blocks[file].get<file_t>().inode].get<inode_t>().size=size;
    }
    {	
	//aos::lock_guard<aos::mutex> guard{hd.blocks[file].get<file_t>().mutex};
	hd.blocks[hd.blocks[file].get<file_t>().inode].get<inode_t>().writecount--;
    }

    return size;
}
