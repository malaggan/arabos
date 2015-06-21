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
