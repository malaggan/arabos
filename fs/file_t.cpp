#include <file_t.h>
#include <ata.h>
#include <string.h>

block_t read_block(sector_t sector_num) {
	block_t b; b.read(sector_num);
	return b;
}

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
	  readcount(0), writecount(0), link(1)
{}

file_t::file_t(aos::string<20> const &  name, mode_t mode,file_type type,int inode,bool is_symlink)
	:inode(inode),
	 type(type),
	 name(name),
	 mode(mode), is_symlink(is_symlink)//,mutex()
{}

aos::static_vector<aos::string<20>,10>
file_t::split(const aos::string<128> str,
              const aos::string<2> delim)
{
	aos::static_vector<aos::string<20>,10> parts;
	size_t start, end = 0;
	while (end < str.size()) {
		start = end;
		while (start < str.size() && (delim.find(str[start]) !=aos::string<2>::npos)) {
			start++;  // skip initial whitespace
		}
		end = start;
		while (end < str.size() && (delim.find(str[end]) ==aos::string<2>::npos)) {
			end++; // skip to end of word
		}
		if (end-start != 0) {  // just ignore zero-length strings.
			parts.push_back(aos::string<20>(str, start,end));
		}
	}
	return parts;
}

bool file_t::isSymlink() const {return is_symlink;}

int file_t::find(const aos::string<128> path)	const
{
	assert(path.at(0) == '/');

	assert(!path.empty());

	if(path.size() == 1)
		return 0; //ROOT

	auto strs = ROOT.file.split(path, aos::string<2>{"/"});

	return ROOT.file.find(aos::begin(strs), aos::end(strs));
}

int file_t::find(aos::static_vector<aos::string<20>,10>::const_iterator first,
                 aos::static_vector<aos::string<20>,10>::const_iterator last) const
{
	assert(type==file_type::D);

	block_t in = read_block(inode);

	aos::static_vector<uint32_t,94> inode_files= in.inode.index_file;

	if(inode_files.empty())
		return -1;

	for(auto inode : inode_files) {
		block_t tmp = read_block( inode );

		if(tmp.file.name == *first)
			return (++first == last) ? inode : tmp.file.find(first, last);
	}

	return -1;
}

void file_t::add(aos::static_vector<uint32_t,8> const &index)
{
	block_t ft = read_block(inode);
	for(unsigned int i=0; i<index.size(); i++)
		ft.inode.index_file.push_back(index[i]);
	ft.write(inode);
}

void file_t::add(int index)
{
	block_t ft = read_block(inode);
	ft.inode.index_file.push_back(index);
	ft.write(inode);
}

bool file_t::isDirectory() const
{
	if(this->type==file_type::D)
		return true;
	return false;
}

bool inode_t::check_permission(uid_t uid, gid_t gid ) const {
	return uid==0 || ( (this->uid==uid ) && (this->gid==gid) );
}

aos::static_vector<uint32_t,8> HDD::search(size_t total_size) {
	unsigned int blocks_num=(total_size -1)/BLOCK_SIZE +1;
	aos::static_vector<uint32_t,8> free_index;
	for(unsigned int i=0;i<10;i++)
		if(read_block(i).tag==block_type::free)
		{
			free_index.push_back(i);
			if(free_index.size()==blocks_num)
				return free_index;
		}

	return free_index;
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

	// last points at the last slash
	return strdup(last + 1); // TODO: test
}
