template<class InputIt>
difference_type_t<InputIt>
distance(InputIt first, InputIt last) {
    difference_type_t<InputIt> dist = 0;
    while(first++ != last) dist++;
    return dist;
}

// TODO: distance can be made more efficient for random_access_iterators
// TODO: advance()
