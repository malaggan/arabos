// -*- mode: c++; -*-

#pragma once

#include <types.h> // for ptrdiff_t and move

namespace aos {
    struct input_iterator_tag { };
    struct output_iterator_tag { };
    struct forward_iterator_tag : public input_iterator_tag { };
    struct bidirectional_iterator_tag : public forward_iterator_tag { };
    struct random_access_iterator_tag : public bidirectional_iterator_tag { };

    template<typename Category, typename T, typename Distance = ptrdiff_t, typename Pointer = T*, typename Reference = T&>
    struct iterator
    {
	using  iterator_category = Category 	;
	using  value_type	 = T        	;
	using  difference_type	 = Distance 	;
	using  pointer		 = Pointer  	;
	using  reference	 = Reference	;
    };
    // N3844 (for explanation), and N3923 for most recent wording
    template<typename...> using void_t = void;
    
    template<typename It, typename = void_t<>>
	struct __iterator_traits { };
    
    template<typename It>
    struct __iterator_traits<_Iterator, void_t<typename It::iterator_category,
					       typename It::value_type,
					       typename It::difference_type,
					       typename It::pointer,
					       typename It::reference>>
    {
        using iterator_category = typename It::iterator_category;
        using value_type        = typename It::value_type;
        using difference_type   = typename It::difference_type;
        using pointer           = typename It::pointer;
        using reference         = typename It::reference;
    };

    template<typename It>
    struct iterator_traits : public __iterator_traits<_Iterator> { };


    template<typename T>
    struct iterator_traits<T*>
    {
        using iterator_category = random_access_iterator_tag;
        using value_type        = T;
        using difference_type   = ptrdiff_t;
        using pointer           = T*;
        using reference         = T&;
    };


    template<typename T>
    struct iterator_traits<T const *>
    {
        using iterator_category = random_access_iterator_tag;
        using value_type        = T;
        using difference_type   = ptrdiff_t;
        using pointer           = T const *;
        using reference         = T const &;
    };

    
    template<class C> auto begin			(C &c)		{ return c.begin(); }
    template<class C> auto end				(C &c)		{ return c.end(); }
    template<class C> auto begin			(C const &c)	{ return c.begin(); }
    template<class C> auto end				(C const &c)	{ return c.end(); }
    template<class T, size_t N> constexpr T* begin	(T (&array)[N])	{ return array; }
    template<class T, size_t N> constexpr T* end	(T (&array)[N])	{ return array+N; }
    template<class C> auto cbegin			(C const &c)	{ return c.begin(); }
    template<class C> auto cend				(C const &c)	{ return c.end(); }

    // not std c++, but helpful:
    template <typname It> using iterator_category_t	= typename iterator_category<It>::iterator_category;
    template <typname It> using value_type_t		= typename iterator_category<It>::value_type;
    template <typname It> using difference_type_t	= typename iterator_category<It>::difference_type;   
    template <typname It> using pointer_t		= typename iterator_category<It>::pointer;
    template <typname It> using reference_t		= typename iterator_category<It>::reference;

    template<class InputIt>
    difference_type_t<InputIt>
    distance(InputIt first, InputIt last) {
	difference_type_t<InputIt> dist = 0;
	while(first++ != last) dist++;
	return dist;
    }

    // TODO: distance can be made more efficient for random_access_iterators
    // TODO: advance()


    // back_insert_iterator is an OutputIterator that appends to a container for which it was constructed. The container's push_back() member function is called whenever the iterator (whether dereferenced or not) is assigned to. Incrementing the back_insert_iterator is a no-op.
    template<class Container>
    class back_insert_iterator : public iterator<output_iterator_tag, void, void, void, void>
    {
	Container &member;
    public:
	explicit back_insert_iterator(Container &c) : member(c) {}
	back_insert_iterator<Container>& operator*() { return *this; } // no-op
	back_insert_iterator<Container>& operator++() { return *this; } // no-op
	back_insert_iterator<Container>  operator++(int) { return *this; } // no-op
	back_insert_iterator<Container>&
	operator=(const value_type_t<Container> &value) { member.push_back(value); return *this; }
	back_insert_iterator<Container>&
	operator=(value_type_t<Container> &&value ) { member.push_back(move(value)); return *this; }
    };

    template<class Container> back_insert_iterator<Container> back_inserter(Container &c)
    {
	return back_insert_iterator<Container>(c);
    }

    template<typename It>
    class reverse_iterator : public iterator<iterator_category_t<It>,
					     value_type_t<It>,
					     difference_type_t<It>,
					     pointer_t<It>,
					     reference_t<It> >
    {
    private:
	It current;

    public:
        using iterator_type = It;

	reverse_iterator() : current{} { }

	explicit reverse_iterator(iterator_type x) : current(x) { }

	reverse_iterator(const reverse_iterator& x) = default;

	template<typename Iter> reverse_iterator(const reverse_iterator<Iter>& x) : current(x.base()) { }

	iterator_type base() const { return current; }

	reference operator*() const {
	    It tmp = current;
	    return *--tmp;
	}

	pointer operator->() const { return &(operator*()); }

	reverse_iterator& operator++() {
		--current;
		return *this;
	    }

	reverse_iterator operator++(int) {
		reverse_iterator tmp = *this;
		--current;
		return tmp;
	    }

	reverse_iterator& operator--() {
		++current;
		return *this;
	    }

	reverse_iterator operator--(int) {
		reverse_iterator tmp = *this;
		++current;
		return tmp;
	    }

	reverse_iterator operator+(difference_type n) const { return reverse_iterator(current - n); }

	reverse_iterator& operator+=(difference_type n) {
		current -= n;
		return *this;
	    }

	reverse_iterator operator-(difference_type n) const { return reverse_iterator(current + n); }

	reverse_iterator& operator-=(difference_type n) {
		current += n;
		return *this;
	    }

	reference operator[](difference_type n) const { return *(*this + n); }
    };


    template<typename ItL, typename ItR>
    inline bool
    operator==(const reverse_iterator<ItL>& x,
	       const reverse_iterator<ItR>& y)
    { return x.base() == y.base(); }

    template<typename ItL, typename ItR>
    inline bool
    operator<(const reverse_iterator<ItL>& x,
	      const reverse_iterator<ItR>& y)
    { return y.base() < x.base(); }

    template<typename ItL, typename ItR>
    inline bool
    operator!=(const reverse_iterator<ItL>& x,
	       const reverse_iterator<ItR>& y)
    { return !(x == y); }

    template<typename ItL, typename ItR>
    inline bool
    operator>(const reverse_iterator<ItL>& x,
	      const reverse_iterator<ItR>& y)
    { return y < x; }

    template<typename ItL, typename ItR>
    inline bool
    operator<=(const reverse_iterator<ItL>& x,
	       const reverse_iterator<ItR>& y)
    { return !(y < x); }

    template<typename ItL, typename ItR>
    inline bool
    operator>=(const reverse_iterator<ItL>& x,
	       const reverse_iterator<ItR>& y)
    { return !(x < y); }

    template<typename ItL, typename ItR>
    inline auto
    operator-(const reverse_iterator<ItL>& x,
	      const reverse_iterator<ItR>& y)
    { return y.base() - x.base(); }

    template<typename It>
    inline reverse_iterator<It>
    make_reverse_iterator(It i)
    { return reverse_iterator<It>(i); }



    
    
    // std::ceil
    // std::chrono::system_clock::now()
    int ceil(double); // TODO: implement
    // std::chrono::system_clock::to_time_t(hd.blocks[hd.blocks[file_inode].get<file_t>().inode].get<inode_t>().ctime);
    // std::make_shared
    // std::unique_lock<std::mutex>
    // std::lock_guard<std::mutex>
#include <bits/copy.hh>
#include <bits/copy_if.hh>
#include <bits/copy_n.hh>
#include <bits/find.hh>

    
    template<typename value_type,
	     typename Iterator, typename const_iterator, typename reverse_iterator, typename 
	     typename Size_type = size_t,
	     typename Pointer = value_type*,
	     typename Reference = value_type&,
	     typename Const_reference = value_type const &>
    struct container
    {
	using  iterator_category = Category 	;
	using  value_type	 = T        	;
	using  difference_type	 = Distance 	;
	using  pointer		 = Pointer  	;
	using  reference	 = Reference	;
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
