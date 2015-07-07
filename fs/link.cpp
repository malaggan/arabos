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
	block_t ft;
	convert(ft,file);
	block_t ftlink;
	ftlink.change_type<file_t>(newname,ft.file.mode,file_type::F,ft.file.inode,false);
	//hd.blocks[free_index.front()].change_type<file_t>(newname,hd.blocks[file].file.mode,file_type::F,hd.blocks[file].file.inode,false);
	printk (DEBUG  "LINK ftlink newname :%s\n",newname);
	printk (DEBUG  "LINK ftlink name :%s\n",ftlink.file.name.c_str());

	convert_to_write(ftlink,free_index.front());

	block_t it;
	convert(it,ft.file.inode);
	//hd.blocks[hd.blocks[file].get<file_t>().inode].get<inode_t>().link++;
	it.inode.link++;
	convert_to_write(it,ft.file.inode);
	int parent =ROOT.file.find(dirname(strdup(newpath)));
	convert(ft,parent);
	ft.file.add(free_index.front());
	convert_to_write(ft,parent);
	//hd.blocks[parent].get<file_t>().add(free_index.front());*/
	return 0;

}
