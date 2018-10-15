#pragma once
#include <algorithm>
#include "TemplateHeap.h"

class Node
{
public:
	explicit Node(int key = INT_MAX, int rank = 0)
		: key_(key)
	{

	}

	Node(const Node &other)
		: key_(other.key_)
	{

	}

	int key_;
};

typedef Node NodeSkew;

class NodeLeftist : public Node
{
public:
	explicit NodeLeftist(int key = INT_MAX, int rank = -INT_MAX)
		: Node(key)
		, rank_(rank)
	{

	}

	NodeLeftist(const NodeLeftist &other)
		: Node(other)
		, rank_(other.rank_)
	{

	}

	int rank_;
};

template < class HeapNode >
class LeftistHeapTemplate : public IHeap
{
private:
	typedef LeftistHeapTemplate < HeapNode > MyHeap;
public:
	LeftistHeapTemplate < HeapNode >()
		: left_(nullptr)
		, right_(nullptr)
		, vertex_()
	{
		
	}

	explicit LeftistHeapTemplate < HeapNode >(int key)
		: left_(nullptr)
		, right_(nullptr)
		, vertex_(key)
	{
		left_ = new MyHeap();
		left_->vertex_ = HeapNode(key, 1);
	}

	LeftistHeapTemplate < HeapNode >(const MyHeap& other)
		: left_(other.left_)
		, right_(other.right_)
		, vertex_(vertex_)
	{

	}

	~LeftistHeapTemplate < HeapNode >()
	{
		delete this->left_;
		delete this->right_;
	}

	void Meld(IHeap *ptr)
	{
		MyHeap *other = dynamic_cast < MyHeap* >(ptr);
		left_ = LeftistMeld_(this->left_, other->left_);
		other->left_ = nullptr;
	}

	void Insert(int key)
	{
		MyHeap *cur = new MyHeap(key);
		Meld(cur);
		cur->left_ = nullptr;
		cur->right_ = nullptr;
		delete cur;
	}

	int GetMin() const
	{
		return left_->vertex_.key_;
	}

	void ExtractMin()
	{
		MyHeap* cur = LeftistMeld_(left_->left_, left_->right_);
		left_->left_ = nullptr;
		left_->right_ = nullptr;
		delete left_;
		left_ = cur;
	}

	size_t GetSize() const
	{
		return MyGetSize_() - 1;
	}

private:
	
	size_t MyGetSize_() const
	{
		if (this == nullptr)
			return 0;
		size_t ans = 1;
		if (left_)
			ans += left_->MyGetSize_();
		if (right_)
			ans += right_->MyGetSize_();
		return ans;
	}

	MyHeap* LeftistMeld_(MyHeap *first, MyHeap *second)
	{
		if (first == nullptr)
			return second;
		if (second == nullptr)
			return first;
		if (first->vertex_.key_ > second->vertex_.key_)
			std::swap(first, second);
		first->right_ = LeftistMeld_(first->right_, second);
		this->MakeSwap_(first);
		return first;
	}

protected:

	MyHeap* GetRight_(MyHeap *ptr) const
	{
		return ptr->right_;
	}

	MyHeap* GetLeft_(MyHeap *ptr) const
	{
		return ptr->left_;
	}

	int GetKey_(MyHeap *ptr) const
	{
		return ptr->vertex_.key_;
	}

	int GetRank_(MyHeap *ptr) const
	{
		return ptr->vertex_.rank_;
	}

	void SwapChildren_(MyHeap *ptr)
	{
		std::swap(ptr->left_, ptr->right_);
	}

	void UpdateKey_(MyHeap *ptr, int x)
	{
		ptr->vertex_.key_ = x;
	}

	void UpdateRank_(MyHeap *ptr, int x)
	{
		ptr->vertex_.rank_ = x;
	}

	virtual void MakeSwap_(MyHeap *ptr) {}

	MyHeap *left_;
	MyHeap *right_;
	HeapNode vertex_;
};

class LeftistHeap : public LeftistHeapTemplate < NodeLeftist >
{
protected:
	void MakeSwap_(LeftistHeapTemplate < NodeLeftist > *ptr)
	{
		LeftistHeap* cur = dynamic_cast < LeftistHeap* >(ptr);
		if (GetRank_(GetRight_(ptr)) > GetRank_(GetLeft_(ptr)))
			SwapChildren_(ptr);
		UpdateRank_(ptr, 1 + std::min(GetRank_(GetRight_(ptr)), GetRank_(GetLeft_(ptr))));
	}

	size_t GetRank_(LeftistHeapTemplate < NodeLeftist > *ptr) const
	{
		if (ptr == nullptr)
			return 0;
		else
			return LeftistHeapTemplate < NodeLeftist >::GetRank_(ptr);
	}
};

class SkewHeap : public LeftistHeapTemplate < NodeSkew >
{
protected:
	void MakeSwap_(LeftistHeapTemplate < NodeSkew > *ptr)
	{
		SwapChildren_(ptr);
	}
};
