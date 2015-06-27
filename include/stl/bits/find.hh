template<class InputIt, class T>
InputIt find(InputIt first, InputIt last, const T& value)
{
	for (; first != last; ++first) {
		if (*first == value) {
			return first;
		}
	}
	return last;
}
