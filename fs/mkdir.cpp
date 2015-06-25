#include <file_t.h>

// #include <libgen.h>
// #include <boost/algorithm/string.hpp>

int  sfs_mkdir(const char *path , mode_t mode)
{
    printk(DEBUG  "In Mkdir\n");
    char *pathd = strdup (path);
    char *dir = dirname (pathd);
    printk(DEBUG  "In Mkdir0\n");
    if(ROOT.find(path)>0)
    	return EEXIST;
    printk(DEBUG  "In Mkdir1\n");	
    //aos::list<aos::string> strs;
    //boost::split(strs, path, boost::is_any_of("/")); 
    printk(DEBUG  "In Mkdir2\n");
    printk(DEBUG  ">>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>In parent:%s:2\n",dir);
    int parent = ROOT.find(dir);
    printk(DEBUG  ">>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>In parent:%d:2\n",parent);
    if(parent==-1)
   	return -ENOENT;

    auto free_index=hd.search(2*sizeof(block_t));
    if(free_index.size() !=2)
	return -EEXIST;
    printk(DEBUG  "In Mkdir4  free_index:%d\n",free_index.front());
    printk(DEBUG  "In Mkdir4  free_index back:%d\n",free_index.back());
    
    //printk(DEBUG  ">>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>in mkdir *strs.rbegin():%s:2\n",basename(strdup(path)));
    hd.blocks[free_index.front()].change_type<file_t>(basename(strdup(path)),mode,file_type::D,free_index.back(),false);
    printk(DEBUG  "In Mkdir3 File Name int the block is %s\n",hd.blocks[free_index.front()].get<file_t>().name.c_str());
    //printk(DEBUG  "In Mkdir3 File Name is %s\n",(*strs.rbegin()).c_str());
    hd.blocks[free_index.front()].get<file_t>().inode=free_index.back();
    hd.blocks[free_index.back()].change_type<inode_t>();
    printk(DEBUG  "In Mkdir4\n");
    hd.blocks[parent].get<file_t>().add(free_index.front());
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
