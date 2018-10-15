#pragma once
//https://github.com/egavolk/deque
#include <iterator>
#include <algorithm>
#include <cstring>

template < class MyDeque, class MyDequePtr >
class Iterators
	: public std::iterator < std::random_access_iterator_tag,
		typename MyDeque::ValueType, size_t >
{
public:
	typedef typename MyDeque::ValueType T;
	typedef Iterators < MyDeque, MyDequePtr > MyIter;

	Iterators()
		: d_()
		, index_(0)
	{

	}

	Iterators(const Iterators& other)
		: d_(other.d_)
		, index_(other.index_)
	{

	}

	Iterators(size_t index_, MyDequePtr x)
		: d_(x)
		, index_(index_)
	{

	}

	T& operator*() const
	{
		return (*d_)[index_];
	}

	T* operator->() const
	{
		return &((*d_)[index_]);
	}

	MyIter& operator++()
	{
		++index_;
		return (*this);
	}

	MyIter operator++(int)
	{
		MyIter tmp = *this;
		++*this;
		return tmp;
	}

	MyIter& operator--()
	{
		--index_;
		return (*this);
	}

	MyIter operator--(int)
	{
		MyIter tmp = *this;
		--*this;
		return tmp;
	}

	MyIter& operator+=(size_t x)
	{
		index_ += x;
		return (*this);
	}

	MyIter operator+(size_t x) const
	{
		MyIter tmp = *this;
		return tmp += x;
	}

	MyIter& operator-=(size_t x)
	{
		index_ -= x;
		return (*this);
	}

	MyIter operator-(size_t x) const
	{
		MyIter tmp = *this;
		return tmp -= x;
	}

	int operator-(const MyIter& other) const
	{
		return (int)index_ - other.index_;
	}

	T& operator[](size_t x) const
	{
		return (*(*this + x));
	}

	bool operator==(const MyIter& other) const
	{
		return (index_ == other.index_) && (d_ == other.d_);
	}

	bool operator!=(const MyIter& other) const
	{
		return !(*this == other);
	}

	bool operator<(const MyIter& other) const
	{
		return index_ < other.index_;
	}

	bool operator>(const MyIter& other) const
	{
		return index_ > other.index_;
	}

	bool operator<=(const MyIter& other) const
	{
		return index_ <= other.index_;
	}

	bool operator>=(const MyIter& other) const
	{
		return index_ >= other.index_;
	}

private:
	MyDequePtr d_;
	size_t index_;
};

template < class MyDeque, class MyDequePtr >
	Iterators < MyDeque, MyDequePtr > operator+(
		size_t x, Iterators < MyDeque, MyDequePtr > it)
{
	return it += x;
}

template < class T >
class Deque
{
public:
	typedef Iterators < Deque < T >, const Deque < T >* > const_iterator;
	typedef Iterators < Deque < T >, Deque < T >* > iterator;
	typedef std::reverse_iterator < iterator > reverse_iterator;
	typedef std::reverse_iterator < const_iterator > const_reverse_iterator;
	typedef T ValueType;

	explicit Deque(size_t n = 0, T x = T())
	{
		Build_(back_stack_, back_size_, back_index_, n / 2, x);
		Build_(front_stack_, front_size_, front_index_, n - n / 2, x);
	}

	explicit Deque(const Deque &other)
		: front_size_(other.front_size_)
		, back_size_(other.back_size_)
		, front_index_(other.front_index_)
		, back_index_(other.back_index_)
	{
		BuildArrayAndCopy_(other.back_stack_, back_stack_, back_index_, other.back_size_);
		BuildArrayAndCopy_(other.front_stack_, front_stack_, front_index_, other.front_size_);
	}

	~Deque()
	{
		delete[] back_stack_;
		delete[] front_stack_;
	}

	Deque &operator=(const Deque &other)
	{
		front_size_ = other.front_size_;
		back_size_ = other.back_size_;
		front_index_ = other.front_index_;
		back_index_ = other.back_index_;
		ResizeAndCopy_(other.back_stack_, back_stack_, back_index_, back_size_);
		ResizeAndCopy_(other.front_stack_, front_stack_, front_index_, front_size_);
		return *this;
	}

	size_t size() const
	{
		return back_index_ + front_index_;
	}

	bool empty() const
	{
		return (!(this->size()));
	}

	T back() const
	{
		return FrontOrBackElement_(back_stack_, front_stack_, back_index_);
	}

	T back()
	{
		return FrontOrBackElement_(back_stack_, front_stack_, back_index_);
	}

	T front() const
	{
		return FrontOrBackElement_(front_stack_, back_stack_, front_index_);
	}

	T front()
	{
		return FrontOrBackElement_(front_stack_, back_stack_, front_index_);
	}

	T& operator[](const size_t& index) const
	{
		return this->IthElement_(index);
	}

	T& operator[](const size_t& index)
	{
		return this->IthElement_(index);
	}

	void push_back(const T& x)
	{
		Insert_(back_stack_, back_index_, back_size_, x);
	}

	void push_front(const T& x)
	{
		Insert_(front_stack_, front_index_, front_size_, x);
	}

	void pop_back()
	{
		Remove_(back_stack_, front_stack_, back_size_, front_size_, back_index_, front_index_);
	}

	void pop_front()
	{
		Remove_(front_stack_, back_stack_, front_size_, back_size_, front_index_, back_index_);
	}

	iterator begin()
	{
		return iterator(0, this);
	}

	const_iterator begin() const
	{
		return const_iterator(0, this);
	}

	const_iterator cbegin() const
	{
		return begin();
	}

	reverse_iterator rbegin()
	{
		return reverse_iterator(end());
	}

	const_reverse_iterator rbegin() const
	{
		return const_reverse_iterator(end());
	}

	const_reverse_iterator crbegin() const
	{
		return rbegin();
	}

	iterator end()
	{
		return iterator(this->size(), this);
	}

	const_iterator end() const
	{
		return const_iterator(this->size(), this);
	}

	const_iterator cend() const
	{
		return end();
	}

	reverse_iterator rend()
	{
		return reverse_iterator(begin());
	}

	const_reverse_iterator rend() const
	{
		return const_reverse_iterator(begin());
	}

	const_reverse_iterator crend() const
	{
		return rend();
	}

	size_t GetFrontSize() const
	{
		return front_size_;
	}

	size_t GetBackSize() const
	{
		return back_size_;
	}

	size_t GetFrontIndex() const
	{
		return front_index_;
	}

	size_t GetBackIndex() const
	{
		return back_index_;
	}

	size_t GetMinSize() const
	{
		return MIN_SIZE_;
	}

private:
	void ResizeAndCopy_(const T* from, T* &to, size_t index, size_t size)
	{
		delete to;
		BuildArrayAndCopy_(from, to, size, size);
	}

	void FindSize_(size_t& size, size_t index)
	{
		size = MIN_SIZE_;
		while (size <= index)
			size <<= 1;
	}

	void Build_(T*& a, size_t& size, size_t& index, size_t n, const T& x)
	{
		index = n;
		FindSize_(size, index);
		a = new T[size];
		std::fill_n(a, size, x);
	}

	void BuildArrayAndCopy_(const T* from, T*& to, size_t n, size_t size)
	{
		to = new T[size];
		std::copy_n(from, n, to);
	}

	T FrontOrBackElement_(T* a, T* b, size_t a_index) const
	{
		return a_index ? a[a_index - 1] : b[0];
	}

	T& IthElement_(size_t index) const
	{
		if (index < front_index_)
			return front_stack_[front_index_ - 1 - index];
		else
			return back_stack_[index - front_index_];
	}

	void Resize_(T*& a, size_t index, size_t size)
	{
		T* cur;
		BuildArrayAndCopy_(a, cur, index, size);
		delete[] a;
		a = cur;
	}

	void Insert_(T*& a, size_t& index, size_t& size, const T& x)
	{
		if (index >= size)
		{
			size <<= 1;
			Resize_(a, index, size);
		}
		a[index++] = x;
	}

	void RemoveOtherStack_(T*& a, T*& b, size_t& a_size, size_t& b_size, size_t& a_index, size_t& b_index)
	{
		size_t cur_size = b_index - 1;
		size_t k1 = cur_size - cur_size / 2;
		delete[] a;
		FindSize_(a_size, k1);
		a = new T[a_size];
		std::reverse_copy(b + 1, b + k1 + 1, a);
		a_index = k1;
		size_t k2 = cur_size / 2;
		std::copy(b + k1 + 1, b + k1 + k2 + 1, b);
		b_index = k2;
		Resize_(b, b_index, b_size);
	}

	void Remove_(T*& a, T*& b, size_t& a_size, size_t& b_size, size_t& a_index, size_t& b_index)
	{
		if (a_index)
		{
			--a_index;
			if ((a_size > MIN_SIZE_) && (4 * a_index < a_size))
			{
				a_size >>= 1;
				Resize_(a, a_index, a_size);
			}
		}
		else
		{
			RemoveOtherStack_(a, b, a_size, b_size, a_index, b_index);
		}
	}

	static const size_t MIN_SIZE_ = 1 << 7;
	size_t front_size_;
	size_t back_size_;
	size_t front_index_;
	size_t back_index_;
	T* front_stack_;
	T* back_stack_;
};
