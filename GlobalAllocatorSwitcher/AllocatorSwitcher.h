#pragma once
#include <cstdlib>
#include <new>
#include <algorithm>

class IMemoryManager
{
public:
	virtual void* Alloc(size_t count) = 0;

	virtual void Free(void* ptr) = 0;
};


class StandartAllocator
	: public IMemoryManager
{
public:
	void* Alloc(size_t count)
	{
		return std::malloc(count);
	}

	void Free(void* ptr)
	{
		std::free(ptr);
	}
};

class CMemoryManagerSwitcher
{
public:
	CMemoryManagerSwitcher(IMemoryManager* newAlloctor);

	~CMemoryManagerSwitcher();
private:
	IMemoryManager* previousAllocator_;
};

/*void* MyNew_(size_t count);

void MyDelete_(void* ptr);

void* operator new(size_t count);

void operator delete(void* ptr);

void* operator new[](size_t count);

void operator delete[](void* ptr);

void* operator new(size_t count, const std::nothrow_t& tag) noexcept;

void operator delete(void* ptr, const std::nothrow_t& tag) noexcept;

void* operator new[](size_t count, const std::nothrow_t& tag) noexcept;

void operator delete[](void* ptr, const std::nothrow_t& tag) noexcept;*/

class RuntimeHeap
{
public:
	static void* Alloc(size_t size)
	{
		return std::malloc(size);
	}

	static void Free(void* ptr)
	{
		std::free(ptr);
	}
};

class CurrentMemoryManager
{
public:
	static void* Alloc(size_t size);

	static void Free(void* ptr);
};


template < typename Strategy >
class CAllocatedOn
{
public:
	void* operator new(size_t count)
	{
		return Strategy::Alloc(count);
	}

	void operator delete(void* ptr)
	{
		return Strategy::Free(ptr);
	}

	void* operator new[](size_t count)
	{
		return Strategy::Alloc(count);
	}

		void operator delete[](void* ptr)
	{
		return Strategy::Free(ptr);
	}

		void* operator new(size_t count, const std::nothrow_t& tag) noexcept
	{
		return Strategy::Alloc(count);
	}

	void operator delete(void* ptr, const std::nothrow_t& tag) noexcept
	{
		return Strategy::Free(ptr);
	}

	void* operator new[](size_t count, const std::nothrow_t& tag) noexcept
	{
		return Strategy::Alloc(count);
	}

		void operator delete[](void* ptr, const std::nothrow_t& tag) noexcept
	{
		return Strategy::Free(ptr);
	}
};