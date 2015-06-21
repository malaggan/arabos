template<typename T>
class vector {
    T *a;
public:
    typedef size_t size_type;
    typedef T value_type;
    typedef T& reference;
    typedef T const& const_reference;
    typedef T* iterator;
    typedef T const* const_iterator;
    reference front(); // equivalent to *begin() // TODO: implement
    const_reference front() const; // TODO: implement
    iterator begin(); // TODO: implement
    const_iterator begin() const; // TODO: implement
    iterator end(); // TODO: implement
    const_iterator end() const; // TODO: implement
    iterator erase(const_iterator, const_iterator); // TODO: implement
    iterator erase(const_iterator); // TODO: implement
    bool empty() const noexcept; // TODO: implement
    void clear() noexcept; // TODO: implement
    iterator insert(const_iterator, value_type const &); // TODO: implement
    iterator insert(const_iterator, value_type &&); // TODO: implement
    iterator insert(const_iterator, size_type, value_type const &); // TODO: implement
    template<typename Iterator>
    iterator insert(const_iterator, Iterator, Iterator); // TODO: implement
    size_type size() const noexcept; // TODO: implement
    void push_back(value_type const &); // TODO: implement
    void push_back(value_type &&); // TODO: implement
    reference at(size_type); // TODO: implement
    const_reference at(size_type) const; // TODO: implement
    reference operator[](size_type); // TODO: implement
    const_reference operator[](size_type) const; // TODO: implement
} __attribute__((packed));
