#include <file_t.h>

int sfs_chown (const char *path, uid_t uid, gid_t gid)
{
	printk(DEBUG "chown");
	int index_file = ROOT.file.find(path);
	if(index_file==-1)
		return -ENOENT;

	uid_t current_uid = 0;
	block_t ft;
	convert(ft,index_file);
	block_t it;
	convert(it,ft.file.inode);
	if(current_uid != /*hd.blocks[hd.blocks[index_file].get<file_t>().inode].get<inode_t>()*/it.inode.uid)
		if(current_uid)
			return -EACCES;

	it.inode.uid=uid;
	it.inode.gid=gid;
	convert_to_write(it,index_file);
	return 0;
}
