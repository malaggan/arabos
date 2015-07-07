#include <file_t.h>
#include <memops.h>

int sfs_getattr (const char *path, struct stat *stbuf)
{
	printk(DEBUG "getattr path=%s\n",path);
	int file_inode= ROOT.file.find(path);
	printk(DEBUG ">>>>>>>>>>getattr file_inode=%d\n",file_inode);
	printk(DEBUG "In getattr In IF1\n");
	if(file_inode==-1)
		return -ENOENT;
	printk(DEBUG "In getattr In IF2\n");


	//printk(DEBUG "getattr the name of the file is :%s\n",hd.blocks[file_inode].get<file_t>().name);
	memset(reinterpret_cast<unsigned char*>(stbuf),0,sizeof(*stbuf));
	auto ft = read_block(file_inode);
	auto it = read_block(ft.file.inode);
	stbuf->st_mode = (!(ft.file.type==file_type::D)?S_IFREG:S_IFDIR) | ft.file.mode;

	if(ft.file.isSymlink())
		stbuf->st_mode |= S_IFLNK;
	stbuf->st_nlink = 1;
	stbuf->st_size = it.inode.size;
	stbuf->st_ctime = aos::to_time_t(it.inode.ctime);
	stbuf->st_atime = aos::to_time_t(it.inode.atime);
	stbuf->st_mtime = aos::to_time_t(it.inode.mtime);
	stbuf->st_uid =  it.inode.uid;
	stbuf->st_gid =  it.inode.gid;
	printk(DEBUG "In getattr In IF\n");
	return 0;
}
