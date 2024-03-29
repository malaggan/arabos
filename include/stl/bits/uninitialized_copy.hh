template<class InputIt, class ForwardIt>
ForwardIt uninitialized_copy(InputIt first, InputIt last, ForwardIt d_first)
{
	using Value = value_type_t<ForwardIt>;
	ForwardIt current = d_first;

	for (; first != last; ++first, ++current) {
		new (&*current) Value(*first);
	}
	return current;

}
