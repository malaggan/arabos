namespace __detail {
    template <typename It>
    class vector_iterator : public iterator<iterator_category_t<It>,
					    value_type_t<It>,
					    difference_type_t<It>,
					    pointer_t<It>,
					    reference_t<It>>
    {
    private:
	It begin, current, end;
	int32_t version; // for invalidation. 0 means no version tracking (dummy and sentinels)
	volatile const int32_t *latest_version; // nullptr means no version tracking // TODO: use reference instead of pointer

    public:
	using iterator_category = typename iterator<iterator_category_t<It>,value_type_t<It>,difference_type_t<It>,pointer_t<It>,iterator_category_t<It>>::iterator_category;
	using value_type = typename iterator<iterator_category_t<It>,value_type_t<It>,difference_type_t<It>,pointer_t<It>,value_type_t<It>>::value_type;
	using difference_type = typename iterator<iterator_category_t<It>,value_type_t<It>,difference_type_t<It>,pointer_t<It>,difference_type_t<It>>::difference_type;
	using pointer = typename iterator<iterator_category_t<It>,value_type_t<It>,difference_type_t<It>,pointer_t<It>,pointer_t<It>>::pointer;
	using reference = typename iterator<iterator_category_t<It>,value_type_t<It>,difference_type_t<It>,pointer_t<It>,reference_t<It>>::reference;

	vector_iterator() : begin{nullptr}, current{begin}, end{begin}, version{0}, latest_version{nullptr} { }

	// implicitly convert iterator to const_iterator
	operator vector_iterator<remove_pointer_t<It> const *>() const
	{ return vector_iterator<remove_pointer_t<It> const *>{begin,current,end,latest_version}; }

	void check_bounds(size_t line) const {
	    if(latest_version != nullptr) if(version != *latest_version) printk(ERROR "vector: invalid iterator");
	    if(current == nullptr)
		printk(ERROR "vector: current (%x, %x, %x) is null! (line %d)\n", begin, current, end, line),
		    printk(DEBUG "v = %d,lv = %d\n", version, *latest_version);
	    if(current < begin)
		printk(ERROR "vector: current (%x) < begin (%x)! (line %d)\n", current, begin, line),
		    printk(DEBUG "v = %d,lv = %d\n", version, *latest_version);
	    if(current > end)
		printk(ERROR "vector: current (%x) > end (%x)! (line %d)\n", current, end, line),
		    printk(DEBUG "v = %d,lv = %d\n", version, *latest_version);
	}

	explicit vector_iterator(It _begin, It _end, volatile const int32_t * _latest_version)
	    : begin{_begin}, current{_begin}, end{_end}, version{*_latest_version}, latest_version{_latest_version}
	{ check_bounds(__LINE__); }

	explicit vector_iterator(It _begin, It _current, It _end, volatile const int32_t * _latest_version)
	    : begin{_begin}, current{_current}, end{_end}, version{*_latest_version}, latest_version{_latest_version}
	{ check_bounds(__LINE__); }

	auto get() { return current; }

	reference operator*() const {
	    // TODO: bounds checking
	    check_bounds(__LINE__);
	    return *current;
	}

	pointer operator->() const { return &(operator*()); }

	vector_iterator& operator++() {
	    ++current;
	    check_bounds(__LINE__);
	    return *this;
	}

	vector_iterator operator++(int) {
	    vector_iterator tmp = *this;
	    ++current;
	    check_bounds(__LINE__);
	    return tmp;
	}

	vector_iterator& operator--() {
	    --current;
	    return *this;
	}

	vector_iterator operator--(int) {
	    vector_iterator tmp = *this;
	    --current;
	    check_bounds(__LINE__);
	    return tmp;
	}

	vector_iterator operator+(difference_type n) const {
	    auto p = vector_iterator{begin, current + n, end, latest_version};
	    p.version = this->version;
	    check_bounds(__LINE__);
	    return p;
	}

	vector_iterator& operator+=(difference_type n) {
	    current += n;
	    check_bounds(__LINE__);
	    return *this;
	}

	vector_iterator operator-(difference_type n) const {
	    auto p = vector_iterator{begin, current - n, end, latest_version};
	    p.version = this->version;
	    check_bounds(__LINE__);
	    return p;
	}

	vector_iterator& operator-=(difference_type n) {
	    current -= n;
	    check_bounds(__LINE__);
	    return *this;
	}

	reference operator[](difference_type n) const { return *(*this + n); }

	It base() const {return current;}
    };

    template<typename ItL, typename ItR>
    inline bool
    operator==(const vector_iterator<ItL>& x,
	       const vector_iterator<ItR>& y)
    { return x.base() == y.base(); }

    template<typename ItL, typename ItR>
    inline bool
    operator<(const vector_iterator<ItL>& x,
	      const vector_iterator<ItR>& y)
    { return x.base() < y.base(); }

    template<typename ItL, typename ItR>
    inline bool
    operator!=(const vector_iterator<ItL>& x,
	       const vector_iterator<ItR>& y)
    { return !(x == y); }

    template<typename ItL, typename ItR>
    inline bool
    operator>(const vector_iterator<ItL>& x,
	      const vector_iterator<ItR>& y)
    { return y < x; }

    template<typename ItL, typename ItR>
    inline bool
    operator<=(const vector_iterator<ItL>& x,
	       const vector_iterator<ItR>& y)
    { return !(x > y); }

    template<typename ItL, typename ItR>
    inline bool
    operator>=(const vector_iterator<ItL>& x,
	       const vector_iterator<ItR>& y)
    { return !(x < y); }

    template<typename ItL, typename ItR>
    inline auto
    operator-(const vector_iterator<ItL>& x,
	      const vector_iterator<ItR>& y)
    { return x.base() - y.base(); }

}

template<typename T>
class vector {
public:
    using size_type		= size_t;
    using value_type		= T;
    using reference		= value_type&;
    using const_reference	= value_type const&;
    using iterator		= __detail::vector_iterator<value_type      *>;
    using const_iterator	= __detail::vector_iterator<value_type const*>;
private:
    shared_array<value_type> m_array;
    uint32_t m_capacity;
    uint32_t m_size;
    int32_t m_version;

    using pointer = pointer_t<iterator>;
    using const_pointer = const_pointer_t<iterator>;

    void check_bounds(const_pointer p) const {
	if(p == nullptr)
	    printk(ERROR "vector: p (%x, %x, %x) is null! (__LINE__ %d)\n", m_array.get(), p, m_array.get() + m_size, __LINE__);
	if(p < m_array.get())
	    printk(ERROR "vector: p (%x) < m_array.get() (%x)! (__LINE__ %d)\n", p, m_array.get(), __LINE__);
	if(p > m_array.get() + m_size)
	    printk(ERROR "vector: p (%x) > m_array.get() + m_size (%x)! (__LINE__ %d)\n", p, m_array.get() + m_size, __LINE__);
    }

    iterator _create_iterator(pointer p) {
	if(p == nullptr)
	    panic("create_iterator: null p\n");
	check_bounds(p);
	return iterator{m_array.get(), p, m_array.get() + m_size, &m_version};
    }
    const_iterator _create_const_iterator(const_pointer p) const {
	if(p == nullptr)
	    panic("create_const_iterator: null p\n");
	check_bounds(p);
	return const_iterator{m_array.get(), p, m_array.get() + m_size, &m_version};
    }
public:

    // NOTE: value_type must be default constructible. Otherwise, we are going to have to use uninitialized storage and uninitialized_copy/move
    explicit vector() : m_array{new value_type[8]}, m_capacity{8}, m_size{0}, m_version{1} {}

    iterator begin() {
	return iterator{m_array.get(), m_array.get()+m_size, &m_version};
    }
    const_iterator begin() const {
	return const_iterator{m_array.get(), m_array.get()+m_size, &m_version};
    }
    const_iterator cbegin() const {
	return const_iterator{m_array.get(), m_array.get()+m_size, &m_version};
    }

    iterator end() { return _create_iterator(m_array.get()+m_size); } // TODO: adapt as sentinel
    const_iterator end() const { return _create_const_iterator(m_array.get()+m_size); }
    const_iterator cend() const { return _create_const_iterator(m_array.get()+m_size); }

    reference front() {
	return *begin();
    }
    const_reference front() const {
	return *begin();
    }

    iterator erase(const_iterator first, const_iterator last) {
	if(first == last)
	    if(last == end())
		return last;
	    else
	    {
		++last;
		return last;
	    }
	while(first != last)
	{   // Note: probably there are more efficient ways to do this
	    erase(first);
	    ++first;
	}
	return end();
    }
    iterator erase(const_iterator it) {
	if(it < begin())
	    return printf("vector: it < begin()\n"),end();
	if(it >= end())
	    return printf("vector: it >= end()\n"),end();

	pointer p = const_cast<pointer>(it.get());

	p->~value_type();
	move(p+1, m_array.get() + m_size, p);
	--m_size;
	++m_version; // erase() is invalidating

	if(p >= m_array.get() + m_size)
	    return end();
	else
	    return _create_iterator(p);
    }

    void guarantee_capacity(size_t minimum_capacity) {
	if(m_capacity >= minimum_capacity)
	    return;

	m_version++; // TODO: make atomic.

	auto new_capacity = m_capacity * 3;
	if(new_capacity / 3 != m_capacity)
	    panic("vector: guarantee_capacity overflow\n");

	shared_array<value_type> new_array = shared_array<value_type>{new value_type[new_capacity]};
	move(m_array.get(), m_array.get() + m_size, new_array.get());
	swap(m_array, new_array); 

	m_capacity = new_capacity;
    }

    iterator insert(const_iterator pos, value_type const &value) {
	// insert value before pos. pos may be end()
	if(pos < begin() || pos > end())
	    return end();

	auto dist = distance(cbegin(), pos);
	// pos will be invalidated after this
	guarantee_capacity(m_size+1);
	pos = cbegin() + dist;

	// the new end() is not initialized
	new (m_array.get() + m_size) value_type(); // default constructible assumption.
	// To avoid default constructible assumption we have to do
	// uninitialized_copy_backward, but call the destructors for
	// all elements except "new end()".

	// move pos to pos+1 (make space).
	move_backward(pos, cend(),iterator{m_array.get(), m_array.get() + m_size + 1, m_array.get() + m_size + 1, &m_version});

	auto p = const_cast<pointer>(pos.get()); // no need to subtract: "before pos" is now "pos"
	*p = value;

	++m_size;

	return _create_iterator( p );
    }
    iterator insert(const_iterator pos, value_type &&value) {
	// same as insert(const_iterator, value_type const &) but using move
	if(pos < begin() || pos > end())
	    return end();

	auto dist = distance(cbegin(), pos);
	// pos will be invalidated after this
	guarantee_capacity(m_size+1);
	pos = cbegin() + dist;

	new (m_array.get() + m_size) value_type();

	move_backward(pos, const_iterator{end()},iterator{m_array.get(), m_array.get() + m_size + 1, m_array.get() + m_size + 1, &m_version});

	auto p = const_cast<pointer>(pos.get());
	*p = move(value);

	++m_size;

	return _create_iterator( p );
    }
    iterator insert(const_iterator pos, size_type n, value_type const &value) {
	// same as insert(const_iterator, Iterator, Iterator) but using same vlue
	if(pos < begin() || pos > end())
	    return end();

	auto dist = distance(cbegin(), pos);
	// pos will be invalidated after this
	guarantee_capacity(m_size+n);
	pos = cbegin() + dist;

	if(n == 0) return begin() + dist;

	while(n --> 0)
	    insert(cbegin() + (dist), value);

	return begin() + dist;
    }
    template<typename Iterator>
    enable_if_t<
	!is_same_v<
	    value_type_t<Iterator>,
	    void>,
	iterator>
    insert(const_iterator pos, Iterator first, Iterator last) {
	if(pos < begin() || pos > end())
	    return end();

	auto dist = distance(cbegin(), pos);
	// pos will be invalidated after this
	guarantee_capacity(m_size + distance(first, last));
	pos = cbegin() + dist;

	if(first == last) return begin() + dist; //pos; cannot return pos because it is const_iterator, but we return iterator

	while(first != last)
	    insert(cbegin() + (dist++), *first++); // TODO: can be done more efficiently (insert by bulk)

	return begin() + dist; // must do it here to account for possible pointer invalidation due to resize
    }

    bool empty() const noexcept {
	return m_size == 0;
    }
    void clear() noexcept {
	m_size = 0;
    }

    size_type size() const noexcept {
	return m_size;
    }

    void push_back(value_type const &v) {
	guarantee_capacity(m_size+1);
	auto p = m_array.get() + m_size; // uninitialized
	new (p) value_type(v); // copy constrictuble
	++m_size;
    }
    void push_back(value_type &&v) {
	guarantee_capacity(m_size+1);
	auto p = m_array.get() + m_size;
	new (p) value_type(move(v));
	++m_size;
    }

    reference at(size_type n) {
	return *(begin() + n);
    }
    const_reference at(size_type n) const {
	return *(begin() + n);
    }

    reference operator[](size_type n) { return at(n); }
    const_reference operator[](size_type n) const { return at(n); }
} __attribute__((packed));
