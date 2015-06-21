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
