#pragma once
#include <iostream>

void swap(int &first, int &second)
{
	int cur = first;
	first = second;
	second = cur;
}

class Array
{
private:
	int *a;
	unsigned a_size;

public:
	Array(const Array &other)
		: a_size(other.a_size)
	{
		a = new int[other.a_size];
		for (unsigned i = 0; i < other.a_size; ++i)
			a[i] = other.a[i];
	}

	Array(unsigned _n = 0)
	{
		int *cur = new int[_n];
		a = cur;
		a_size = _n;
	}

	~Array()
	{
		delete[]a;
		a_size = 0;
	}

	int &operator[](int k) const&
	{
		return *(a + k);
	}

	Array &operator=(const Array &other)
	{
		if (this == &other) {
			return *this;
		}
		if (this->a_size != other.a_size)
		{
			delete[] a;
			a_size = other.a_size;

			a = new int[a_size];
		}
		for (unsigned i = 0; i < a_size; ++i)
			a[i] = other.a[i];
		return *this;
	}

	unsigned size() const&
	{
		return a_size;
	}
};

class Permutation
{
private:
	Array a;

public:
	Permutation(const Permutation &other)
		: a(other.a)
	{}

	Permutation(const unsigned _n = 0)
	{
		a = Array(_n);
		for (unsigned i = 0; i < _n; ++i)
			a[i] = i;
	}

	Permutation(const unsigned &_n, const int *_a)
	{
		a = Array(_n);
		for (unsigned i = 0; i < _n; ++i)
			a[i] = _a[i];
	}

	~Permutation()
	{

	}

	Permutation &operator=(const Permutation &other)
	{
		if (this == &other) 
		{
			return *this;
		}
		//a = Array(other.a.size());
		a = other.a;
		return *this;
	}

	int operator[](const unsigned k) const
	{
		return a[k];
	}

	void operator()(int *other) const
	{
		Array c = Array(a.size());
		for (unsigned i = 0; i < a.size(); ++i)
		{
			c[i] = other[i];
		}
		for (unsigned i = 0; i < a.size(); ++i)
		{
			other[a[i]] = c[i];
		}
	}

	bool operator==(const Permutation &other) const&
	{
		if (this == &other)
		{
			return true;
		}
		if (a.size() != other.a.size())
			return false;
		for (unsigned i = 0; i < a.size(); ++i)
			if (a[i] != other.a[i])
				return false;
		return true;
	}

	bool operator<(const Permutation &other) const&
	{
		unsigned n;
		if (a.size() < other.size())
			n = a.size();
		else
			n = other.size();
		for (unsigned i = 0; i < n; ++i)
			if (a[i] < other[i])
				return true;
			else
				if (a[i] > other[i])
					return false;
		return a.size() < other.size();
	}

	bool operator!=(const Permutation &other) const&
	{
		return !(*this == other);
	}

	bool operator<=(const Permutation &other) const&
	{
		return (*this == other) || (*this < other);
	}

	bool operator>(const Permutation &other) const&
	{
		return (other < *this);
	}

	bool operator>=(const Permutation &other) const&
	{
		return (*this == other) || (other < *this);
	}

	void print() const
	{
		for (unsigned i = 0; i < a.size(); ++i)
			printf("%d ", a[i]);
		printf("\n");
	}

	unsigned size() const&
	{
		return a.size();
	}

	const Permutation previous() const
	{
		Permutation cur;
		cur = *this;
		int m_l = -1, m_r = -1;
		for (int i = (int)cur.a.size() - 2; i >= 0; --i)
		{
			if (cur.a[i] > cur.a[i + 1])
			{
				m_l = i;
				m_r = i + 1;
				for (int j = i + 1; j < (int)a.size(); ++j)
					if ((cur.a[m_r] < cur.a[j]) && (cur.a[j] < cur.a[m_l]))
						m_r = j;
				break;
			}
		}
		if (m_l != -1)
		{
			swap(cur.a[m_l], cur.a[m_r]);
		}
		m_l++;
		for (int i = m_l; i < m_l + ((int)cur.a.size() - m_l) / 2; ++i)
			swap(cur.a[i], cur.a[cur.a.size() + m_l - i - 1]);
		return cur;
	}

	const Permutation next() const
	{
		Permutation cur = *this;
		int m_l = -1, m_r = -1;
		for (int i = (int)cur.a.size() - 2; i >= 0; --i)
		{
			if (cur.a[i] < cur.a[i + 1])
			{
				m_l = i;
				m_r = i + 1;
				for (int j = i + 1; j < (int)cur.a.size(); ++j)
					if ((cur.a[m_r] > cur.a[j]) && (cur.a[j] > cur.a[m_l]))
						m_r = j;
				break;
			}
		}
		if (m_l != -1)
		{
			swap(cur.a[m_l], cur.a[m_r]);
		}
		m_l++;
		for (int i = m_l; i < m_l + ((int)cur.a.size() - m_l) / 2; ++i)
			swap(cur.a[i], cur.a[cur.a.size() + m_l - i - 1]);
		return cur;
	}

	const Permutation inverse() const
	{
		Permutation cur = Permutation(a.size());
		for (unsigned i = 0; i < a.size(); ++i)
		{
			cur.a[a[i]] = i;
		}
		return cur;
	}

	Permutation &operator*=(const Permutation &other)
	{
		Permutation c = Permutation(a.size());
		for (unsigned i = 0; i < a.size(); ++i)
		{
			c.a[i] = a[other.a[i]];
		}
		*this = c;
		return *this;
	}

	const Permutation operator*(const Permutation &other) const&
	{
		Permutation c = *this;
		c *= other;
		return c;
	}

	Permutation &operator++() 
	{
		*this = this->next();
		return *this;
	}

	Permutation &operator--()
	{
		*this = this->previous();
		return *this;
	}

	Permutation operator++(int)
	{
		Permutation old = *this;
		*this = this->next();
		return old;
	}

	Permutation operator--(int)
	{
		Permutation old = *this;
		*this = this->previous();
		return old;
	}
};