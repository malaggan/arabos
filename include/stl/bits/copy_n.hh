template< class InputIt, class Size, class OutputIt>
OutputIt copy_n(InputIt first, Size count, OutputIt result)
{
    if (count > 0) {
	*result++ = *first;
	for (Size i = 1; i < count; ++i) {
	    *result++ = *++first;
	}
    }
    return result;
}
