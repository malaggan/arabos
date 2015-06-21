namespace { namespace __detail {
	template <typename T>
	class vector_iterator : public iterator<iterator_category_t<T*>,
						value_type_t<T*>,
						difference_type_t<T*>,
						pointer_t<T*>,
						reference_t<T*>>
	{
	private:
	    T *current;
	    T *begin, *end;;

	public:
	    using iterator_category = typename iterator<iterator_category_t<T*>,value_type_t<T*>,difference_type_t<T*>,pointer_t<T*>,iterator_category_t<T*>>::iterator_category;
	    using value_type = typename iterator<iterator_category_t<T*>,value_type_t<T*>,difference_type_t<T*>,pointer_t<T*>,value_type_t<T*>>::value_type;
	    using difference_type = typename iterator<iterator_category_t<T*>,value_type_t<T*>,difference_type_t<T*>,pointer_t<T*>,difference_type_t<T*>>::difference_type;
	    using pointer = typename iterator<iterator_category_t<T*>,value_type_t<T*>,difference_type_t<T*>,pointer_t<T*>,pointer_t<T*>>::pointer;
	    using reference = typename iterator<iterator_category_t<T*>,value_type_t<T*>,difference_type_t<T*>,pointer_t<T*>,reference_t<T*>>::reference;

	    vector_iterator() : begin{nullptr}, current{begin}, end{begin} { }

	    explicit vector_iterator(T* begin, T* end)
		: begin{begin}, current{begin}, end{end} {}

	    reference operator*() const {
		// TODO: bounds checking
		return *current;
	    }

	    pointer operator->() const { return &(operator*()); }

	    vector_iterator& operator++() {
		++current;
		return *this;
	    }

	    vector_iterator operator++(int) {
		vector_iterator tmp = *this;
		++current;
		return tmp;
	    }

	    vector_iterator& operator--() {
		--current;
		return *this;
	    }

	    vector_iterator operator--(int) {
		vector_iterator tmp = *this;
		--current;
		return tmp;
	    }

	    vector_iterator operator+(difference_type n) const { return vector_iterator(current + n); }

	    vector_iterator& operator+=(difference_type n) {
		current += n;
		return *this;
	    }

	    vector_iterator operator-(difference_type n) const { return vector_iterator(current - n); }

	    vector_iterator& operator-=(difference_type n) {
		current -= n;
		return *this;
	    }

	    reference operator[](difference_type n) const { return *(*this + n); }

	    T* base() const {return current;}
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
	{ return y.base() < x.base(); }

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
	{ return !(y < x); }

	template<typename ItL, typename ItR>
	inline bool
	operator>=(const vector_iterator<ItL>& x,
		   const vector_iterator<ItR>& y)
	{ return !(x < y); }

	template<typename ItL, typename ItR>
	inline auto
	operator-(const vector_iterator<ItL>& x,
		  const vector_iterator<ItR>& y)
	{ return y.base() - x.base(); }

    }
}
	
template<typename T>
class vector {
    shared_array<T> m_array;
    uint32_t m_capacity;
    uint32_t m_size;
public:
    using size_type		= size_t ;
    using value_type		= T;
    using reference		= T& ;
    using const_reference	= T const& ;
    using iterator		= __detail::vector_iterator<value_type>;
    using const_iterator	= __detail::vector_iterator<value_type const*> ;

    vector() : m_array{new T[8]}, m_capacity{8}, m_size{0} {}
    
    iterator begin() {
	return __detail::vector_iterator<value_type>{m_array.get(),m_array.get()+m_size};
    }
    const_iterator begin() const; // TODO: implement

    iterator end(); // TODO: implement
    const_iterator end() const; // TODO: implement

    
    reference front(); // equivalent to *begin() // TODO: implement
    const_reference front() const; // TODO: implement
    
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
