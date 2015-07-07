#include <file_t.h>
#include <string.h>

int remove_data(aos::static_vector<uint32_t,94> index_file){
	block_t block;
	block.tag=block_type::free;
	for(int i=0;i<index_file.size();i++)
		block.write(index_file[i]);
	return 0;
}
int rmdir_rec(aos::static_vector<uint32_t,94> index_file){
	block_t file;
	block_t inode;
	block_t dataobj;
	block_t block;
	block.tag=block_type::free;

	for(int i=0;i<index_file.size();i++){
		file.read(index_file[i]);
		inode.read(file.file.inode);
		if(file.file.type==file_type::F)
		{
			block.write(index_file[i]);
			block.write(file.file.inode);
			remove_data(inode.inode.index_file);
		}
		else if(file.file.type==file_type::D){
			if(inode.inode.index_file.size()>0)
				rmdir_rec(inode.inode.index_file);

			block.write(index_file[i]);
			block.write(file.file.inode);
		}
	}
	return 0;
}




int sfs_rmdir(const char *path)
{
	printk(DEBUG  "In RMDIR\n");
	char *pathd = strdup (path);
	char *dir_path = dirname (pathd);

	int dir= ROOT.file.find(path);
	int parent=ROOT.file.find(dir_path);
	auto ft = read_block(dir);
	auto it = read_block(ft.file.inode);
	auto ftparent = read_block(parent);
	auto itparent = read_block(ftparent.file.inode);
	assert(ft.file.type == file_type::D);

	if(dir== -1)
		return -ENOENT;
	else
	{
		rmdir_rec(it.inode.index_file);
		itparent.inode.index_file.erase(aos::find( itparent.inode.index_file.begin(), itparent.inode.index_file.end(),dir));
		it.tag=block_type::free;
		it.write(ft.file.inode);
		ft.tag=block_type::free;
		ft.write(dir);
		itparent.write(ftparent.file.inode);
		printk(DEBUG "END OF UNLINK\n");
	}

	return 0;

}
