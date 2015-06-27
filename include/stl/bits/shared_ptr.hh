template<typename T>
class shared_ptr
{
	T* m_ptr;
	uint_fast32_t *use_count;
public:
	using element_type = T;

	shared_ptr() : m_ptr{nullptr}, use_count{nullptr} {}
	explicit shared_ptr(T* p) : m_ptr{p}, use_count{new uint_fast32_t(1)} {}

	shared_ptr(shared_ptr const &sp) : m_ptr(sp.m_ptr), use_count(sp.use_count) { atomic_inc(use_count); }
	shared_ptr(shared_ptr &&sp) : m_ptr(sp.m_ptr), use_count(sp.use_count) { sp.m_ptr = nullptr; sp.use_count = nullptr; // the moved sp should not participate in incrementing or decrementing the use count
	}

	shared_ptr& operator=(shared_ptr const &sp) {
		m_ptr = sp.m_ptr;
		use_count = sp.use_count;
		atomic_inc(use_count);
		return *this;
	}
	shared_ptr& operator=(shared_ptr &&sp) {
		m_ptr = sp.m_ptr;
		use_count = sp.use_count;
		sp.m_ptr = nullptr;
		sp.use_count = nullptr; // the moved sp should not participate in incrementing or decrementing the use count
		return *this;
	}

	~shared_ptr() {
		if(!use_count) return;
		atomic_dec(use_count);
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

	shared_array() : m_array{nullptr}, use_count{nullptr} {}
	explicit shared_array(T* p) : m_array{p}, use_count{new uint_fast32_t(1)} {}

	shared_array(shared_array const &sp) : m_array(sp.m_array), use_count(sp.use_count) { atomic_inc(use_count); }
	shared_array(shared_array &&sp) : m_array(sp.m_array), use_count(sp.use_count) { sp.m_array = nullptr; sp.use_count = nullptr; // the moved sp should not participate in incrementing or decrementing the use count
	}

	shared_array& operator=(shared_array const &sp) {
		m_array = sp.m_array;
		use_count = sp.use_count;
		atomic_inc(use_count);
		return *this;
	}
	shared_array& operator=(shared_array &&sp) {
		m_array = sp.m_array;
		use_count = sp.use_count;
		sp.m_array = nullptr;
		sp.use_count = nullptr; // the moved sp should not participate in incrementing or decrementing the use count
		return *this;
	}

	void check() const {
		if(!use_count || !m_array)
			panic("shared_array: !use_count || !m_array\n");
	}

	element_type* get() {
		check();
		return m_array;
	}
	element_type const * get() const {
		check();
		return m_array;
	}

	~shared_array() {
		if(!use_count) {
			// printf("~shared_array use_count(ptr) = nullptr\n");
			return;
		}
		// printf("~shared_array (use_count@before dec = %d), use_count(ptr) = %x\n", *use_count, use_count);
		atomic_dec(use_count);
		// printf("~shared_array (use_count@after dec = %d), m_array = %x\n", *use_count, m_array);
		if(!*use_count && m_array != nullptr)
		{
			// printf("~shared_array: deleting\n");
			// TODO: delete use_count;
			delete[] m_array;
		}
	}
};
