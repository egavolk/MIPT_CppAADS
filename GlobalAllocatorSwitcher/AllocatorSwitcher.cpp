#pragma once
#include <cstdlib>
#include <new>
#include <algorithm>
#include "AllocatorSwitcher.h"

static IMemoryManager* currentAllocatorPtr = nullptr;

CMemoryManagerSwitcher::CMemoryManagerSwitcher(IMemoryManager* newAlloctor)
{
	previousAllocator_ = currentAllocatorPtr;
	currentAllocatorPtr = newAlloctor;
}

CMemoryManagerSwitcher::~CMemoryManagerSwitcher()
{
	currentAllocatorPtr = previousAllocator_;
}

void* CurrentMemoryManager::Alloc(size_t size)
{
	return currentAllocatorPtr->Alloc(size);
}

void CurrentMemoryManager::Free(void* ptr)
{
	currentAllocatorPtr->Free(ptr);
}

static constexpr unsigned allign = std::max(alignof(std::max_align_t), sizeof(IMemoryManager*));


void* MyNew_(size_t count)
{
	char* currentPtr;
	if (currentAllocatorPtr)
		currentPtr = static_cast <char*> (currentAllocatorPtr->Alloc(count + allign));
	else
		currentPtr = static_cast <char*> (malloc(count + allign));
	IMemoryManager** allocatorPtr = static_cast <IMemoryManager**> (static_cast < void* > (currentPtr));
	*allocatorPtr = currentAllocatorPtr;
	return static_cast < void* >(static_cast < char* > (currentPtr) + allign);
}

void MyDelete_(void* ptr)
{
	ptr = static_cast < void* >(static_cast < char* >(ptr) - allign);
	IMemoryManager** allocatorPtrPtr = static_cast <IMemoryManager**> (ptr);
	IMemoryManager* allocatorPtr = *allocatorPtrPtr;
	if (allocatorPtr)
		allocatorPtr->Free(ptr);
	else
		free(ptr);
}

void* operator new(size_t count)
{
	return MyNew_(count);
}

void operator delete(void* ptr)
{
MyDelete_(ptr);
}

void* operator new[](size_t count)
{
	return MyNew_(count);
}

void operator delete[](void* ptr)
{
MyDelete_(ptr);
}

void* operator new(size_t count, const std::nothrow_t& tag) noexcept
{
	return MyNew_(count);
}

void operator delete(void* ptr, const std::nothrow_t& tag) noexcept
{
MyDelete_(ptr);
}

void* operator new[](size_t count, const std::nothrow_t& tag) noexcept
{
	return MyNew_(count);
}

void operator delete[](void* ptr, const std::nothrow_t& tag) noexcept
{
MyDelete_(ptr);
}


