#pragma once
#include <algorithm>
#include <list>
#include <ctime>
#include <vector>
#include <numeric>

enum OperationType { PushBack = 0, PopBack = 1, PushFront = 2, PopFront = 3 };

template < typename T, typename List >
class TimeListChecker
{
public:
    TimeListChecker() noexcept {}

    clock_t Time(std::vector < OperationType > &operations, std::vector < T > & values)
    {
        clock_t time = clock();
        for (size_t i = 0; i < operations.size(); ++i)
        {
            if (operations[i] == PushBack)
                list_.push_back(values[i]);
            else if (operations[i] == PopBack)
                list_.pop_back();
            else if (operations[i] == PushFront)
                list_.push_front(values[i]);
            else if (operations[i] == PopFront)
                list_.pop_front();
        }
        return clock() - time;
    }

    void MakeOperationsList(std::vector < OperationType > &operations, std::vector < T > & values, size_t* numbersOfOperations)
    {
        operations.clear();
        values.clear();
        if (numbersOfOperations[2] + numbersOfOperations[0] <
            numbersOfOperations[1] + numbersOfOperations[3])
            throw "Invalid values of numbersOfOperations";
        size_t currentSize = 0;
        size_t n = std::accumulate(numbersOfOperations, numbersOfOperations + 4, 0);
        size_t numbersOfOperationsCopy[4];
        std::copy_n(numbersOfOperations, 4, numbersOfOperationsCopy);
        std::vector < OperationType > pushPop = { PushBack, PopBack, PushFront, PopFront };
        std::vector < OperationType > push = { PushBack, PushFront };
        for (size_t i = 0; i < n; ++i)
        {
            size_t x = rand();
            values.push_back(x);
            if (currentSize)
            {
                InsertOperation(operations, pushPop, push, numbersOfOperationsCopy, currentSize, x);
            }
            else
            {
                InsertOperation(operations, push, pushPop, numbersOfOperationsCopy, currentSize, x);
            }
        }
    }

public:
    List list_;

    void MyErase(std::vector < OperationType > &a, OperationType x)
    {
        int toErase = -1;
        for (size_t i = 0; i < a.size(); ++i)
            if (a[i] == x)
                toErase = i;
        if (toErase != -1)
            a.erase(a.begin() + toErase);
    }

    void InsertOperation(std::vector < OperationType > &operations, std::vector < OperationType > &first,
        std::vector < OperationType > &second, size_t* numbersOfOperationsCopy, size_t &size, size_t x)
    {
        size_t y = x % first.size();
        OperationType current = first[y];
        operations.push_back(current);
        --numbersOfOperationsCopy[current];
        if (!numbersOfOperationsCopy[current])
        {
            MyErase(first, current);
            MyErase(second, current);
        }
        if (current % 2)
            size--;
        else
            size++;
    }
};

class CompareListsAndAllocators
{
public:
    template < typename T, typename List1, typename List2 >
    void Compare()
    {
        std::vector < OperationType > operations;
        std::vector < int > values;
        TimeListChecker < T, List1 > checker1;
        TimeListChecker < T, List2 > checker2;
        size_t numbersOfOperations[4] = { 1000000, 1000, 300000, 2000 };
        checker1.MakeOperationsList(operations, values, numbersOfOperations);
        printf("list1 Alloc1: %.3f\n", 1.0 * checker1.Time(operations, values) / CLOCKS_PER_SEC);
        printf("list2 Alloc2 : %.3f\n", 1.0 * checker2.Time(operations, values) / CLOCKS_PER_SEC);
        printf(":)\n");
    }
};