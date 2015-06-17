#include <file_t.h>

int sfs_open (const char *path, int /*out*/ *file_handle)//struct fuse_file_info *f)
{
    printk(DEBUG "In OPEN \n");
    int file= ROOT.find(path);
    if(file==-1 )
	return -ENOENT;
    
    if(!(hd.blocks[hd.blocks[file].file.inode].inode.check_permission(fuse_get_context()->uid,fuse_get_context()->gid))
       && hd.blocks[file].file.type==file_type::D ) {
	return -EACCES;
    }
  
    if(! ( ( hd.blocks[file].file.mode & S_IRUSR) || ( hd.blocks[file].file.mode && S_IRGRP ) || ( hd.blocks[file].file.mode & S_IROTH) ) ) {
	return -EACCES;
    }
    /*f->fh*/ *file_handle =file;
    hd.blocks[hd.blocks[file].file.inode].inode.usecount++;
    printk(DEBUG "In OPEN END\n");
    return 0;
}
