template<class BidirIt1, class BidirIt2>
BidirIt2 copy_backward(BidirIt1 first, BidirIt1 last, BidirIt2 d_last)
{
	while (first != last) {
		*(--d_last) = *(--last);
	}
	return d_last;
}
