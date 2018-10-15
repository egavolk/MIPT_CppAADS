#pragma once
template < typename T >
class StackAllocator
{
public:
    typedef T value_type;
    typedef StackAllocator < T > Alloc;

    StackAllocator() noexcept
        : currentStorageIndex_(-1)
        , currentStorageSize_(SIZE_OF_STORAGE_)
    {
        storagesPtr_ = new T*[MAX_NUMBER_OF_STORAGES_];
    }

    ~StackAllocator() noexcept
    {
        for (int i = 0; i < currentStorageIndex_; ++i)
            delete[] storagesPtr_[i];
        delete[] storagesPtr_;
    }

    template < typename U >
    StackAllocator(const StackAllocator < U >& other) noexcept
        : StackAllocator < T >()
    {

    }

    StackAllocator(const StackAllocator < T >& other) noexcept
        : StackAllocator < T >()
    {

    }

    StackAllocator(StackAllocator < T >&& other) noexcept
        : storagesPtr_(other.storagesPtr_)
        , currentStorageIndex_(other.currentStorageIndex_)
        , currentStorageSize_(other.currentStorageSize_)
    {

    }

    Alloc& operator=(StackAllocator < T >&& other) noexcept
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

    T* allocate(size_t n)
    {
        if (currentStorageSize_ + n > SIZE_OF_STORAGE_)
        {
            ++currentStorageIndex_;
            storagesPtr_[currentStorageIndex_] = new T[SIZE_OF_STORAGE_];
            currentStorageSize_ = 0;
        }
        if (n > SIZE_OF_STORAGE_)
            throw "Too big n in allocate function";
        T* res = storagesPtr_[currentStorageIndex_] + currentStorageSize_;
        currentStorageSize_ += n;
        return res;
    }

    void deallocate(T* p, size_t n) {}

    template < typename U >
    struct rebind
    {
        typedef StackAllocator < U > other;
    };
    
private:
    T** storagesPtr_;
    static const size_t MAX_NUMBER_OF_STORAGES_ = 100;
    static const size_t BYTES_IN_STORAGE_ = 1000000;
    static const size_t SIZE_OF_STORAGE_ = BYTES_IN_STORAGE_ / sizeof(T);
    int currentStorageIndex_;
    size_t currentStorageSize_;
};

template < typename T1, typename T2 >
constexpr bool operator== (const StackAllocator < T1 > & alloc1, const StackAllocator < T2 > & alloc2) noexcept
{
    return true;
}

template < typename T1, typename T2 >
constexpr bool operator!= (const StackAllocator < T1 > & alloc1, const StackAllocator < T2 > & alloc2) noexcept
{
    return false;
}
