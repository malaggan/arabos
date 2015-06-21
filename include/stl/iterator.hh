// -*- mode: c++; -*-

#pragma once

#include <types.h> // for ptrdiff_t and move

namespace aos {

#include <stl/bits/iterator_type.hh>
#include <stl/bits/iterator_traits.hh>
#include <stl/bits/range_access.hh>
    
    template<class InputIt>
    difference_type_t<InputIt>
    distance(InputIt first, InputIt last) {
	difference_type_t<InputIt> dist = 0;
	while(first++ != last) dist++;
	return dist;
    }

    // TODO: distance can be made more efficient for random_access_iterators
    // TODO: advance()


#include <stl/bits/back_insert_iterator.hh>
#include <stl/bits/reverse_iterator.hh>
    
    // std::ceil
    // std::chrono::system_clock::now()
    int ceil(double); // TODO: implement
    // std::chrono::system_clock::to_time_t(hd.blocks[hd.blocks[file_inode].get<file_t>().inode].get<inode_t>().ctime);
    // std::make_shared
    // std::unique_lock<std::mutex>
    // std::lock_guard<std::mutex>
#include <stl/bits/copy.hh>
#include <stl/bits/copy_if.hh>
#include <stl/bits/copy_n.hh>
#include <stl/bits/find.hh>

    
    template<typename T,
	     typename Iterator,
	     typename Const_iterator,
	     typename Reverse_iterator,  
	     typename Size_type = size_t,
	     typename Pointer = T*,
	     typename Reference = T&,
	     typename Const_reference = T const &>
    struct container
    {
	//using  value_type	 = T        	;
	//using  difference_type	 = Distance 	;
	//using  pointer		 = Pointer  	;
	//using  reference	 = Reference	;
    };

    
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
