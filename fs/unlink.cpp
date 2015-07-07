#include <file_t.h>
#include <string.h>

int sfs_unlink (const char *path)
{
	printk(DEBUG  "In Unlink\n");
	char *pathd = strdup (path);
	char *dir = dirname (pathd);

	int parent= ROOT.file.find(dir);
	auto ftparent = read_block(parent);
	auto itparent = read_block(ftparent.file.inode);
	int current = ROOT.file.find(path);
	auto ft = read_block(current);
	auto it = read_block(ft.file.inode);

	if(current == -1)
		return -ENOENT;

	assert(ft.file.type == file_type::F);

	for(unsigned int i = 0; i < it.inode.index_file.size() ; i++ )
	{
		if(it.inode.link == 1 ){
			auto btmp = read_block(it.inode.index_file[i]);
			btmp.tag=block_type::free;
			btmp.data.data.clear();
			btmp.write(it.inode.index_file[i]);
			it.inode.index_file.erase(it.inode.index_file.begin()+i);

		}
	}
	it.write(ft.file.inode);
	ft.tag = block_type::free;
	for(unsigned int i = 0 ; i < itparent.inode.index_file.size() ; i++)
	{
		if(itparent.inode.index_file[i] == current){
			itparent.inode.index_file.erase(itparent.inode.index_file.begin() + i);
			ft.tag=block_type::free;
			it.tag=block_type::free;
			it.write(ft.file.inode);
			ft.write(current);
		}
	}
	itparent.write(ftparent.file.inode);

	printk(DEBUG "END OF UNLINK\n");
	return 0;

}
