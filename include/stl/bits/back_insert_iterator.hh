// back_insert_iterator is an OutputIterator that appends to a container for which it was constructed. The container's push_back() member function is called whenever the iterator (whether dereferenced or not) is assigned to. Incrementing the back_insert_iterator is a no-op.
template<class Container>
class back_insert_iterator : public iterator<output_iterator_tag, void, void, void, void>
{
	Container &member;
public:
	explicit back_insert_iterator(Container &c) : member(c) {}
	back_insert_iterator<Container>& operator*() { return *this; } // no-op
	back_insert_iterator<Container>& operator++() { return *this; } // no-op
	back_insert_iterator<Container>  operator++(int) { return *this; } // no-op
	back_insert_iterator<Container>&
	operator=(const value_type_t<typename Container::iterator> &value) { member.push_back(value); return *this; }
	back_insert_iterator<Container>&
	operator=(value_type_t<typename Container::iterator> &&value ) { member.push_back(move(value)); return *this; }
};

template<class Container> back_insert_iterator<Container> back_inserter(Container &c)
{
	return back_insert_iterator<Container>(c);
}
