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

namespace aos {
    // std::chrono::system_clock::now()
    // std::begin
    template< class C > 
    auto begin( C& c ) { return c.begin(); }

    template< class C > 
    auto begin( const C& c ) { return c.begin(); }
   
    template< class T, size_t N > 
    constexpr T* begin( T (&array)[N] ) { return array; }

    template< class C > 
    constexpr auto cbegin( const C& c ) { return c.begin(); }
    // std::end
    template< class C > 
    auto end( C& c ) { return c.end(); }

    template< class C > 
    auto end( const C& c ) { return c.end(); }

    template< class T, size_t N > 
    constexpr T* end( T (&array)[N] ) { return array+N; }

    template< class C > 
    constexpr auto cend( const C& c ) { return c.end(); }

    // std::distance
    template< class InputIt >
    size_t // Note:: should be "typename std::iterator_traits<InputIt>::difference_type" 
    distance( InputIt first, InputIt last ) {
	size_t dist = 0;
	while(first++!=last) dist++;
	return dist;
    }
    // std::ceil
    int ceil(double); // TODO: implement
    // std::chrono::system_clock::to_time_t(hd.blocks[hd.blocks[file_inode].get<file_t>().inode].get<inode_t>().ctime);
    // std::make_shared
    // std::vector
    // std::unique_lock<std::mutex>
    // std::lock_guard<std::mutex>
    // std::copy
    template<class InputIt, class OutputIt>
    OutputIt copy(InputIt first, InputIt last, 
		  OutputIt d_first)
    {
	while (first != last) {
	    *d_first++ = *first++;
	}
	return d_first;
    }

    template<class InputIt, class OutputIt, class UnaryPredicate>
    OutputIt copy_if(InputIt first, InputIt last, 
		     OutputIt d_first, UnaryPredicate pred)
    {
	while (first != last) {
	    if (pred(*first))
		*d_first++ = *first;
	    first++;
	}
	return d_first;
    }
    // std::copy_n
    template< class InputIt, class Size, class OutputIt>
    OutputIt copy_n(InputIt first, Size count, OutputIt result)
    {
	if (count > 0) {
	    *result++ = *first;
	    for (Size i = 1; i < count; ++i) {
		*result++ = *++first;
	    }
	}
	return result;
    }
    // std::find
    template<class InputIt, class T>
    InputIt find(InputIt first, InputIt last, const T& value)
    {
	for (; first != last; ++first) {
	    if (*first == value) {
		return first;
	    }
	}
	return last;
    }
    // std::move
    template<typename _Tp>
    constexpr remove_reference_t<_Tp>&&
    move(_Tp&& __t) noexcept
    { return static_cast<remove_reference_t<_Tp>&&>(__t); }
    // std::back_inserter
    // back_insert_iterator is an OutputIterator that appends to a container for which it was constructed. The container's push_back() member function is called whenever the iterator (whether dereferenced or not) is assigned to. Incrementing the std::back_insert_iterator is a no-op.
    template< class Container >
    class back_insert_iterator
    {
	using value_type = void;
	using difference_type =	void;
	using pointer = void;
	using reference = void;
	Container& member;
    public:
	explicit back_insert_iterator(Container& c) : member(c) {}
	back_insert_iterator<Container>& operator*() { return *this; } // no-op
	back_insert_iterator<Container>& operator++() { return *this; } // no-op
	back_insert_iterator<Container>  operator++( int ) { return *this; } // no-op
	back_insert_iterator<Container>&
	operator=( const typename Container::value_type& value ) { member.push_back(value); return *this; }
	back_insert_iterator<Container>&
	operator=( typename Container::value_type&& value ) { member.push_back(move(value)); return *this; }	
    };
    template< class Container >
    back_insert_iterator<Container> back_inserter( Container& c )
    {
	return back_insert_iterator<Container>(c);
    }
    
    template<typename T, size_t N>
    class static_vector {
	T a[N];
    public:
	typedef size_t size_type;
	typedef T value_type;
	typedef T& reference;
	typedef T const& const_reference;
	typedef T* iterator;
	typedef T const* const_iterator;
	typedef T* reverse_iterator;
	typedef T const* const_reverse_iterator;
	reference front(); // equivalent to *begin() // TODO: implement
	const_reference front() const; // TODO: implement
	iterator begin(); // TODO: implement
	const_iterator begin() const; // TODO: implement
	reverse_iterator rbegin();
	const_reverse_iterator rbegin() const;
	iterator end(); // TODO: implement
	const_iterator end() const; // TODO: implement
	iterator erase(const_iterator, const_iterator); // TODO: implement
	iterator erase(const_iterator); // TODO: implement
	bool empty() const noexcept; // TODO: implement
	void clear() noexcept; // TODO: implement
	size_type size() const noexcept; // TODO: implement
	void push_back(value_type const &); // TODO: implement
	void push_back(value_type &&); // TODO: implement
	reference at(size_type); // TODO: implement
	const_reference at(size_type) const; // TODO: implement
	reference operator[](size_type); // TODO: implement
	const_reference operator[](size_type) const; // TODO: implement
    } __attribute__((packed));

    template<typename T>
    class vector {
	T *a;
    public:
	typedef size_t size_type;
	typedef T value_type;
	typedef T& reference;
	typedef T const& const_reference;
	typedef T* iterator;
	typedef T const* const_iterator;
	reference front(); // equivalent to *begin() // TODO: implement
	const_reference front() const; // TODO: implement
	iterator begin(); // TODO: implement
	const_iterator begin() const; // TODO: implement
	iterator end(); // TODO: implement
	const_iterator end() const; // TODO: implement
	iterator erase(const_iterator, const_iterator); // TODO: implement
	iterator erase(const_iterator); // TODO: implement
	bool empty() const noexcept; // TODO: implement
	void clear() noexcept; // TODO: implement
	iterator insert(const_iterator, value_type const &); // TODO: implement
	iterator insert(const_iterator, value_type &&); // TODO: implement
	iterator insert(const_iterator, size_type, value_type const &); // TODO: implement
	template<typename Iterator>  
	iterator insert(const_iterator, Iterator, Iterator); // TODO: implement
	size_type size() const noexcept; // TODO: implement
	void push_back(value_type const &); // TODO: implement
	void push_back(value_type &&); // TODO: implement
	reference at(size_type); // TODO: implement
	const_reference at(size_type) const; // TODO: implement
	reference operator[](size_type); // TODO: implement
	const_reference operator[](size_type) const; // TODO: implement
    } __attribute__((packed));

    class time_point{
    } __attribute__((packed));

    __time_t to_time_t(time_point); // TODO: implement

    time_point now(); // TODO: implement

    class string{
    public:
	string() {}
	/*implicit*/ string(char const*str); // TODO: implement
	typedef size_t size_type;
	typedef char value_type;
	typedef char& reference;
	typedef char const& const_reference;
	typedef char* iterator;
	typedef char const* const_iterator;
	iterator begin(); // TODO: implement
	const_iterator begin() const; // TODO: implement
	iterator end(); // TODO: implement
	const_iterator end() const; // TODO: implement
	size_t size() const noexcept; // TODO: implement
	char const* c_str() const; // TODO: implement
	reference at(size_type); // TODO: implement
	const_reference at(size_type) const; // TODO: implement
	reference operator[](size_type); // TODO: implement
	const_reference operator[](size_type) const; // TODO: implement
	iterator erase(const_iterator, const_iterator); // TODO: implement
	iterator erase(const_iterator); // TODO: implement
	bool empty() const noexcept; // TODO: implement
    } __attribute__((packed));
    bool operator==( const string& lhs, const string& rhs );

    template<typename T>
    class list {
    public:
	typedef T value_type;
	typedef T& reference;
	typedef T const& const_reference;
	typedef T* iterator;
	typedef T const* const_iterator;
	typedef T* reverse_iterator;
	typedef T const* const_reverse_iterator;
	reference front(); // equivalent to *begin() // TODO: implement
	const_reference front() const; // TODO: implement
	reference backt(); // equivalent to *rbegin() // TODO: implement
	const_reference back() const; // TODO: implement
	iterator begin(); // TODO: implement
	const_iterator begin() const; // TODO: implement
	reverse_iterator rbegin();
	const_reverse_iterator rbegin() const;
	iterator end(); // TODO: implement
	const_iterator end() const; // TODO: implement
	size_t size() const noexcept; // TODO: implement
	iterator erase(const_iterator, const_iterator); // TODO: implement
	iterator erase(const_iterator); // TODO: implement
	void push_back(value_type const &); // TODO: implement
	void push_back(value_type &&); // TODO: implement
    } __attribute__((packed));

    template<typename T, size_t N>
    class array {
	T m_array[N];
    public:
	typedef T value_type;
	array() : m_array{} {}
	T& operator[](size_t index) { return m_array[index]; } // TODO: bound checking
	T const & operator[](size_t index) const { return m_array[index]; }
	size_t size() const noexcept; // TODO: implement
    } __attribute__((packed));
}

extern "C"
char *dirname (char *path) {
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

extern "C"
char *basename (char* path) {
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
    //aos::array<char,496> data;
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
    aos::string name;
    mode_t mode;  
    bool is_symlink;
    explicit file_t(aos::string  name , mode_t mode,file_type type,int inode,bool is_symlink);
   
    bool isSymlink() const;
    bool isRoot() const;
    
    int find(aos::string const path) const; // AAAAAH return by value !
    int find(aos::list<aos::string>::const_iterator first, aos::list<aos::string>::const_iterator last) const;
   
    void add(aos::list<uint32_t> index);
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
    static constexpr uint16_t obj_size = block_size - sizeof(tag);

    /* non-static data member */
    // this is what is called a ``discriminated union''
    union {
	inode_t inode;
	data_t data;
        file_t file;
	aos::array<uint8_t,obj_size> pad;
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
  
#define getter(Type, Tag, Const)			\
    template <typename T>				\
    aos::enable_if_t<aos::is_same<T,Type>::value,	\
		     T Const &> get() Const {		\
	assert(tag == tag_v<T>);			\
	return Tag;					\
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
    aos::array<block_t,num_blocks> blocks;
     
    HDD(): blocks{}
	{
	    // blocks[1].change_type<data_t, block_t::block_type::data>();
	    blocks[2].change_type<inode_t>();
	    blocks[3].change_type<inode_t>(); //inode of the root
	}
    aos::list<uint32_t> search(size_t total_size);
};

extern HDD hd;
extern file_t& ROOT;

