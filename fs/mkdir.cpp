#include <file_t.h>
#include <string.h>

int  sfs_mkdir(const char *path , mode_t mode)
{
	printk(DEBUG  "In Mkdir\n");
	printk(DEBUG  "In Mkdir the path is path:%s\n",path);
	auto strs = ROOT.file.split(aos::string<128>{path},aos::string<2>{"/"});
	char *pathd = strdup (path);
	char *dir = dirname (pathd);
	printk(DEBUG  "In Mkdir0\n");
	if(ROOT.file.find(path)>0)
		return EEXIST;
	printk(DEBUG  "In Mkdir1\n");
	printk(DEBUG  "In Mkdir2\n");
	printk(DEBUG  ">>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>the dirname in mkdir:%s:2\n",dir);
	int parent = ROOT.file.find(dir);
	printk(DEBUG  ">>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>In parent:%d:2\n",parent);
	if(parent==-1)
		return -ENOENT;

	auto free_index=hd.search(2*sizeof(block_t));
	if(free_index.size() !=2)
		return -EEXIST;
	// printk(DEBUG  "In Mkdir4  free_index:%d\n",free_index.front());
	// printk(DEBUG  "In Mkdir4  free_index back:%d\n",free_index.back());

	//printk(DEBUG  ">>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>in mkdir *strs.rbegin():%s:2\n",basename(strdup(path)));
	// hd.blocks[free_index.front()].Change_type<file_t>(basename(strdup(path)),mode,file_type::D,free_index.back(),false);
	block_t ft;
	ft.change_type<file_t>(*strs.rbegin(),mode,file_type::D,free_index.back(),false);
	convert_to_write(ft,free_index.front());
	printk(DEBUG  "In Mkdir3 File the index of file: %d\n",ft.file.inode);
	printk(DEBUG  "In Mkdir3 File the free_index.front(): %d\n",free_index.front());
	//printk(DEBUG  "In Mkdir3 File Name is %s\n",(*strs.rbegin()).c_str());
	// ft.file.inode=free_index.back();
	// hd.blocks[free_index.back()].change_type<inode_t>();
	block_t it;
	it.change_type<inode_t>();
	convert_to_write(it,free_index.back());
	printk(DEBUG  "In Mkdir4\n");
	// hd.blocks[parent].get<file_t>().add(free_index.front());
	block_t re;
	convert(re,parent);
	re.file.add(free_index.front());

	block_t pi;
	convert(pi,re.file.inode);
	printk(DEBUG  "In Mkdir5 pi.indexfile:%d \n",pi.inode.index_file.size());

	printk(DEBUG  "In Mkdir5\n");

	return 0;
}






/*int  sfs_mkdir(const char *path , mode_t mode)
  {
  printk(DEBUG  "In Mkdir\n");

  if(ROOT.find(path))
  return -EEXIST;

  char *pathd = strdup (path);
  char *pathb = strdup (path);
  char *dir = dirname (pathd);
  char *base = basename (pathb);

  int parent = ROOT.find(dir);

  if(!parent)
  return -ENOENT;

  // note to myself: use emplace_back with perfect forwarding here
  parent.add(aos::make_shared<file_t>(boost::none, base, mode));

  // TODO: free pathd, pathb, dir, and base here.

  return 0;
  }*/
