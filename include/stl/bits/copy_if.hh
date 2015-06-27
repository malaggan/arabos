template<class InputIt, class OutputIt, class UnaryPredicate>
OutputIt copy_if(InputIt first, InputIt last,
		 OutputIt d_first, UnaryPredicate pred)
{
    while (first != last) {
	if (pred(*first))
	    *d_first++ = *first;
	first++;
    }
    return d_first;
}
