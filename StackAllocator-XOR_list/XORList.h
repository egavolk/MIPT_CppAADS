#pragma once
#include <iterator>
#include <memory>
#include <utility>

template < typename T >
class XORListNode
{
public:
    typedef XORListNode < T > Node;

    XORListNode()
        : data_(T())
        , neighbours_(0)
    {

    }

    XORListNode(Node &other)
        : data_(other.data_)
        , neighbours_(other.neighbours_)
    {

    }

    XORListNode(Node &&other)
        : data_(std::move(other.data_))
        , neighbours_(other.neighbours_)
    {
        other.data_ = T();
    }

    XORListNode(const T& value, Node* left, Node* right)
        : data_(value)
    {
        neighbours_ = (left ? reinterpret_cast < unsigned > (left) : 0) ^
            (right ? reinterpret_cast < unsigned > (right) : 0);
    }

    XORListNode(T&& value, Node* left, Node* right)
        : data_(std::move(value))
    {
        neighbours_ = (left ? reinterpret_cast < unsigned > (left) : 0) ^
            (right ? reinterpret_cast < unsigned > (right) : 0);

        value = T();
    }

    ~XORListNode()
    {
    }

    Node& operator=(const Node& other)
    {
        ~XORListNode();
        data_ = other.data_;
        neighbours_ = other.neighbours_;
    }

    Node& operator=(Node&& other)
    {
        
        ~XORListNode();
        data_ = std::move(other.data_);
        neighbours_ = other.neighbours_;
        
        other.data_ = T();
    }

    inline T& GetData()
    {
        return data_;
    }

    inline unsigned GetNeighbours()
    {
        return neighbours_;
    }

    inline void SetData(const T& other)
    {
        data_ = other;
    }

    inline void SetData(T&& other)
    {
        data_ = std::move(other);
        other = T();
    }

    inline void SetNeighbours(unsigned other)
    {
        neighbours_ = other;
    }

    inline void UpdateNeighbours(unsigned other)
    {
        neighbours_ ^= other;
    }

private:
    T data_;
    unsigned neighbours_;
};


template < typename T >
class XORListIterator
{
public:
    typedef T value_type;
    typedef size_t difference_type;
    typedef T* pointer;
    typedef T& reference;
    typedef std::bidirectional_iterator_tag iterator_category;

    typedef XORListNode < T > Node;
    typedef XORListIterator < T > MyIt;

    XORListIterator()
        : current_(nullptr)
        , next_(nullptr)
        , previous_(nullptr)
    {

    }

    XORListIterator(const MyIt& other)
        : current_(other.current_)
        , next_(other.next_)
        , previous_(other.previous_)
    {

    }

    XORListIterator(Node* prev, Node* cur, Node* next)
        : current_(cur)
        , next_(next)
        , previous_(prev)
    {

    }

    MyIt& operator=(const MyIt& other)
    {
        current_ = other.current_;
        next_ = other.next_;
        previous_ = other.previous_;
        return *this;
    }

    Node* GetCurrent() const
    {
        return current_;
    }

    Node* GetNext() const
    {
        return next_;
    }

    Node* GetPrevious() const
    {
        return previous_;
    }

    T& operator*() const
    {
        return current_->GetData();
    }

    T* operator->() const
    {
        return &(current_->GetData_());
    }

    MyIt& operator++()
    {
        if (next_)
        {
            unsigned newNextU = reinterpret_cast < unsigned >(current_) ^ next_->GetNeighbours();
            previous_ = current_;
            current_ = next_;
            next_ = reinterpret_cast < Node* >(newNextU);
        }
        else
        {
            previous_ = current_;
            current_ = nullptr;
        }
        return (*this);
    }

    MyIt operator++(int)
    {
        MyIter tmp = *this;
        ++(*this);
        return tmp;
    }

    MyIt& operator--()
    {
        if (previous_)
        {
            unsigned newPrevU = reinterpret_cast <unsigned>(current_) ^ previous_->GetNeighbours();
            next_ = current_;
            current_ = previous_;
            previous_ = reinterpret_cast <Node*>(newPrevU);
        }
        else
        {
            next_ = current_;
            current_ = nullptr;
        }
        return (*this);
    }

    MyIt operator--(int)
    {
        MyIter tmp = *this;
        --(*this);
        return tmp;
    }

    void Update(Node* previous, Node* current, Node* next)
    {
        previous_ = previous;
        current_ = current;
        next_ = next;
    }

private:

    Node* current_;
    Node* next_;
    Node* previous_;
};

template<typename T>
bool operator==(const XORListIterator<T>& first, const XORListIterator<T>& second)
{
    return (first.GetCurrent() == second.GetCurrent() &&
        first.GetNext() == second.GetNext() &&
        first.GetPrevious() == second.GetPrevious());
}

template<typename T>
bool operator!=(const XORListIterator<T>& first, const XORListIterator<T>& second)
{
    return !(first == second);
}


template < typename T, class Allocator = std::allocator < T > >
class XORList
{
public:
    typedef XORList < T, Allocator > MyList;
    typedef XORListNode < T > Node;
    typedef XORListIterator < T > iterator;
    typedef typename Allocator::template rebind < Node >::other AllocatorNode;
    typedef std::allocator_traits < AllocatorNode > MyNodeAllocTraits;

    explicit XORList(const Allocator& alloc = Allocator())
        : first_(nullptr)
        , last_(nullptr)
        , allocNode_(AllocatorNode(alloc))
        , size_(0)
    {

    }

    XORList(size_t count, const T& value = T(), const Allocator& alloc = Allocator())
        : allocNode_(AllocatorNode(alloc))
        , size_(count)
    {
        Node* prev = allocNode_.allocate(1);
        MyNodeAllocTraits::construct< Node, Node >(allocNode_, prev, Node(value, nullptr, nullptr));
        first_ = prev;
        for (size_t i = 1; i < count; ++i)
        {
            Node* cur = allocNode_.allocate(1);
            MyNodeAllocTraits::construct < Node, Node >(allocNode_, cur, Node(value, prev, nullptr));
            prev->UpdateNeighbours(reinterpret_cast < unsigned > (cur));
            prev = cur;
        }
        last_ = prev;
    }

    ~XORList()
    {
        clearData_();
    }

    XORList(const MyList& other)
        : allocNode_(other.allocNode_)
    {
        copy_(other, this);
    }

    XORList(MyList&& other)
        : allocNode_(std::move(other.allocNode_))
        , first_(other.first_)
        , last_(other.last_)
        , size_(other.size_)
    {
        other.first_ = nullptr;
        other.last_ = nullptr;
    }

    MyList& operator=(const MyList& other)
    {
        if (&other != this)
        {
            clearData_();
            copy_(other, this);
        }
        return *this;
    }

    MyList& operator=(MyList&& other)
    {
        if (&other != this)
        {
            clearData_();

            first_ = other.first_;
            last_ = other.last_;
            size_ = other.size_;
            allocNode_ = std::move(other.allocNode_);

            other.first_ = nullptr;
            other.last_ = nullptr;
        }
        return *this;
    }

    const size_t size()
    {
        return size_;
    }

    void push_back(const T& value)
    {
        push_side_(last_, value);
    }

    void push_front(const T& value)
    {
        push_side_(first_, value);
    }

    void push_back(T&& value)
    {
        push_side_(last_, std::move(value));
    }

    void push_front(T&& value)
    {
        push_side_(first_, std::move(value));
    }

    void pop_back()
    {
        pop_side_(last_);
    }

    void pop_front()
    {
        pop_side_(first_);
    }

    iterator begin()
    {
        return iterator(nullptr, first_, reinterpret_cast < Node* >(first_->GetNeighbours()));
    }

    iterator end()
    {
        return iterator(last_, nullptr, nullptr);
    }

    void insert_before(iterator& it, const T& value)
    {
        Node* cur = insert_between_(it.GetPrevious(), it.GetCurrent(), value);
        it.Update(cur, it.GetCurrent(), it.GetNext());
    }

    void insert_before(iterator& it, T&& value)
    {
        Node* cur = insert_between_(it.GetPrevious(), it.GetCurrent(), std::move(value));
        it.Update(cur, it.GetCurrent(), it.GetNext());
    }

    void insert_after(iterator& it, const T& value)
    {
        Node* cur = insert_between_(it.GetCurrent(), it.GetNext(), value);
        it.Update(it.GetPrevious(), it.GetCurrent(), cur);
    }

    void insert_after(iterator& it, T&& value)
    {
        Node* cur = insert_between_(it.GetCurrent(), it.GetNext(), std::move(value));
        it.Update(it.GetPrevious(), it.GetCurrent(), cur);
    }

    void insert_before(iterator&& it, const T& value)
    {
        Node* cur = insert_between_(it.GetPrevious(), it.GetCurrent(), value);
    }

    void insert_before(iterator&& it, T&& value)
    {
        Node* cur = insert_between_(it.GetPrevious(), it.GetCurrent(), std::move(value));
    }

    void insert_after(iterator&& it, const T& value)
    {
        Node* cur = insert_between_(it.GetCurrent(), it.GetNext(), value);
    }

    void insert_after(iterator&& it, T&& value)
    {
        Node* cur = insert_between_(it.GetCurrent(), it.GetNext(), std::move(value));
    }

    void erase(iterator& it)
    {
        --size_;
        if (it.GetPrevious())
        {
            it.GetPrevious()->UpdateNeighbours(reinterpret_cast < unsigned > (it.GetCurrent()) ^
                reinterpret_cast < unsigned > (it.GetNext()));
        }
        unsigned nextNodeU;
        if (it.GetNext())
        {
            it.GetNext()->UpdateNeighbours(reinterpret_cast < unsigned > (it.GetCurrent()) ^
                reinterpret_cast < unsigned > (it.GetPrevious()));
            nextNodeU = 0;
        }
        else
            nextNodeU = reinterpret_cast < unsigned > (it.GetCurrent()) ^ it.GetNext()->GetNeighbours();
        MyNodeAllocTraits::destroy < Node >(allocNode_, it.GetCurrent());
        allocNode_.deallocate(it.GetCurrent(), 1);
        it.Update(it.GetPrevious(), it.GetNext(), reinterpret_cast < unsigned > (nextNodeU));
    }

private:
    Node* insert_between_(Node* before, Node* after, const T& value)
    {
        if (before == nullptr)
        {
            push_front(value);
            return first_;
        }
        if (after == nullptr)
        {
            push_back(value);
            return last_;
        }
        ++size_;
        Node* cur = allocNode_.allocate(1);
        MyNodeAllocTraits::construct < Node, Node >(allocNode_, cur, Node(value, before, after));
        before->UpdateNeighbours(reinterpret_cast < unsigned > (cur) ^ reinterpret_cast < unsigned > (after));
        after->UpdateNeighbours(reinterpret_cast < unsigned > (cur) ^ reinterpret_cast < unsigned > (before));
        return cur;
    }

    Node* insert_between_(Node* before, Node* after, T&& value)
    {
        if (before == nullptr)
        {
            push_front(std::move(value));
            return first_;
        }
        if (after == nullptr)
        {
            push_back(std::move(value));
            return last_;
        }
        ++size_;
        Node* cur = allocNode_.allocate(1);
        MyNodeAllocTraits::construct < Node, Node >(allocNode_, cur, Node(std::move(value), before, after));
        before->UpdateNeighbours(reinterpret_cast < unsigned > (cur) ^ reinterpret_cast < unsigned > (after));
        after->UpdateNeighbours(reinterpret_cast < unsigned > (cur) ^ reinterpret_cast < unsigned > (before));
        return cur;
    }

    void push_side_(Node*& side, const T& value)
    {
        ++size_;
        Node* cur = allocNode_.allocate(1);
        MyNodeAllocTraits::construct < Node, Node >(allocNode_, cur, Node(value, side, nullptr));
        if (size_ == 1)
        {
            first_ = last_ = cur;
            return;
        }
        side->UpdateNeighbours(reinterpret_cast < unsigned > (cur));
        side = cur;
    }

    void push_side_(Node*& side, T&& value)
    {
        ++size_;
        Node* cur = allocNode_.allocate(1);
        MyNodeAllocTraits::construct < Node, Node >(allocNode_, cur, Node(std::move(value), side, nullptr));
        if (size_ == 1)
        {
            first_ = last_ = cur;
            return;
        }
        side->UpdateNeighbours(reinterpret_cast < unsigned > (cur));
        side = cur;
    }

    void pop_side_(Node*& side)
    {
        --size_;
        if (size_ == 0)
        {
            first_ = last_ = nullptr;
            return;
        }
        Node* cur = reinterpret_cast < Node* > (side->GetNeighbours());
        cur->UpdateNeighbours(reinterpret_cast < unsigned > (side));
        MyNodeAllocTraits::destroy < Node >(allocNode_, side);
        allocNode_.deallocate(side, 1);
        side = cur;
    }

    void clearData_()
    {
        if (first_)
        {
            Node* cur;
            unsigned prevPtr = 0;
            do
            {
                cur = reinterpret_cast <Node*> (first_->GetNeighbours() ^ prevPtr);
                prevPtr = reinterpret_cast <unsigned> (first_);
                allocNode_.deallocate(first_, 1);
                first_ = cur;
            } while (first_);
            first_ = nullptr;
            last_ = nullptr;
            size_ = 0;
        }
    }

    void copy_(const MyList& from, MyList* to)
    {
        if (from.size_ == 0)
        {
            to->first_ = nullptr;
            to->last_ = nullptr;
            to->size_ = 0;
            return;
        }
        Node* prevFrom = from.first_;
        Node* curFrom = reinterpret_cast < Node* >(from.first_->GetNeighbours());
        Node* prev = allocNode_.allocate(1);
        MyNodeAllocTraits::construct < Node, Node >(allocNode_, prev, Node(prevFrom->GetData(), nullptr, nullptr));
        to->first_ = prev;
        while (curFrom)
        {
            Node* cur = allocNode_.allocate(1);
            MyNodeAllocTraits::construct < Node, Node >(allocNode_, cur, Node(curFrom->GetData(), nullptr, nullptr));
            prev->UpdateNeighbours(reinterpret_cast < unsigned > (cur));
            prev = cur;
            unsigned nextFromU = reinterpret_cast < unsigned > (prevFrom) ^ curFrom->GetNeighbours();
            prevFrom = curFrom;
            curFrom = reinterpret_cast < Node* >(nextFromU);
        }
        to->last_ = prev;
        to->size_ = from.size_;
    }

    Node* first_;
    Node* last_;
    AllocatorNode allocNode_;
    size_t size_;
};