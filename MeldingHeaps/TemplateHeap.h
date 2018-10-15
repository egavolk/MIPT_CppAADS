#pragma once

class IHeap
{
public:

	virtual ~IHeap() {}

	virtual void Insert(int key) = 0;

	virtual int GetMin() const = 0;

	virtual void ExtractMin() = 0;

	virtual void Meld(IHeap *other) = 0;

	virtual size_t GetSize() const = 0;
};
