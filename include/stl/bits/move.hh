template<class InputIt, class OutputIt>
OutputIt move(InputIt first, InputIt last,
              OutputIt d_first)
{
	while (first != last) {
		*d_first++ = move(*first++);
	}
	return d_first;
}
