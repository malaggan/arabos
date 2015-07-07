#include <file_t.h>
#include <string.h>

int sfs_unlink (const char *path)
{
	printk(DEBUG  "In Unlink\n");
	char *pathd = strdup (path);
	char *dir = dirname (pathd);

	int parent= ROOT.file.find(dir);
	//auto& parent = hd.blocks[parent_node_number].file;
	//auto& parent_inode = hd.blocks[parent.inode].inode;
	block_t ftparent;
	convert(ftparent,parent);
	block_t itparent;
	convert(itparent,ftparent.file.inode);
	int current = ROOT.file.find(path);
	//auto& current = hd.blocks[current_node_number].file;
	//auto& current_inode = hd.blocks[current.inode].inode;
	block_t ft;
	convert(ft,current);
	block_t it;
	convert(it,ft.file.inode);
	//int current = ROOT.file.find(path);


	if(current == -1)
		return -ENOENT;

	assert(ft.file.type == file_type::F);

	for(unsigned int i = 0; i < it.inode.index_file.size() ; i++ )
	{
		if(it.inode.link == 1 ){
			block_t btmp;
			convert(btmp,it.inode.index_file[i]);
			//hd.blocks[current_inode.index_file[i]].tag = block_type::free;
			btmp.tag=block_type::free;
			//hd.blocks[current_inode.index_file[i]].data.data.clear();
			btmp.data.data.clear();
			convert_to_write(btmp,it.inode.index_file[i]);
			//(hd.blocks[current.inode].inode).index_file.erase((hd.blocks[current.inode].inode).index_file.begin() + i);
			it.inode.index_file.erase(it.inode.index_file.begin()+i);

		}
	}
	convert_to_write(it,ft.file.inode);
	//hd.blocks[current_node_number].tag = block_type::free;
	ft.tag = block_type::free;
	for(unsigned int i = 0 ; i < /*hd.blocks[parent.inode]*/itparent.inode.index_file.size() ; i++)
	{
		if(/*hd.blocks[parent.inode]*/itparent.inode.index_file[i] == current/*static_cast<uint32_t>(current_node_number)*/){
			/*hd.blocks[parent.inode]*/itparent.inode.index_file.erase(/*hd.blocks[parent.inode]*/itparent.inode.index_file.begin() + i);
			ft.tag=block_type::free;
			it.tag=block_type::free;
			convert_to_write(it,ft.file.inode);
			convert_to_write(ft,current);
		}
	}
	convert_to_write(itparent,ftparent.file.inode);

//parent->children->erase(current->name);
	//current.reset();
	printk(DEBUG "END OF UNLINK\n");
	return 0;

}
