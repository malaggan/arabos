template<typename T>
class shared_ptr
{
    T* m_ptr;
    uint_fast32_t *use_count;
public:
    using element_type = T;
    
    shared_ptr() : m_ptr{nullptr}, use_count{new uint_fast32_t(0)} {}
    explicit shared_ptr(T* p) : m_ptr{p}, use_count{new uint_fast32_t(1)} {}

    shared_ptr(shared_ptr const &sp) : m_ptr(sp.m_ptr), use_count(sp.use_count) { ++(*use_count); }
    shared_ptr(shared_ptr &&sp) : m_ptr(sp.m_ptr), use_count(sp.use_count) { sp.m_ptr = nullptr; }

    shared_ptr& operator=(shared_ptr const &sp) {
	m_ptr = sp.m_ptr;
	use_count = sp.use_count;
	++(*use_count);
	return *this;
    }
    shared_ptr& operator=(shared_ptr &&sp) {
	m_ptr = sp.m_ptr;
	use_count = sp.use_count;
	sp.m_ptr = nullptr;
	return *this;
    }
   
    ~shared_ptr() {
	// TODO: atomic? (get_and_decrement)
	--(*use_count);	    
	if(!*use_count && m_ptr != nullptr)
	{
	    // TODO: delete use_count;
	    delete m_ptr;
	}
	
    }
};

template<typename T>
class shared_array
{
    T* m_array;
    uint_fast32_t *use_count;
public:
    using element_type = T;
    
    shared_array() : m_array{nullptr}, use_count{new uint_fast32_t(0)} {}
    explicit shared_array(T* p) : m_array{p}, use_count{new uint_fast32_t(1)} {}

    shared_array(shared_array const &sp) : m_array(sp.m_array), use_count(sp.use_count) { ++(*use_count); }
    shared_array(shared_array &&sp) : m_array(sp.m_array), use_count(sp.use_count) { sp.m_array = nullptr; }

    shared_array& operator=(shared_array const &sp) {
	m_array = sp.m_array;
	use_count = sp.use_count;
	++(*use_count);
	return *this;
    }
    shared_array& operator=(shared_array &&sp) {
	m_array = sp.m_array;
	use_count = sp.use_count;
	sp.m_array = nullptr;
	return *this;
    }

    element_type* get() { return m_array; }
    element_type const * get() const { return m_array; }
   
    ~shared_array() {
	// TODO: atomic? (get_and_decrement)
	--(*use_count);	    
	if(!*use_count && m_array != nullptr)
	{
	    // TODO: delete use_count;
	    delete[] m_array;
	}
    }
};

