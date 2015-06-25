namespace __detail {
    template <typename It>
    class static_vector_iterator : public iterator<iterator_category_t<It>,
					    value_type_t<It>,
					    difference_type_t<It>,
					    pointer_t<It>,
					    reference_t<It>>
    {
    private:
	It begin, current, end;

    public:
	using iterator_category = typename iterator<iterator_category_t<It>,value_type_t<It>,difference_type_t<It>,pointer_t<It>,iterator_category_t<It>>::iterator_category;
	using value_type = typename iterator<iterator_category_t<It>,value_type_t<It>,difference_type_t<It>,pointer_t<It>,value_type_t<It>>::value_type;
	using difference_type = typename iterator<iterator_category_t<It>,value_type_t<It>,difference_type_t<It>,pointer_t<It>,difference_type_t<It>>::difference_type;
	using pointer = typename iterator<iterator_category_t<It>,value_type_t<It>,difference_type_t<It>,pointer_t<It>,pointer_t<It>>::pointer;
	using reference = typename iterator<iterator_category_t<It>,value_type_t<It>,difference_type_t<It>,pointer_t<It>,reference_t<It>>::reference;

	static_vector_iterator() : begin{nullptr}, current{begin}, end{begin} { }

	// implicitly convert iterator to const_iterator
	operator static_vector_iterator<remove_pointer_t<It> const *>() const
	{ return static_vector_iterator<remove_pointer_t<It> const *>{begin,current,end}; }

	explicit static_vector_iterator(It _begin, It _end)
	    : begin{begin}, current{_begin}, end{_end} {}

	explicit static_vector_iterator(It _begin, It _current, It _end)
	    : begin{_begin}, current{_current}, end{_end} {}

	auto get() { return current; }

	reference operator*() const {
	    // TODO: bounds checking
	    return *current;
	}

	pointer operator->() const { return &(operator*()); }

	static_vector_iterator& operator++() {
	    ++current;
	    return *this;
	}

	static_vector_iterator operator++(int) {
	    static_vector_iterator tmp = *this;
	    ++current;
	    return tmp;
	}

	static_vector_iterator& operator--() {
	    --current;
	    return *this;
	}

	static_vector_iterator operator--(int) {
	    static_vector_iterator tmp = *this;
	    --current;
	    return tmp;
	}

	static_vector_iterator operator+(difference_type n) const { return static_vector_iterator{begin, current + n, end}; }

	static_vector_iterator& operator+=(difference_type n) {
	    current += n;
	    return *this;
	}

	static_vector_iterator operator-(difference_type n) const { return static_vector_iterator{begin, current - n, end}; }

	static_vector_iterator& operator-=(difference_type n) {
	    current -= n;
	    return *this;
	}

	reference operator[](difference_type n) const { return *(*this + n); }

	It base() const {return current;}
    };

    template<typename ItL, typename ItR>
    inline bool
    operator==(const static_vector_iterator<ItL>& x,
	       const static_vector_iterator<ItR>& y)
    { return x.base() == y.base(); }

    template<typename ItL, typename ItR>
    inline bool
    operator<(const static_vector_iterator<ItL>& x,
	      const static_vector_iterator<ItR>& y)
    { return x.base() < y.base(); }

    template<typename ItL, typename ItR>
    inline bool
    operator!=(const static_vector_iterator<ItL>& x,
	       const static_vector_iterator<ItR>& y)
    { return !(x == y); }

    template<typename ItL, typename ItR>
    inline bool
    operator>(const static_vector_iterator<ItL>& x,
	      const static_vector_iterator<ItR>& y)
    { return y < x; }

    template<typename ItL, typename ItR>
    inline bool
    operator<=(const static_vector_iterator<ItL>& x,
	       const static_vector_iterator<ItR>& y)
    { return !(x > y); }

    template<typename ItL, typename ItR>
    inline bool
    operator>=(const static_vector_iterator<ItL>& x,
	       const static_vector_iterator<ItR>& y)
    { return !(x < y); }

    template<typename ItL, typename ItR>
    inline auto
    operator-(const static_vector_iterator<ItL>& x,
	      const static_vector_iterator<ItR>& y)
    { return x.base() - y.base(); }

}

template<typename T, size_t N>
class static_vector {
public:
    using size_type		= size_t;
    using value_type		= T;
    using reference		= value_type&;
    using const_reference	= value_type const&;
    using iterator		= __detail::static_vector_iterator<value_type      *>;
    using const_iterator	= __detail::static_vector_iterator<value_type const*>;
private:
    T m_array[N]; // default constructible
    uint32_t m_size;

    using pointer = pointer_t<iterator>;
    using const_pointer = const_pointer_t<iterator>;
    iterator _create_iterator(pointer p) { return iterator{m_array, p, m_array + m_size}; }
    const_iterator _create_const_iterator(const_pointer p) const { return const_iterator{m_array, p, m_array + m_size}; }
public:

    // NOTE: value_type must be default constructible. Otherwise, we are going to have to use uninitialized storage and uninitialized_copy/move
    static_vector() : m_array{}, m_size{0} {}

    iterator begin() {
	return iterator{m_array, m_array+m_size};
    }
    reverse_iterator<iterator> rbegin() {
	return make_reverse_iterator(end());
    }
    reverse_iterator<iterator> rbegin() const {
	return make_reverse_iterator(end());
    }
    const_iterator begin() const {
	return const_iterator{m_array, m_array+m_size};
    }
    const_iterator cbegin() const {
	return const_iterator{m_array, m_array+m_size};
    }

    iterator end() { return _create_iterator(m_array+m_size); } // TODO: adapt as sentinel
    reverse_iterator<iterator> rend() {
	return make_reverse_iterator(begin());
    }
    reverse_iterator<iterator> rend() const {
	return make_reverse_iterator(begin());
    }
    const_iterator end() const { return _create_const_iterator(m_array+m_size); }
    const_iterator cend() const { return _create_const_iterator(m_array+m_size); }

    reference front() {
	return *begin();
    }
    const_reference front() const {
	return *begin();
    }

    reference back() {
	return *rbegin();
    }
    const_reference back() const {
	return *rbegin();
    }

    iterator erase(const_iterator first, const_iterator last) {
	if(first == last)
	    if(last == end())
		return end();
	    else
	    {
		++last;
		return _create_iterator( m_array + distance(cbegin(), last) );
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
	    return printf("static_vector: it < begin()\n"),end();
	if(it >= end())
	    return printf("static_vector: it >= end()\n"),end();

	pointer p = const_cast<pointer>(it.get());
	iterator ret = _create_iterator(p+1); // should correctly return end() as required in certain cases

	p->~value_type();
	move(p+1, m_array + m_size, p);
	--m_size;
	return ret;
    }

    bool guarantee_capacity(size_t minimuN) {
	if(N >= minimuN)
	    return true;
	printf("static_vector: capacity overflow!\n");
	return false;
    }

    iterator insert(const_iterator pos, value_type const &value) {
	// insert value before pos. pos may be end()
	if(pos < begin() || pos > end())
	    return end();

	auto needed_capacity = m_size+1;
	if(!guarantee_capacity(needed_capacity)) return end();

	// the new end() is not initialized
	new (m_array + m_size) value_type(); // default constructible assumption.
	// To avoid default constructible assumption we have to do
	// uninitialized_copy_backward, but call the destructors for
	// all elements except "new end()".

	// move pos to pos+1 (make space).
	move_backward(pos, cend(), _create_iterator(m_array + m_size + 1));

	auto p = const_cast<pointer>(pos.get()); // no need to subtract: "before pos" is now "pos"
	*p = value;

	++m_size;

	return _create_iterator( p );
    }
    iterator insert(const_iterator pos, value_type &&value) {
	// same as insert(const_iterator, value_type const &) but using move
	if(pos < begin() || pos > end())
	    return end();

	auto needed_capacity = m_size+1;
	if(!guarantee_capacity(needed_capacity)) return end();

	new (m_array + m_size) value_type();

	move_backward(pos, const_iterator{end()}, _create_iterator(m_array + m_size + 1));

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

	if(n == 0) return begin() + dist;

	while(n --> 0)
	    insert(pos, value);

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

	if(first == last) return begin() + dist; //pos; cannot return pos because it is const_iterator, but we return iterator

	while(first != last)
	    insert(pos, *first++); // TODO: can be done more efficiently (insert by bulk)

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
	if(!guarantee_capacity(m_size+1)) return;
	auto p = m_array + m_size; // uninitialized
	new (p) value_type(v); // copy constrictuble
	++m_size;
    }
    void push_back(value_type &&v) {
	if(!guarantee_capacity(m_size+1)) return;
	auto p = m_array + m_size;
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
