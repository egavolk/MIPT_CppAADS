#pragma once
#include <cstdlib>
#include "AllocatorSwitcher.h"

class StackAllocator
    :public IMemoryManager
{
public:
    StackAllocator() noexcept
        : currentStorageIndex_(-1)
        , currentStorageSize_(SIZE_OF_STORAGE_)
    {
        storagesPtr_ = static_cast < char** > (std::malloc(MAX_NUMBER_OF_STORAGES_ * sizeof(char*)));
    }

    ~StackAllocator() noexcept
    {
        for (int i = 0; i < currentStorageIndex_; ++i)
            free(storagesPtr_[i]);
        free(storagesPtr_);
    }

    StackAllocator(const StackAllocator& other) noexcept
        : StackAllocator()
    {

    }

    StackAllocator(StackAllocator&& other) noexcept
        : storagesPtr_(other.storagesPtr_)
        , currentStorageIndex_(other.currentStorageIndex_)
        , currentStorageSize_(other.currentStorageSize_)
    {

    }

    StackAllocator& operator=(StackAllocator&& other) noexcept
    {
        if (this != &other)
        {
            this->~StackAllocator();

            storagesPtr_ = other.storagesPtr_;
            currentStorageIndex_ = other.currentStorageIndex_;
            currentStorageSize_ = other.currentStorageSize_;

            other.storagesPtr_ = nullptr;
        }
        return *this;
    }

    void* Alloc(size_t n)
    {
        if (currentStorageSize_ + n > SIZE_OF_STORAGE_)
        {
            ++currentStorageIndex_;
            storagesPtr_[currentStorageIndex_] = static_cast < char* > (std::malloc(SIZE_OF_STORAGE_));
            currentStorageSize_ = 0;
        }
        if (n > SIZE_OF_STORAGE_)
            throw "Too big n in allocate function";
        char* res = storagesPtr_[currentStorageIndex_] + currentStorageSize_;
        currentStorageSize_ += n;
        return static_cast < void* > (res);
    }

    void Free(void* p) {}

private:
    char** storagesPtr_;
    static const size_t MAX_NUMBER_OF_STORAGES_ = 100;
    static const size_t BYTES_IN_STORAGE_ = 1000000;
    static const size_t SIZE_OF_STORAGE_ = BYTES_IN_STORAGE_;
    int currentStorageIndex_;
    size_t currentStorageSize_;
};

template < typename T1, typename T2 >
constexpr bool operator== (const StackAllocator& alloc1, const StackAllocator& alloc2) noexcept
{
    return true;
}

template < typename T1, typename T2 >
constexpr bool operator!= (const StackAllocator& alloc1, const StackAllocator& alloc2) noexcept
{
    return false;
}