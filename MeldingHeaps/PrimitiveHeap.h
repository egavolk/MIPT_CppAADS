#pragma once
#include <vector>
#include <algorithm>

bool ReverseComparator(int a, int b)
{
	return a > b;
}

class PrimitiveHeap
{
public:
	typedef PrimitiveHeap MyHeap;

	PrimitiveHeap()
	{

	}

	PrimitiveHeap(int key)
	{
		a_.push_back(key);
	}

	PrimitiveHeap(PrimitiveHeap &other)
	{
		a_.resize(other.a_.size());
		std::copy(other.a_.begin(), other.a_.end(), a_.begin());
	}

	void Insert(int key)
	{
		a_.insert(std::lower_bound(a_.begin(), a_.end(), key, ReverseComparator), key);
	}

	void AddHeap(int key)
	{
		this->Insert(key);
	}

	int GetMin() const
	{
		return a_.back();
	}

	void ExtractMin()
	{
		a_.pop_back();
	}

	void Meld(MyHeap *other)
	{
		int n = a_.size(), m = other->a_.size();
		a_.insert(a_.end(), other->a_.begin(), other->a_.end());
		std::inplace_merge(a_.rbegin(), a_.rbegin() + m, a_.rend());
		other->a_.clear();
	}

	size_t GetSize() const
	{
		return a_.size();
	}

private:
	std::vector < int > a_;
};
