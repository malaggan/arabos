#include <file_t.h>
#include <string.h>

int sfs_link (const char *oldpath, const char *newpath)
{
	printk (DEBUG  "LINK        ");
	int file=ROOT.file.find(oldpath);
	printk (DEBUG  "LINK file index :%d\n",file);
	if(file==-1)
		return -ENOENT;
	auto free_index=hd.search(sizeof(block_t));
	printk (DEBUG  "LINK free_index size :%d\n",free_index.size());
	if(free_index.size() !=1)
		return -EEXIST;
	if(ROOT.file.find(newpath)>=0)
		return EEXIST;
	char *newname=basename(strdup(newpath));
	auto ft = read_block(file);
	block_t ftlink;
	ftlink.change_type<file_t>(newname,ft.file.mode,file_type::F,ft.file.inode,false);

	printk (DEBUG  "LINK ftlink newname :%s\n",newname);
	printk (DEBUG  "LINK ftlink name :%s\n",ftlink.file.name.c_str());

	ftlink.write(free_index.front());

	auto it = read_block(ft.file.inode);
	it.inode.link++;
	it.write(ft.file.inode);
	int parent = ROOT.file.find(dirname(strdup(newpath)));
	ft.read(parent);
	ft.file.add(free_index.front());
	ft.write(parent);

	return 0;
}
