namespace __detail {
	template <typename It>
	class string_iterator : public iterator<iterator_category_t<It>,
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

		string_iterator() : begin{nullptr}, current{begin}, end{begin} { }

		// implicitly convert iterator to const_iterator
		operator string_iterator<remove_pointer_t<It> const *>() const
		{ return string_iterator<remove_pointer_t<It> const *>{begin,current,end}; }

		explicit string_iterator(It _begin, It _end)
			: begin{begin}, current{_begin}, end{_end} {}

		explicit string_iterator(It _begin, It _current, It _end)
			: begin{_begin}, current{_current}, end{_end} {}

		auto get() { return current; }

		reference operator*() const {
			// TODO: bounds checking
			return *current;
		}

		pointer operator->() const { return &(operator*()); }

		string_iterator& operator++() {
			++current;
			return *this;
		}

		string_iterator operator++(int) {
			string_iterator tmp = *this;
			++current;
			return tmp;
		}

		string_iterator& operator--() {
			--current;
			return *this;
		}

		string_iterator operator--(int) {
			string_iterator tmp = *this;
			--current;
			return tmp;
		}

		string_iterator operator+(difference_type n) const { return string_iterator{begin, current + n, end}; }

		string_iterator& operator+=(difference_type n) {
			current += n;
			return *this;
		}

		string_iterator operator-(difference_type n) const { return string_iterator{begin, current - n, end}; }

		string_iterator& operator-=(difference_type n) {
			current -= n;
			return *this;
		}

		reference operator[](difference_type n) const { return *(*this + n); }

		It base() const {return current;}
	};

	template<typename ItL, typename ItR>
	inline bool
	operator==(const string_iterator<ItL>& x,
	           const string_iterator<ItR>& y)
	{ return x.base() == y.base(); }

	template<typename ItL, typename ItR>
	inline bool
	operator<(const string_iterator<ItL>& x,
	          const string_iterator<ItR>& y)
	{ return x.base() < y.base(); }

	template<typename ItL, typename ItR>
	inline bool
	operator!=(const string_iterator<ItL>& x,
	           const string_iterator<ItR>& y)
	{ return !(x == y); }

	template<typename ItL, typename ItR>
	inline bool
	operator>(const string_iterator<ItL>& x,
	          const string_iterator<ItR>& y)
	{ return y < x; }

	template<typename ItL, typename ItR>
	inline bool
	operator<=(const string_iterator<ItL>& x,
	           const string_iterator<ItR>& y)
	{ return !(x > y); }

	template<typename ItL, typename ItR>
	inline bool
	operator>=(const string_iterator<ItL>& x,
	           const string_iterator<ItR>& y)
	{ return !(x < y); }

	template<typename ItL, typename ItR>
	inline auto
	operator-(const string_iterator<ItL>& x,
	          const string_iterator<ItR>& y)
	{ return x.base() - y.base(); }

}

template<size_t N>
class string {
public:
	using size_type		= size_t;
	using value_type		= char;
	using reference		= value_type&;
	using const_reference	= value_type const&;
	using iterator		= __detail::string_iterator<value_type      *>;
	using const_iterator	= __detail::string_iterator<value_type const*>;
private:
	char m_array[N+1]; // NOTE: 0-terminated string. we do not need to force this except on c_str
	uint32_t m_size;

	using pointer = pointer_t<iterator>;
	using const_pointer = const_pointer_t<iterator>;
	iterator _create_iterator(pointer p) { return iterator{m_array, p, m_array + m_size}; }
	const_iterator _create_const_iterator(const_pointer p) const { return const_iterator{m_array, p, m_array + m_size}; }
public:
	static const size_type npos = -1;

	// NOTE: value_type must be default constructible. Otherwise, we are going to have to use uninitialized storage and uninitialized_copy/move
	string() : m_array{0}, m_size{0} {}

	template<size_t NN>
	string(string<NN> const &other, size_t start, size_t end) : m_array{0}, m_size{end-start} {
		if(m_size > NN)
			printf("string: overflow in string::string(string, size_t, size_t)\n");
		copy(other.begin() + start, other.begin() + end, m_array);
		m_array[m_size] = 0;
	}

	iterator begin() {
		return iterator{m_array, m_array+m_size};
	}
	const_iterator begin() const {
		return const_iterator{m_array, m_array+m_size};
	}
	const_iterator cbegin() const {
		return const_iterator{m_array, m_array+m_size};
	}

	iterator end() { return _create_iterator(m_array+m_size); } // TODO: adapt as sentinel
	const_iterator end() const { return _create_const_iterator(m_array+m_size); }
	const_iterator cend() const { return _create_const_iterator(m_array+m_size); }

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
			return printf("string: it < begin()\n"),end();
		if(it >= end())
			return printf("string: it >= end()\n"),end();

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
		printf("string: capacity overflow!\n");
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
		m_array[m_size] = 0; // zero terminated string
		return *(begin() + n);
	}
	const_reference at(size_type n) const {
		// m_array[m_size] = 0; // TODO: zero terminated string, but we can't do it here, cuz const. we'll have to guarantee this invariant throughout all modifying methods
		return *(begin() + n);
	}

	reference operator[](size_type n) { return at(n); }
	const_reference operator[](size_type n) const { return at(n); }

	size_type find(char c, size_type pos = 0) const {
		for(size_t i = 0; i < m_size; i++)
			if(m_array[i] == c)
				return i;
		return npos;
	}

	// string specific:
	char const * c_str() const {
		return m_array;
	}
	/*implicit*/ string(char const*str) {
		size_t i = 0;
		for(; str[i] && i < N + 1; i++) {
			m_array[i] = str[i];
		}
		m_array[i] = 0; // zero terminated string
		m_size = i;
	}
} __attribute__((packed));


// string specific:
template <size_t N1, size_t N2>
bool operator==(const string<N1>& lhs, const string<N2>& rhs) {
	if(lhs.size() != rhs.size())
		return false;
	if(lhs.size() == 0) // both string are empty
		return true;
	for(size_t i = 0; i < lhs.size(); i++) {
		if(lhs[i] != rhs[i])
			return false;
	}
	return true;
}
