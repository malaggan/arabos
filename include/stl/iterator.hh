#pragma once
namespace aos {
    template<typename T, typename Distance = ptrdiff_t,
	     typename Pointer = T*, typename Reference = T&>
    struct iterator
    {
      typedef T        value_type;
      typedef Distance  difference_type;
      typedef Pointer   pointer;
      typedef Reference reference;
    };
    
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
    // std::unique_lock<std::mutex>
    // std::lock_guard<std::mutex>
#include <bits/copy.hh>
#include <bits/copy_if.hh>
#include <bits/copy_n.hh>
#include <bits/find.hh>
    
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
	typedef size_t size_type;
	typedef char value_type;
	typedef char& reference;
	typedef char const& const_reference;
	typedef char* iterator;
	typedef char const* const_iterator;

	string() {}
	string( const string& other,
              size_type pos,
              size_type count = npos);
	/*implicit*/ string(char const*str); // TODO: implement
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
	static const size_type npos = -1;
	size_type find( const string& str, size_type pos = 0 ) const;
	size_type find( const char* s, size_type pos, size_type count ) const;
	size_type find( const char* s, size_type pos = 0 ) const;
	size_type find( char ch, size_type pos = 0 ) const;
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
