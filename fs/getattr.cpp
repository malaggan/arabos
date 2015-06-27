#include <file_t.h>

// #include <boost/algorithm/string.hpp>

int sfs_getattr (const char *path, struct stat *stbuf)
{
	printk(DEBUG "getattr path=%s\n",path);
	int file_inode= hd.blocks[0].get<file_t>().find(path);
	printk(DEBUG ">>>>>>>>>>>>>>>>>>>>>>>>>getattr file_inode=%d\n",file_inode);
	printk(DEBUG "In getattr In IF1\n");
	if(file_inode==-1)
		return -ENOENT;
	printk(DEBUG "In getattr In IF2\n");


	//printk(DEBUG "getattr the name of the file is :%s\n",hd.blocks[file_inode].get<file_t>().name);
	memset(reinterpret_cast<unsigned char*>(stbuf),0,sizeof(*stbuf));
	stbuf->st_mode = (!(hd.blocks[file_inode].get<file_t>().type==file_type::D)?S_IFREG:S_IFDIR) | hd.blocks[file_inode].get<file_t>().mode;

	if(hd.blocks[file_inode].get<file_t>().isSymlink())
		stbuf->st_mode |= S_IFLNK;
	stbuf->st_nlink = 1;
	stbuf->st_size = hd.blocks[hd.blocks[file_inode].get<file_t>().inode].get<inode_t>().size;
	stbuf->st_ctime = aos::to_time_t(hd.blocks[hd.blocks[file_inode].get<file_t>().inode].get<inode_t>().ctime);
	stbuf->st_atime = aos::to_time_t(hd.blocks[hd.blocks[file_inode].get<file_t>().inode].get<inode_t>().atime);
	stbuf->st_mtime = aos::to_time_t(hd.blocks[hd.blocks[file_inode].get<file_t>().inode].get<inode_t>().mtime);
	stbuf->st_uid =  hd.blocks[hd.blocks[file_inode].get<file_t>().inode].get<inode_t>().uid;
	stbuf->st_gid =  hd.blocks[hd.blocks[file_inode].get<file_t>().inode].get<inode_t>().gid;
	printk(DEBUG "In getattr In IF\n");
	return 0;
}
