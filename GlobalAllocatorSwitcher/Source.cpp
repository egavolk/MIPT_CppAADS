#include "AllocatorSwitcher.h"
#include "StackAllocator.h"
#include <list>
#include <cstdio>

void f();

class MyClass
    : public CAllocatedOn < RuntimeHeap >
{

};

int main()
{
    StackAllocator alloc = StackAllocator();
	for (int j = 0; j < 1; ++j)
	{
		CMemoryManagerSwitcher switcher = CMemoryManagerSwitcher(&alloc);
		f();
		printf("%d", sizeof(IMemoryManager));
		long long* a = new long long[5];
		for (int i = 0; i < 5; ++i)
		{
			a[i] = i;
			printf("%lld", a[i]);
		}
		delete[] a;
		MyClass* c1 = new MyClass[10];
		MyClass* c2 = new MyClass[10];
		delete[] c1;
		delete[] c2;
	}
	int* g = new int[10];
	f();
    return 0;
}