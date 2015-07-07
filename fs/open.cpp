#include <file_t.h>

int sfs_open (const char *path, int /*out*/ *file_handle)//struct fuse_file_info *f)
{
	printk(DEBUG "In OPEN \n");
	int file= ROOT.file.find(path);
	if(file==-1 )
		return -ENOENT;
	block_t fileobj;
	convert(fileobj,file);
	block_t inode;
	convert(inode,fileobj.file.inode);
	if(!(inode.inode.check_permission(0/*uid*/, 0/*gid*/)) && fileobj.file.type==file_type::D ) {
		return -EACCES;
	}

	if(! ( ( fileobj.file.mode & S_IRUSR) || ( fileobj.file.mode && S_IRGRP ) || ( fileobj.file.mode & S_IROTH) ) ) {
		return -EACCES;
	}
	*file_handle =file;
	inode.inode.usecount++;
	convert_to_write(inode,fileobj.file.inode);
	printk(DEBUG "In OPEN END\n");
	return 0;
}
