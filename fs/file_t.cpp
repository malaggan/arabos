#include <file_t.h>

// #include <boost/algorithm/string.hpp>
//#include <string>
//#include <list>

// file_t::file_t(file_t const &f)
// {
//   name=f.name;
//   mode=f.mode;
//   type=f.type;
//   inode=f.inode;
//   is_symlink= f.is_symlink;
  
//   }

inode_t::inode_t(inode_t const &i)
    :size(i.size),
     usecount(i.usecount),
     ctime(i.ctime),
     atime(i.atime),
     mtime(i.mtime),
     gid(i.gid),
     readcount(i.readcount),
     writecount(i.writecount),link(i.link)
{}
inode_t::inode_t()
    : size(0),
      usecount(0),
      ctime(aos::now()), atime(ctime),mtime(ctime),
      uid(0), gid(0),
      readcount(0), writecount(0), /*cond(),*/link(1)
{
}

file_t::file_t(aos::string  name, mode_t mode,file_type type,int inode,bool is_symlink=false)
    :inode(inode),
     type(type),
     name(name),
     mode(mode), is_symlink(is_symlink)//,mutex()
{
    printk(DEBUG ">>>>>>>>>>>>>>>>>>>>>>>>>in CONSTRACTOR\n ");
    printk(DEBUG ">>>>>>>>>>>>>>>>>>>>>>>>>in find 2  before end %s:\n",name.c_str());
    // if(!isDirectory())
    //	contents = aos::make_shared<aos::vector<char>>();
}
void file_t::split(const aos::string& str, const aos::string& delim,aos::list<aos::string>& parts)
{
   size_t start, end = 0;
  while (end < str.size()) {
    start = end;
    while (start < str.size() && (delim.find(str[start]) !=aos::string::npos)) {
      start++;  // skip initial whitespace
    }
    end = start;
    while (end < str.size() && (delim.find(str[end]) ==aos::string::npos)) {
      end++; // skip to end of word
    }
    if (end-start != 0) {  // just ignore zero-length strings.
      parts.push_back(aos::string(str, start,end-start));
    }
  }
}

bool file_t::isSymlink() const {return is_symlink;}
/*bool file_t::isRoot() const { return !this->root; }
 */

// AAAAAAAAAAAAAAAAAAAAH return by value not reference !!!!!!!!!!!!!
int file_t::find(const aos::string path)	
    const
{
    printk(DEBUG "IN FIND path=%s\n",path.c_str());
    assert(path.at(0) == '/');
    assert(!path.empty());
    if(path.size() == 1)
    {
	printk(DEBUG ">>>>>>>>>>>>>>>>>>>>>>>>>in if loop of find 1 ");
	return 0;//ROOT
    }
    //  else if(path.at(1)=='.')
    //  return -1;
    printk(DEBUG ">>>>>>>>>>>>>>>>>>>>>>>>>in loop of find 1 ");
    printk(DEBUG ">>>>>>>>>>>>>>>>>>>>>>>>>in  find 1 the original path is:%s ",path.c_str());
    aos::list<aos::string> strs;
    ROOT.split(path,"/",strs); 
    strs.erase(strs.begin());
    printk(DEBUG ">>>>>>>>>>>>>>>>>>>>>>>>>in for loop find 1 file name strs.begine():%s ",(*strs.begin()).c_str());
    return hd.blocks[0].get<file_t>().find(aos::begin(strs), aos::end(strs));////ROOT.find(aos::begin(strs), aos::end(strs));
}

int  file_t::find(aos::list<aos::string>::const_iterator first, aos::list<aos::string>::const_iterator last)
    const
{
    assert(type==file_type::D);
    aos::static_vector<uint32_t,94> inode_files= hd.blocks[inode].get<inode_t>().index_file;
    printk(DEBUG ">>>>>>>>>>>>>>>>>>>>>>>>>in for loop find 2 file inode :%d ",inode);
    printk(DEBUG ">>>>>>>>>>>>>>>>>>>>>>>>>in for loop find 2 file name :%s ",name.c_str());
    printk(DEBUG ">>>>>>>>>>>>>>>>>>>>>>>>>in for loop find 2 file name *frist:%s ",(*first).c_str());
    printk(DEBUG ">>>>>>>>>>>>>>>>>>>>>>>>>in for THE SIZE OF INODE_FILES:%d",inode_files.size());

    if(inode_files.size()==0)
	return -1;
   
    unsigned int i;
    for(i=0;i<inode_files.size();i++){
	printk(DEBUG ">>>>>>>>>>>>>>>>>>>>>>>>>in for loop of find 2 ");
	if(hd.blocks[inode_files[i]].file.name==*first)
	{
	    printk(DEBUG ">>>>>>>>>>>>>>>>>>>>>>>>>in for loop of inode_file:%d ",inode_files[i]);
	    printk(DEBUG ">>>>>>>>>>>>>>>>>>>>>>>>>in for loop of find 2 ");
	    if(aos::distance(first,last)==1)
	    {
		printk(DEBUG ">>>>>>>>>>>>>>>>>>>>>>>>>in find 2  if");
		return inode_files.at(i);
		break;
	    }
	    return  hd.blocks[inode_files.at(i)].get<file_t>().find(++first, last);	
	}
	else if(i==inode_files.size()-1&&first==(--last))
	{
	    printk(DEBUG ">>>>>>>>>>>>>>>>>>>>>>>>>in find 2  else if");
	    return -1;
	}
    }
  
    printk(DEBUG ">>>>>>>>>>>>>>>>>>>>>>>>>in find 2  before end");
    printk(DEBUG ">>>>>>>>>>>>>>>>>>>>>>>>>in *inode_files.begin()++:%d:",*inode_files.begin());//++); // WARN: increment here does not make sense
}


void file_t::add(aos::list<uint32_t> index)
{
    for(unsigned int i=0;i<index.size();i++)
	hd.blocks[inode].get<inode_t>().index_file.push_back(*(index.begin()));//++); // WARN: increment here does not make sense
}

void file_t::add(int index)
{
    hd.blocks[inode].get<inode_t>().index_file.push_back(index);
    printk(DEBUG ">>>>>>>>>>>>>>>>>>>>>>>>>in ADD FILE :%d: ",*hd.blocks[inode].get<inode_t>().index_file.rbegin());
}

/*void file_t::add(aos::list<uint32_t> index)
  {
  for(unsigned int i=0;i<index.size();i++)
  hd.blocks[inode].inode.index_file.push_back(*(index.begin()++));
  }*/


bool file_t::isDirectory() const
{
    if(this->type==file_type::D)
	return true;
    return false;
}


bool inode_t::check_permission(uid_t uid, gid_t gid ) const {
    return uid==0 || ( (this->uid==uid ) && (this->gid==gid) );
}



aos::list<uint32_t> HDD::search(size_t total_size) //// AAAAAH signed size !!
{
    unsigned int blocks_num=aos::ceil((float)total_size/BLOCK_SIZE); // AAAAAAAH integer division
  
    aos::list<uint32_t> free_index;
    for(unsigned int i=0;i<blocks.size();i++)
    {
	if(blocks[i].tag==block_type::free)
	{
	    free_index.push_back(i);
	    if(free_index.size()==blocks_num)
	    {
		return free_index;
	    }
	}
    }
    return free_index;
}






