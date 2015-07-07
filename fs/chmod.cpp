#include <file_t.h>

int sfs_chmod (const char *path, mode_t mode) {
	int index_file = ROOT.file.find(path);

	if(index_file == -1)
		return -EEXIST;

	auto ft = read_block(index_file);
	auto it = read_block(ft.file.inode);

	if(!it.inode.check_permission(0/*uid*/, 0/*gid*/))
		return -EACCES;

	ft.file.mode = mode;
	ft.write(index_file);

	return 0;
}
