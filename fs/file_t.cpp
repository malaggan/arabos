#include <file_t.h>
#include <ata.h>
#include <string.h>

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

file_t::file_t(aos::string<20> const &  name, mode_t mode,file_type type,int inode,bool is_symlink)
	:inode(inode),
	 type(type),
	 name(name),
	 mode(mode), is_symlink(is_symlink)//,mutex()
{
	printk(DEBUG ">>>>>>>>>>in CONSTRACTOR\n ");
	printk(DEBUG ">>>>>>>>>>the name of the objectis: %s\n",name.c_str());

	// if(!isDirectory())
	//	contents = aos::make_shared<aos::vector<char>>();
}
aos::static_vector<aos::string<20>,10> file_t::split(const aos::string<128> str, const aos::string<2> delim)
{
	aos::static_vector<aos::string<20>,10> parts;
	size_t start, end = 0;
	while (end < str.size()) {
		// /printk (DEBUG  "in split parts.begin:%s   ",(*parts.begin()).c_str());
		/// printk (DEBUG  "in split str :%s   ",str.c_str());
		/// printk (DEBUG  "in split str size :%d   ",str.size());
		start = end;
		while (start < str.size() && (delim.find(str[start]) !=aos::string<2>::npos)) {
			start++;  // skip initial whitespace
		}
		end = start;
		/// printk (DEBUG  "in split end:%d  ",end);
		while (end < str.size() && (delim.find(str[end]) ==aos::string<2>::npos)) {
			end++; // skip to end of word
		}
		/// printk (DEBUG  "in split end:%d  ",end);
		/// printk (DEBUG  "in split start:%d  ",start);
		if (end-start != 0) {  // just ignore zero-length strings.
			parts.push_back(aos::string<20>(str, start,end));
			/// printk (DEBUG  "in split aftar parts.begin:%s   ",(*parts.begin()).c_str());
		}
	}
	return parts;
}

bool file_t::isSymlink() const {return is_symlink;}
/*bool file_t::isRoot() const { return !this->root; }
 */

// AAAAAAAAAAAAAAAAAAAAH return by value not reference !!!!!!!!!!!!!
int file_t::find(const aos::string<128> path)
	const
{
	printk(DEBUG "IN FIND path=%s\n",path.c_str());
	assert(path.at(0) == '/');
	assert(!path.empty());
	if(path.size() == 1)
	{
		printk(DEBUG ">>>in if loop of find 1 ");
		return 0;//ROOT
	}
	//  else if(path.at(1)=='.')
	//  return -1;
	printk(DEBUG ">>in loop of find 1 ");
	auto strs=ROOT.file.split(path,aos::string<2>{"/"});
	//strs.erase(strs.begin());
	printk(DEBUG ">>in for loop find 1 file name strs.begine():%s ",(*strs.begin()).c_str());
	return ROOT.file.find(aos::begin(strs), aos::end(strs));////ROOT.find(aos::begin(strs), aos::end(strs));
}

int  file_t::find(aos::static_vector<aos::string<20>,10>::const_iterator first, aos::static_vector<aos::string<20>,10>::const_iterator last)
	const
{
	if(type==file_type::D)
		printk(DEBUG ">>find 2 file type:\n");
	assert(type==file_type::D);
	block_t in;
	convert(in,inode);
	aos::static_vector<uint32_t,94> inode_files= in.inode.index_file;

	printk(DEBUG ">>in for loop find 2 file inode :%d    ",inode);
	printk(DEBUG ">>in for loop find 2 file name :%s \n",name.c_str());
	printk(DEBUG ">>in for loop find 2 file name *frist:%s    ",(*first).c_str());
	printk(DEBUG ">>in for THE SIZE OF INODE_FILES:%d \n",inode_files.size());
	if(inode_files.size()==0)
		return -1;

	unsigned int i;
	for(i=0;i<inode_files.size();i++){
		printk(DEBUG ">>inode_files[%d]:%d \n",i,inode_files[i]);
		block_t tmp;
		convert(tmp,inode_files[i]);
		printk(DEBUG ">>file inode file name :%s \n",tmp.file.name.c_str());
		if(tmp.file.name==*first)
		{

			printk(DEBUG ">>in for loop of inode_file:%d \n",inode_files[i]);
			printk(DEBUG ">>find distance :%d\n",aos::distance(first,last));
			if(aos::distance(first,last)==0)
			{
				printk(DEBUG ">>in find 2  if\n");
				return inode_files.at(i);
				break;
			}
			return  tmp.file.find(++first, last);
		}
		else if(i==inode_files.size()-1&&first==(--last))
		{
			printk(DEBUG ">>in find 2  else if\n");
			return -1;
		}
	}

	printk(DEBUG ">>in find 2  before end");
	printk(DEBUG ">>in *inode_files.begin()++:%d:",*inode_files.begin());//++); // WARN: increment here does not make sense

}


void file_t::add(aos::static_vector<uint32_t,8> const &index)
{
	block_t ft;
	convert(ft,inode);
	for(unsigned int i=0;i<index.size();i++)
		ft.inode.index_file.push_back(index[i]);//++); // WARN: increment here does not make sense
	convert_to_write(ft,inode);
}

void file_t::add(int index)
{
	block_t ft;
	convert(ft,inode);
	ft.inode.index_file.push_back(index);
	printk(DEBUG ">>>>>>>>>>in ADD INODE :%d: \n",inode);
	printk(DEBUG ">>>>>>>>>>in ADD FILE :%d: \n",*ft.inode.index_file.rbegin());
	printk(DEBUG ">>>>>>>>>>in ADD SIZE :%d: \n",ft.inode.index_file.size());
	convert_to_write(ft,inode);
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



aos::static_vector<uint32_t,8> HDD::search(size_t total_size) //// AAAAAH signed size !!
{
	unsigned int blocks_num=(total_size -1)/BLOCK_SIZE +1; // AAAAAAAH integer division
	printk (DEBUG  "search block_num:%d\n",blocks_num);
	aos::static_vector<uint32_t,8> free_index;
	//printk (DEBUG  "search blocks size:%d\n",blocks.size());
	for(unsigned int i=0;i<10;i++)
	{
		block_t ft;
		convert(ft,i);
		//printk (DEBUG  "search block[%d]:tag:%s\n",i,(blockobj->tag).c_Str());
		/*if(ft.tag==block_type::free)
		  printk (DEBUG  "search block free \n");
		  if(ft.tag==block_type::inode)
		  printk (DEBUG  "search block inode \n");
		  if(ft.tag==block_type::file)
		  printk (DEBUG  "search block file \n");*/
		if(ft.tag==block_type::free)
		{
			// printk (DEBUG  "search block free");
			free_index.push_back(i);
			if(free_index.size()==blocks_num)
			{
				return free_index;
			}
		}
	}
	printk (DEBUG  "sreach  free size %d \n",free_index.size());
	return free_index;
}


// uint16_t d[2-56];
//uint16_t * a;
// file_t ROOT =hd.blocks[0].change_type<file_t>("/",0777,file_type::D,3,false);
// uint16_t* b=reinterpret_cast<uint16_t*>(&ROOT);
// file_t* p1 = reinterpret_cast<file_t*>(b);

// write(b,0);
// read(d,0);
// printf("name :%d\n",( reinterpret_cast<file_t*>(b))->inode);
// printf("name :%s\n",( reinterpret_cast<file_t*>(b))->name.c_str());
// file_t* p1 = reinterpret_cast<file_t*>(d);
//  for(int i = 0; i < 10; i++)
// printf("read: %s\n",p1->name.c_str());

int convert(block_t &blockobj,int sec_num)
{
	uint16_t data[256];
	read(data,sec_num);
	blockobj =reinterpret_cast<block_t&>(data);
	return 0;
}
int convert_to_write(block_t &blockobj,int sec_num)
{
	uint16_t *data=reinterpret_cast<uint16_t*>(&blockobj);
	write(data,sec_num);
	return 0;
}






char *dirname (char *path) {
	if(!path || !*path)
		return strdup("/");//

	// remove trailing slashes (they are not part of the path) // WARN: modifies argument
	char *last = path + strnlen(path, 10000) - 1;
	while(last!=path && *last == '/') *last-- = 0;

	// find previous slash
	while(last!=path && *last != '/') last--;

	// either string is all made of slashes, or there is no slashes at all
	if(last == path) return strdup("/");//

	// last points at the last slash
	return strndup(path, last-path); // TODO: test
}

char *basename (char* path) {
	if(!path || !*path)
		return strdup("/");//

	// remove trailing slashes (they are not part of the path) // WARN: modifies argument
	char *last = path + strnlen(path, 10000) - 1;
	while(last!=path && *last == '/') *last-- = 0;

	// string is all made of slashes,
	if(last == path) return strdup("/");

	// find previous slash
	while(last!=path && *last != '/') last--;

	//  there is no slashes at all
	auto len = strnlen(path,10000);
	if(last == path && len == 1) return strdup(path);
	printk (DEBUG  "basename last:%s\n",last);
	printk (DEBUG  "basename last+1:%s\n",last+1);
	// last points at the last slash
	return strdup(last + 1); // TODO: test
}
