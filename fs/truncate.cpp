#include <file_t.h>

int sfs_truncate (const char *path, off_t size)
{
	//aos::lock_guard<aos::mutex> guard{ROOT.mutex};
	int file_index=ROOT.file.find(path);


	if(file_index==-1) {
		printk (DEBUG  "truncate -ENOENT: \"!file\"\n");
		return -ENOENT;
	}
	block_t fileobj;
	convert(fileobj,file_index);
	assert(!fileobj.file.isDirectory());

	if(fileobj.file.type==file_type::D) {
		printk (DEBUG  "truncate -ENOENT: \"!file->contents\"\n");
		return -ENOENT;
	}
	block_t inodeobj;
	convert(inodeobj,fileobj.file.inode);

	{
		//aos::unique_lock<aos::mutex> lk(hd.blocks[file_index].file.mutex);
		bool condition=(inodeobj.inode.readcount > 0 && inodeobj.inode.writecount == 0);
		//ROOT.cond.wait(lk,[&]{return !condition;});
		inodeobj.inode.writecount++;
		convert_to_write(inodeobj,fileobj.file.inode);
	}

	convert(inodeobj,fileobj.file.inode);
	if(static_cast<unsigned int>(size) != inodeobj.inode.size) {
		inodeobj.inode.size=size; /// wallah a3lam :D
	}


	inodeobj.inode.mtime=aos::now();
	{
		//aos::lock_guard<aos::mutex> guard{hd.blocks[file_index].file.mutex};
		inodeobj.inode.writecount--;
	}
	convert_to_write(inodeobj,fileobj.file.inode);
	return 0;
}
