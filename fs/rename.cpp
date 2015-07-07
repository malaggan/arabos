#include <file_t.h>
#include <string.h>

int sfs_rename (const char *path, const char *newpath)
{
	printk (DEBUG  "rename ");
	int i_file=ROOT.file.find(path);
	printk (DEBUG  "rename file index :%d\n",i_file);
	if(i_file==-1)
		return -ENOENT;

	int parent=ROOT.file.find(dirname(strdup(path)));
	printk (DEBUG  "rename parent index:%d\n",parent);
	int newfile=ROOT.file.find(newpath);
	if(newfile>=0)
		return EEXIST;
	printk (DEBUG  "rename new file index:%d\n",newfile);
	int newparent=ROOT.file.find(dirname(strdup(newpath)));
	printk (DEBUG  "rename new parent index:%d\n",newparent);
	block_t ft;//the old file
	convert(ft,i_file);
	block_t ftparent;//the old parent
	convert(ftparent,parent);
	block_t ftnewparent;//the new parent
	convert(ftnewparent,newparent);
	block_t itparent;//inode of old parent
	convert(itparent,ftparent.file.inode);
	//block_t itnewparent;//inode of new parent

	//aos::static_vector<uint32_t,1> temp;
	auto tmp=aos::find(itparent.inode.index_file.begin(),
	                   itparent.inode.index_file.end(),i_file);
	itparent.inode.index_file.erase(tmp,tmp+1);
	convert_to_write(itparent,ftparent.file.inode);
	ft.file.name=basename(strdup(newpath));
	printk (DEBUG  "rename new file name:%s\n",ft.file.name.c_str());
	convert_to_write(ft,i_file);
	ftnewparent.file.add(i_file);


	//itparent.change_type<inode_t>();
	//convert(itparent,ftnewparent.file.inode);//new  parent inode
	//convert_to_write(itparent,ftnewparent.file.inode);


	return 0;

}
