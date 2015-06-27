#include <file_t.h>

int sfs_chown (const char *path, uid_t uid, gid_t gid)
{
	printk(DEBUG "chown");
	int index_file = ROOT.find(path);
	if(index_file==-1)
		return -ENOENT;

	uid_t current_uid = 0;

	if(current_uid != hd.blocks[hd.blocks[index_file].get<file_t>().inode].get<inode_t>().uid)
		if(current_uid)
			return -EACCES;

	hd.blocks[hd.blocks[index_file].get<file_t>().inode].get<inode_t>().uid=uid;
	hd.blocks[hd.blocks[index_file].get<file_t>().inode].get<inode_t>().gid=gid;

	return 0;
}
