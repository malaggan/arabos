#include <file_t.h>
#include <string.h>

int sfs_symlink(char const *target,char const *linkpath)
{
	printk(DEBUG "%s:%d target = %s, linkpath = %s\n",__FUNCTION__,__LINE__,target,linkpath);

	if(ROOT.file.find(linkpath)>0)
		return EEXIST;

	auto symlink_index = hd.search(sizeof(block_t)*3);
	if(symlink_index.size()!=3)
		printf(DEBUG"symlink Can't find 3 free blocks\n" );
	block_t ft;
	ft.change_type<file_t>(basename(strdup(linkpath)), 0777,file_type::F,
	                       symlink_index[1]
	                       , true);

	ft.write(symlink_index.front());
	block_t it;
	it.change_type<inode_t>();
	it.write(symlink_index[1]);

	block_t dt;
	dt.change_type<data_t>();

	ft.file.add(symlink_index.back());
	aos::copy(target, target+strnlen(target,10000), aos::back_inserter(dt.data.data));
	dt.write(symlink_index.back());
	int parent = ROOT.file.find(dirname(strdup(linkpath)));
	auto ftparent = read_block(parent);
	if(parent==-1)
		return -1;

	ftparent.file.add(symlink_index.front());

	return 0;
}
