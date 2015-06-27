// -*- mode: c++; -*-
// #define FUSE_USE_VERSION 26
#pragma once
#define UNUSED __attribute__((__unused__))

// #include <boost/container/static_vector.hpp>
// #include <boost/optional.hpp>
// #include <condition_variable>
// #include <boost/mpl/set.hpp>
// #include <type_traits>
// #include <functional>
// #include <cstring>
// #include <cassert>
// #include <memory>
// #include <vector>
// #include <chrono>
// #include <array>
// #include <cmath>
// #include <list>
// #include <string>
// #include <array>
// #include <map>
// #include <utility>

#include <types.h>
#include <mm.h> // for placement new. // TODO: move new operators to <new.h>
#include <lib.h> // for assert and printk
#include <stl/iterator.hh>

constexpr uint16_t BLOCK_SIZE = 512;
constexpr uint16_t NUM_BLOCKS = 100; // TODO: read from ATA

typedef int64_t off_t;

enum {EACCES=1, EEXIST, ENOENT, ENOTDIR};

constexpr uint64_t num_blocks = NUM_BLOCKS;

enum class block_type : uint8_t { free, data, inode, file};
/* File types.  */
#define	S_IFDIR	0040000	/* Directory.  */
// #define	S_IFCHR	0020000	/* Character device.  */
// #define	S_IFBLK	0060000	/* Block device.  */
#define	S_IFREG	0100000	/* Regular file.  */
// #define	S_IFIFO	0010000	/* FIFO.  */
#define	S_IFLNK	0120000	/* Symbolic link.  */
// #define	S_IFSOCK 0140000	/* Socket.  */
#define	__S_IREAD	0400	/* Read by owner.  */
#define	S_IRUSR	__S_IREAD	/* Read by owner.  */
#define	S_IRGRP	(S_IRUSR >> 3)	/* Read by group.  */
#define	S_IROTH	(S_IRGRP >> 3)	/* Read by others.  */
struct stat
{
	// __dev_t st_dev;			/* Device.				*/
	// __ino_t __st_ino;			/* 32bit file serial number.		*/
	mode_t st_mode;			/* File mode.				*/
	size_t st_nlink;			/* Link count.				*/
	uid_t st_uid;			/* User ID of the file's owner.		*/
	gid_t st_gid;			/* Group ID of the file's group.	*/
	// __dev_t st_rdev;			/* Device number, if device.		*/
	uint64_t st_size;			/* Size of file, in bytes.		*/
	// __blksize_t st_blksize;		/* Optimal block size for I/O.		*/
	// __blkcnt64_t st_blocks;		/* Number 512-byte blocks allocated.	*/
	__time_t st_atime;			/* Time of last access.			*/
	// __syscall_ulong_t st_atimensec;	/* Nscecs of last access.		*/
	__time_t st_mtime;			/* Time of last modification.		*/
	// __syscall_ulong_t st_mtimensec;	/* Nsecs of last modification.		*/
	__time_t st_ctime;			/* Time of last status change.		*/
	// __syscall_ulong_t st_ctimensec;	/* Nsecs of last status change.		*/
	// __ino64_t st_ino;			/* File serial number.			*/
};

inline char *dirname (char *path) __attribute__((always_inline));
inline char *basename (char* path) __attribute__((always_inline));

inline char *dirname (char *path) {
	if(!path || !*path)
		return strdup(".");

	// remove trailing slashes (they are not part of the path) // WARN: modifies argument
	char *last = path + strnlen(path, 10000) - 1;
	while(last!=path && *last == '/') *last-- = 0;

	// find previous slash
	while(last!=path && *last != '/') last--;

	// either string is all made of slashes, or there is no slashes at all
	if(last == path) return strdup(".");

	// last points at the last slash
	return strndup(path, last-path); // TODO: test
}

inline char *basename (char* path) {
	if(!path || !*path)
		return strdup(".");

	// remove trailing slashes (they are not part of the path) // WARN: modifies argument
	char *last = path + strnlen(path, 10000) - 1;
	while(last!=path && *last == '/') *last-- = 0;

	// string is all made of slashes,
	if(last == path) return strdup("/");

	// find previous slash
	while(last!=path && *last != '/') last--;

	//  there is no slashes at all
	if(last == path) return strdup(path);

	// last points at the last slash
	return strdup(last + 1); // TODO: test
}

struct inode_t
{
	inode_t(inode_t const  &i);
	inode_t();
	aos::static_vector<uint32_t,94> index_file;
	static constexpr block_type tag = block_type::inode;
	size_t size;
	int usecount;
	aos::time_point ctime, atime, mtime;
	uid_t uid;
	gid_t gid;
	int readcount, writecount;
	int link;
	bool check_permission(uid_t uid, gid_t gid ) const;
	//what about map some thing to think
	// using map_l = aos::map< aos::string, int >;
	// union: map_l index_map; // must use fixed-size map!
} __attribute__((packed));

struct data_t
{
	//aos::static_vector<uint8_t,496> data;   //char not int
	aos::static_vector<char,496> data;
	//aos::static_vector<char,496> data;
	static constexpr block_type tag = block_type::data;
} __attribute__((packed));

// Exceptions
class FileExists{};
enum class file_type: uint8_t {F,D};

struct file_t
{
	static constexpr block_type tag = block_type::file;
	int inode;
	file_type type;
	aos::string<20> name;
	mode_t mode;
	bool is_symlink;
	explicit file_t(aos::string<20> const & name , mode_t mode, file_type type, int inode, bool is_symlink=false);

	bool isSymlink() const;
	bool isRoot() const;

	int find(aos::string<128> const path) const; // AAAAAH return by value !
	int find(aos::static_vector<aos::string<20>,10>::const_iterator first, aos::static_vector<aos::string<20>,10>::const_iterator last) const;

	void split(const aos::string<128>& str,const aos::string<2>& delim,aos::static_vector<aos::string<20>,10>& parts); // TODO: should be static
	void add(aos::static_vector<uint32_t,8> const & index);
	void add(int index);
	bool isDirectory() const;
	~file_t(){} // AAAAH non-virtual destructor ! fine, if the class is final (keyword?)
} __attribute__((packed));



template< typename Type, size_t ExpectedSize, size_t ActualSize = 0 >
struct validate_size : aos::true_type
{
	static_assert( ActualSize == ExpectedSize,
	               "actual size does not match expected size" );
};

template< typename Type, size_t ExpectedSize >  //partial specialization
struct validate_size< Type, ExpectedSize, 0 >
	: validate_size< Type, ExpectedSize, sizeof( Type ) >  //what type is to get its size and
{};


//make specialization cuz if true call one and if false call 2
template<typename T, size_t s = sizeof(T::tag)> // TODO check that tag is static, constexpr, with type block_type
struct has_tag : aos::true_type {
	static_assert(T::tag==block_type::file, "invalid tag");
	static_assert(T::tag==block_type::free, "invalid tag");
	static_assert(T::tag==block_type::inode, "invalid tag");
};

template<typename T>
struct has_tag<T, 0> : aos::false_type {
};

template<typename T>
bool has_tag_v = has_tag<T>::value;  //conatain the variable of the tag template true or false

template<typename T, aos::enable_if_t<has_tag_v<T>,void>* = true>  //??  maybe to ge tag of the block ---why *
block_type tag_v = T::tag;

//specializaton to if has tag callone and if no tag call 2

template<typename T>
block_type tag_v<T,0> = block_type::free;

// I do not encode block_type in block_t template parameter to
// enabe type-erasure (so we can store all blocks in the same
// array).  I also cannot use class polymorphism (type-erasure or
// not) since thisis struct will be reloaded from disk, and class
// polymorphism uses pointers, and there is no guarantee that
// pointers remain valid when the block is loaded from disk.
// We need to use (extended) unions.
struct block_t {
	/* static data member */
	static constexpr uint16_t block_size = BLOCK_SIZE;

	/* non-static data member */
	block_type tag;

	/* static data member */
	static constexpr uint16_t obj_size = block_size - sizeof(tag) - sizeof(uint32_t) /* because we are using static_vector instead of array */;

	/* non-static data member */
	// this is what is called a ``discriminated union''
	union {
		inode_t inode;
		data_t data;
		file_t file;
		aos::static_vector<uint8_t,obj_size> pad;
	};

	// copy constructor
	block_t(block_t const &o) : block_t() {
		tag = o.tag;
		// no need to call the dtor since this is a ctor! no previous
		// instances exist.
		switch(tag)
		{
		case block_type::data:
			new (&data) data_t(o.data);
		case block_type::file:
			new (&file) file_t(o.file.name,o.file.mode,o.file.type,o.file.inode,o.file.is_symlink);
			// new (&file) file_t(file.name,file.mode,file.type,file.inode,file.is_symlink);
			// new (&file) file_t(o.file);
			break;
		case block_type::inode:
			new (&inode) inode_t(o.inode);
			break;
		case block_type::free:
			// do nothing.
			break;
		}
	}

	~block_t() {
		// FIXME: We should not call the destructor when the object is
		// removed from memory, since it might still live on disk!
		switch(tag)
		{
		case block_type::data:
			data.~data_t();
		case block_type::file:
			file.~file_t();
			break;
		case block_type::inode:
			inode.~inode_t();
			break;
		case block_type::free:
			// do nothing.
			break;
		}
	}

	// default ctor
	block_t() : tag{block_type::free} {
		static_assert(validate_size< block_t, block_size >::value, "sizeof(block_t) != block_size");
	}

	template <typename T, typename ...Args>
	T& change_type(Args ...args) {
		this->~block_t(); //why ????
		tag = tag_v<T>;
		new (&pad) T(aos::forward<Args>(args)...);   //Returns an rvalue reference to arg if arg is not an lvalue reference
		return get<T>();
	}
	//get<>:Returns a reference to the Ith element of tuple tpl.

#define getter(Type, Tag, Const)	  \
	template <typename T> \
	aos::enable_if_t<aos::is_same<T,Type>::value, \
	                 T Const &> get() Const { \
		assert(tag == tag_v<T>); \
		return Tag; \
	}

	getter(inode_t	, inode     , const)
	getter(inode_t	, inode     ,      )
	getter(file_t	, file     , const)
	getter(file_t	, file      ,      )
	getter(data_t	, data      , const)
	getter(data_t	, data      ,      )
};
//bt return el tag if is valid
// ~/opt/gcc/bin/g++  afs_create.cc -c -D_FILE_OFFSET_BITS=64 -I/usr/include/fuse  -std=c++14 -DBLOCK_SIZE=512 -DNUM_BLOCKS=100 -o afs_create.o |& less
struct HDD
{
public:
	aos::static_vector<block_t,num_blocks> blocks;

	HDD(): blocks{}
		{
			// blocks[1].change_type<data_t, block_t::block_type::data>();
			blocks[2].change_type<inode_t>();
			blocks[3].change_type<inode_t>(); //inode of the root
		}
	aos::static_vector<uint32_t,8> search(size_t total_size);
};

extern HDD hd;
extern file_t& ROOT;

aos::static_vector<aos::string<20>,10> sfs_readdir (const char *path);
