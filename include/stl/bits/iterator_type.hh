struct input_iterator_tag { };
struct output_iterator_tag { };
struct forward_iterator_tag : public input_iterator_tag { };
struct bidirectional_iterator_tag : public forward_iterator_tag { };
struct random_access_iterator_tag : public bidirectional_iterator_tag { };

template<typename Category, typename T, typename Distance = ptrdiff_t, typename Pointer = T*, typename Reference = T&>
class iterator
{
public:
	using  iterator_category	= Category      ;
	using  value_type		= T             ;
	using  difference_type	= Distance      ;
	using  pointer		= Pointer       ;
	using  reference		= Reference	;
};
