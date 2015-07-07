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
	                       //*(++symlink_index.begin())
	                       symlink_index[1] // WARN: additio does not make sense here
	                       , true);
	/*hd.blocks[*(symlink_index.begin())].change_type<file_t>(basename(strdup(linkpath)), 0777,file_type::F,
	//*(++symlink_index.begin())
	*(symlink_index.begin()) // WARN: additio does not make sense here
	, true);*/
	convert_to_write(ft,symlink_index.front());
	block_t it;
	it.change_type<inode_t>();
	convert_to_write(it, symlink_index[1]);

	/* hd.blocks[
	// *(++symlink_index.begin())
	*(symlink_index.begin()) // WARN: addition does not make sense here
	].change_type<inode_t>();*/
	block_t dt;
	dt.change_type<data_t>();

	//hd.blocks[symlink_index.back()].change_type<data_t>();
	//hd.blocks[symlink_index.front()].get<file_t>().add(symlink_index.back());
	ft.file.add(symlink_index.back());
	aos::copy(target, target+strnlen(target,10000), aos::back_inserter(/*hd.blocks[symlink_index.back()].get<data_t>()*/dt.data.data));
	convert_to_write(dt,symlink_index.back());
	int parent = ROOT.file.find(dirname(strdup(linkpath)));
	block_t ftparent;
	convert(ftparent,parent);
	if(parent==-1)
		return -1;

	/*hd.blocks[parent].get<file_t>()*/ftparent.file.add(symlink_index.front());

	return 0;
}
