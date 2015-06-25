template<class C> auto begin			(C &c)		{ return c.begin(); }
template<class C> auto end				(C &c)		{ return c.end(); }
template<class C> auto begin			(C const &c)	{ return c.begin(); }
template<class C> auto end				(C const &c)	{ return c.end(); }
template<class T, size_t N> constexpr T* begin	(T (&array)[N])	{ return array; }
template<class T, size_t N> constexpr T* end	(T (&array)[N])	{ return array+N; }
template<class C> auto cbegin			(C const &c)	{ return c.begin(); }
template<class C> auto cend				(C const &c)	{ return c.end(); }
