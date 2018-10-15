#pragma once
#include "TemplateHeap.h"
#include <limits>
#include <algorithm>

class BinomialHeap : public IHeap
{
private:
	typedef BinomialHeap MyHeap;
public:
	BinomialHeap()
		: key_ (INT_MAX)
		, rank_(-INT_MAX)
		, child_(nullptr)
		, parent_(nullptr)
		, brother_(nullptr)
	{
		
	}

	explicit BinomialHeap(int key)
		: key_(INT_MAX)
		, rank_(-INT_MAX)
		, child_(nullptr)
		, parent_(nullptr)
		, brother_(nullptr)
	{
		this->brother_ = new MyHeap();
		this->brother_->key_ = key;
		this->brother_->rank_ = 0;
	}

	BinomialHeap(const BinomialHeap &other)
		: key_(other.key_)
		, rank_(other.rank_)
		, child_(other.child_)
		, parent_(other.parent_)
		, brother_(other.brother_)
	{

	}

	~BinomialHeap()
	{
		delete this->brother_;
		delete this->child_;
	}

	MyHeap& operator=(const MyHeap& other)
	{
		key_ = other.key_;
		rank_ = other.rank_;
		parent_ = other.parent_;
		child_ = other.child_;
		brother_ = other.brother_;
		return *this;
	}

	void Meld(IHeap *ptr)
	{
		MyHeap *other = dynamic_cast <BinomialHeap*>(ptr);
		MyHeap *cur1 = this->brother_;
		MyHeap *cur2 = other->brother_;
		other->brother_ = nullptr;
		MyHeap *begin = this;
		while ((cur1 != nullptr) || (cur2 != nullptr))
		{
			if ((cur2 == nullptr) || ((cur1) && (cur1->rank_ < cur2->rank_)))
			{
				begin->brother_ = cur1;
				cur1 = cur1->brother_;
			}
			else
			{
				begin->brother_ = cur2;
				cur2 = cur2->brother_;
			}
			begin = begin->brother_;
		}
		this->Update_();
	}

	int GetMin() const
	{
		MyHeap *min = GetMinPtr_();
		if (min == nullptr)
			return INT_MAX;
		return min->key_;
	}

	void Insert(int key)
	{
		MyHeap *cur = new MyHeap(key);
		if (this->brother_ == nullptr)
			this->brother_ = cur->brother_;
		else
			this->Meld(cur);
		cur->brother_ = nullptr;
		cur->child_ = nullptr;
	}

	void ExtractMin()
	{
		MyHeap *prev = GetPrevMinptr_();
		MyHeap *min = prev->brother_;
		if (min->rank_ == 0)
		{
			MyHeap *tmp = min->brother_;
			this->brother_->child_ = nullptr;
			this->brother_->brother_ = nullptr;
			delete this->brother_;
			this->brother_ = tmp;
			return;
		}
		prev->brother_ = min->brother_;
		MyHeap *cur = nullptr;
		while (min->child_)
		{
			MyHeap *tmp = min->child_;
			min->child_ = tmp->brother_;
			tmp->brother_ = nullptr;
			tmp->parent_ = nullptr;
			tmp->brother_ = cur;
			cur = tmp;
		}

		min->brother_ = nullptr;
		min->child_ = nullptr;
		delete min;
		
		MyHeap* cur_begin = new BinomialHeap();
		cur_begin->rank_ = -INT_MAX;
		cur_begin->brother_ = cur;
		this->Meld(cur_begin);
	}

	size_t GetSize() const
	{
		if (this == nullptr)
			return 0;
		if (this->rank_ < 0)
		{
			if (this->brother_)
				return this->brother_->GetSize();
			else
				return 0;
		}
		int ans = 1;
		if (this->child_)
			ans += this->child_->GetSize();
		if (this->brother_)
			ans += this->brother_->GetSize();
		return ans;
	}

private:
	MyHeap* GetPrevMinptr_() const
	{
		MyHeap *cur = const_cast < MyHeap* > (this);
		MyHeap *ans;
		int min = INT_MAX;
		while (cur->brother_ != nullptr)
		{
			if (min > cur->brother_->key_)
			{
				ans = cur;
				min = cur->brother_->key_;
			}
			cur = cur->brother_;
		}
		return ans;
	}

	MyHeap* GetMinPtr_() const
	{
		return GetPrevMinptr_()->brother_;
	}

	void Update_()
	{
		MyHeap *cur = this->brother_;
		MyHeap *prev = this;
		if (cur == nullptr)
			return;
		while (cur->brother_)
		{
			MyHeap *first = cur;
			MyHeap *second = cur->brother_;
			if ((first->rank_ == second->rank_) && !((second->brother_) && (second->rank_ == second->brother_->rank_)))
			{
				if (second->key_ < first->key_)
				{
					MySwap_(prev, first, second);
					Unite_(second);
					cur = second;
				}
				else
				{
					Unite_(first);
					cur = first;
				}
			}
			else
			{
				prev = cur;
				cur = cur->brother_;
			}
		}
	}

	void Unite_(MyHeap *cur)
	{
		MyHeap *tmp = cur->brother_;
		cur->brother_ = tmp->brother_;
		tmp->brother_ = cur->child_;
		cur->child_ = tmp;
		tmp->parent_ = cur;
		++cur->rank_;
	}

	void MySwap_(MyHeap *prev, MyHeap *first, MyHeap *second)
	{
		prev->brother_ = second;
		first->brother_ = second->brother_;
		second->brother_ = first;
		std::swap(first, second);
	}

	int key_;
	int rank_;
	MyHeap *child_;
	MyHeap *parent_;
	MyHeap *brother_;
};
