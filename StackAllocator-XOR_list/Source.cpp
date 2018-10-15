#include <cstdio>
#include <algorithm>
#include <list>
#include <ctime>
#include <vector>
#include <numeric>
#include <memory>
#include <string>
#include "StackAllocator.h"
#include "TimeListChecker.h"
#include "XORList.h"


int main() 
{
    CompareListsAndAllocators comp;
    printf("std::list + std::alloc VS std::list + StackAlloc\n");
    comp.Compare < int, std::list < int >, std::list < int, StackAllocator < int > > >();
    printf("XORList + std::alloc VS std::list + std::alloc\n");
    comp.Compare < int, XORList < int >, std::list < int > >();
    printf("XORList + std::alloc VS XORlist + StackAlloc\n");
    comp.Compare < int, XORList < int >, XORList < int, StackAllocator < int > > >();
    return 0;
}