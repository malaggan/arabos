#include <file_t.h>

int sfs_chown (const char *path, uid_t uid, gid_t gid)
{
	int index_file = ROOT.file.find(path);

	if(index_file == -1)
		return -ENOENT;

	uid_t current_uid = 0;

	auto ft = read_block(index_file);
	auto it = read_block(ft.file.inode);

	if(current_uid != it.inode.uid && current_uid)
			return -EACCES;

	it.inode.uid = uid;
	it.inode.gid = gid;
	it.write(index_file);
	return 0;
}
