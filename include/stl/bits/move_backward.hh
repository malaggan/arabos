template<class BidirIt1, class BidirIt2>
BidirIt2 move_backward(BidirIt1 first, BidirIt1 last, BidirIt2 d_last)
{
	while (first != last) {
		*(--d_last) = move(*(--last));
	}
	return d_last;
}
