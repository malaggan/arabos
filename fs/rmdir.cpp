#include <file_t.h>
#include <string.h>

int remove_data(aos::static_vector<uint32_t,94> index_file){
	block_t block;
	block.tag=block_type::free;
	for(int i=0;i<index_file.size();i++)
		convert_to_write(block,index_file[i]);
	return 0;
}
int rmdir_rec(aos::static_vector<uint32_t,94> index_file){
	block_t file;
	block_t inode;
	block_t dataobj;
	block_t block;
	block.tag=block_type::free;

	for(int i=0;i<index_file.size();i++){
		convert(file,index_file[i]);
		convert(inode,file.file.inode);
		if(file.file.type==file_type::F)
		{
			convert_to_write(block,index_file[i]);
			convert_to_write(block,file.file.inode);
			remove_data(inode.inode.index_file);
		}
		else if(file.file.type==file_type::D){
			if(inode.inode.index_file.size()>0)
				rmdir_rec(inode.inode.index_file);

			convert_to_write(block,index_file[i]);
			convert_to_write(block,file.file.inode);
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
	block_t ft;
	convert(ft,dir);
	block_t it;
	convert(it,ft.file.inode);
	block_t ftparent;
	convert(ftparent,parent);
	block_t itparent;
	convert(itparent,ftparent.file.inode);
	assert(ft.file.type == file_type::D);

	if(dir== -1)
		return -ENOENT;
	else
	{
		//it.inode.index_file.erase(it.inode.index_file.begin(),it.inode.index_file.end());
		rmdir_rec(it.inode.index_file);
		itparent.inode.index_file.erase(aos::find( itparent.inode.index_file.begin(), itparent.inode.index_file.end(),dir));
		it.tag=block_type::free;
		convert_to_write(it,ft.file.inode);
		ft.tag=block_type::free;
		convert_to_write(ft,dir);
		convert_to_write(itparent,ftparent.file.inode);
		printk(DEBUG "END OF UNLINK\n");
	}

	return 0;

}
