#include "deque.h"
#include <cstdio>
#include <algorithm>
#include <vector>
#include <ctime>

int MyRand()
{
	return rand() ^ (rand() << 15);
}

void MakePopBack(Deque < int > &d, long long &operations)
{
	if (d.GetBackIndex() == 0)
		operations += d.GetFrontSize();
	else
		if (d.GetBackIndex() * 4 == d.GetBackSize() && d.GetBackSize() != d.GetMinSize())
			operations += d.GetBackSize();
		else
			operations++;
	d.pop_back();
}

void MakePopFront(Deque < int > &d, long long &operations)
{
	if (d.GetFrontIndex() == 0)
		operations += d.GetBackSize();
	else
		if (d.GetFrontIndex() * 4 == d.GetFrontSize() && d.GetFrontSize() != d.GetMinSize())
			operations += d.GetFrontSize();
		else
			operations++;
	d.pop_front();
}

void MakePushBack(Deque < int > &d, int x, long long &operations)
{
	if (d.GetBackSize() == d.GetBackIndex())
		operations += 2 * d.GetBackSize();
	else
		operations++;
	d.push_back(x);
}

void MakePushFront(Deque < int > &d, int x, long long &operations)
{
	if (d.GetFrontSize() == d.GetFrontIndex())
		operations += 2 * d.GetFrontSize();
	else
		operations++;
	d.push_front(x);
}

std::pair < double, double > f(int n)
{
	clock_t t = clock();
	long long operations = 0;
	Deque < int > d;
	for (int i = 0; i < 2 * n; ++i)
	{
		MakePushBack(d, 1, operations);
	}
	for (int i = 0; i < 2 * n; ++i)
	{
		MakePushFront(d, 1, operations);
	}
	for (int i = 0; i < 3 * n; ++i)
	{
		MakePopBack(d, operations);
	}
	for (int i = 0; i < 2 * n; ++i)
	{
		MakePushBack(d, 1, operations);
	}
	for (int i = 0; i < 3 * n; ++i)
	{
		MakePopFront(d, operations);
	}
	return std::make_pair(1.0 * operations / (6 * n), 
		1.0 * (clock() - t) / CLOCKS_PER_SEC);
}

enum test { pop_back = 0, pop_front = 1, push_back = 2, push_front = 3 };

std::pair < double, double > g(int n)
{
	clock_t t = clock();
	long long operations = 0;
	Deque < int > d;
	for (int i = 0; i < 2 * n; ++i)
	{
		if (MyRand() % 2)
			MakePushBack(d, 1, operations);
		else
			MakePushFront(d, 1, operations);
	}
	int k = 2 * n;
	for (int i = 0; i < 2 * n; ++i)
	{
		int x = MyRand() % 4;
		switch (x)
		{
		case pop_back:
			MakePopBack(d, operations);
			break;
		case pop_front:
			MakePopFront(d, operations);
			break;
		case push_back:
			MakePushBack(d, 1, operations);
			++k;
			break;
		case push_front:
			MakePushFront(d, 1, operations);
			++k;
			break;
		}
	}
	return std::make_pair(1.0 * operations / k,
		1.0 * (clock() - t) / CLOCKS_PER_SEC);
}

int main()
{
	for (int i = 200; i < 10000000; i *= 2)
	{
		std::pair < double, double > c1 = f(i);
		std::pair < double, double > c2 = g(i);
		printf("N = %d      C1 = %.3f    time1 = %.3f     C2 = %.3f    time = %.3f\n", i, c1.first, c1.second, c2.first, c2.second);
	}
	return 0;
}
