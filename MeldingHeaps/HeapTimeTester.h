#pragma once
#include "MyTester.h"
#include "TemplateHeap.h"

class HeapTimeTester
{
public:
	explicit HeapTimeTester(bool update_time_test = false, int n_lines = 1e6, int max_n_heaps = 100)
		: update_time_test_(update_time_test)
		, n_lines_(n_lines)
		, max_n_heaps_(max_n_heaps)
	{

	}

	HeapTimeTester &operator=(const HeapTimeTester& other)
	{
		n_lines_ = other.n_lines_;
		return *this;
	}

	template < class T >
	void RunTimeTest()
	{
		std::vector < IHeap* > my_tmp(max_n_heaps_);
		for (int j = 0; j < max_n_heaps_; ++j)
		{
			my_tmp[j] = dynamic_cast <IHeap*> (new T());
		}
		clock_t first = clock();
		MyTimeTest_(my_tmp);
		clock_t second = clock();
		for (int j = 0; j < max_n_heaps_; ++j)
			delete my_tmp[j];
		printf("Time %.3f\n", 1.0 * (second - first) / CLOCKS_PER_SEC);
	}

	void CheckTimeTest()
	{
		if (!update_time_test_)
		{
			printf("Time test are not changed\n");
			return;
		}
		update_time_test_ = false;
		TestGenerator tester;
		tester.GenerateTest(1, n_lines_, n_lines_, "test\\test_time", max_n_heaps_);
		printf("Time test updated\n");
	}

private:
	void MyTimeTest_(std::vector < IHeap* > my)
	{
		size_t size1 = 0, size2 = 0;
		std::string NAME = GenerateFilename(0, "test\\test_time");
		FILE* f = fopen(NAME.c_str(), "r");
		int n;
		fscanf(f, "%d\n", &n);
		for (int j = 0; j < n; ++j)
		{
			char c[20];
			fscanf(f, "%s", c);
			std::string s = c;
			if (s == "AddHeap")
			{
				int key;
				fscanf(f, "%d\n", &key);
				my[size2++]->Insert(key);
			}
			if (s == "Insert")
			{
				int index, key;
				fscanf(f, "%d %d\n", &index, &key);
				my[index]->Insert(key);
			}
			if (s == "GetMin")
			{
				int index;
				fscanf(f, "%d\n", &index);
				index = my[index]->GetMin();
			}
			if (s == "Meld")
			{
				int index1, index2;
				fscanf(f, "%d %d\n", &index1, &index2);
				my[index1]->Meld(my[index2]);
			}
			if (s == "ExtractMin")
			{
				int index;
				fscanf(f, "%d\n", &index);
				my[index]->ExtractMin();
			}
		}
		fclose(f);
	}

	int max_n_heaps_;
	bool update_time_test_;
	int n_lines_;
};
