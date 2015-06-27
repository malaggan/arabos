// N3844 (for explanation), and N3923 for most recent wording
template<typename...> using void_t = void;

template<typename It, typename = void_t<>>
	struct __iterator_traits { };

template<typename It>
struct __iterator_traits<It, void_t<typename It::iterator_category,
                                    typename It::value_type,
                                    typename It::difference_type,
                                    typename It::pointer,
                                    typename It::reference>>
{
	using iterator_category = typename It::iterator_category;
	using value_type        = typename It::value_type;
	using difference_type   = typename It::difference_type;
	using pointer           = typename It::pointer;
	using const_pointer     = remove_pointer_t<pointer> const *; // non-standard
	using reference         = typename It::reference;
};

template<typename It>
struct iterator_traits : public __iterator_traits<It> { };


template<typename T>
struct iterator_traits<T*>
{
	using iterator_category = random_access_iterator_tag;
	using value_type        = T;
	using difference_type   = ptrdiff_t;
	using pointer           = T*;
	using const_pointer     = T const *; // non-standard
	using reference         = T&;
};


template<typename T>
struct iterator_traits<T const *>
{
	using iterator_category = random_access_iterator_tag;
	using value_type        = T;
	using difference_type   = ptrdiff_t;
	using pointer           = T const *;
	using const_pointer     = T const *; // non-standard
	using reference         = T const &;
};

// not std c++, but helpful:
template <typename It> using iterator_category_t	= typename iterator_traits<It>::iterator_category;
template <typename It> using value_type_t		= typename iterator_traits<It>::value_type;
template <typename It> using difference_type_t		= typename iterator_traits<It>::difference_type;
template <typename It> using pointer_t			= typename iterator_traits<It>::pointer;
template <typename It> using const_pointer_t		= typename iterator_traits<It>::const_pointer;
template <typename It> using reference_t		= typename iterator_traits<It>::reference;
