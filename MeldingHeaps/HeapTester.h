#pragma once
#include "PrimitiveHeap.h"
#include "MyTester.h"
#include "TemplateHeap.h"

class HeapTester
{
public:
	explicit HeapTester(int step = 10, int k_tests_for_step = 4, int degree = 4, int min_border = 2, int max_border = 6, int max_n_heaps = 100)
		: step_(step)
		, k_tests_for_step_(k_tests_for_step)
		, degree_(degree)
		, min_border_(min_border)
		, max_border_(max_border)
		, max_n_heaps_(max_n_heaps)
	{

	}

	HeapTester &operator=(const HeapTester& other)
	{
		step_ = other.step_;
		k_tests_for_step_ = other.k_tests_for_step_;
		degree_ = other.degree_;
		min_border_ = other.min_border_;
		max_border_ = other.max_border_;
		max_n_heaps_ = other.max_n_heaps_;
		return *this;
	}

	void CheckTests()
	{
		FILE *test_log = fopen("test_log.txt", "r");
		int cur_step, cur_k, cur_m, cur_min, cur_max;
		if (test_log)
		{
			fscanf(test_log, "%d %d %d %d %d", &cur_step, &cur_k, &cur_m, &cur_min, &cur_max);
			fclose(test_log);
			if ((step_ == cur_step) && (k_tests_for_step_ == cur_k) && (min_border_ == cur_min) && (degree_ == cur_m) && (max_border_ == cur_max))
			{
				printf("Tests are not changed\n");
				return;
			}
		}
		printf("Tests updated\n");
		test_log = fopen("test_log.txt", "w");
		fprintf(test_log, "%d %d %d %d %d", step_, k_tests_for_step_, degree_, min_border_, max_border_);
		int pow = 1;
		TestGenerator tester;
		for (int i = 1; i <= k_tests_for_step_; ++i)
		{
			pow *= step_;
			tester.GenerateTest(degree_, pow / min_border_, pow * max_border_, "test\\test", max_n_heaps_);
		}
		fclose(test_log);
		UpdatePrimitive_();
	}

	template < class T >
	void RunTest()
	{
		for (int i = 0; i < k_tests_for_step_ * degree_; ++i)
		{
			std::vector < IHeap* > my_tmp(max_n_heaps_);
			for (int j = 0; j < max_n_heaps_; ++j)
			{
				my_tmp[j] = dynamic_cast <IHeap*> (new T());
			}
			MyTest_(i, my_tmp);
			for (int j = 0; j < max_n_heaps_; ++j)
				delete my_tmp[j];
		}
	}

private:
	void UpdatePrimitive_()
	{
		for (int i = 0; i < k_tests_for_step_ * degree_; ++i)
		{
			std::vector < PrimitiveHeap > prime;
			std::string NAME_IN = GenerateFilename(i, "test\\test");
			std::string NAME_OUT = GenerateFilename(i, "ans\\ans");
			FILE *f_in = fopen(NAME_IN.c_str(), "r");
			FILE *f_out = fopen(NAME_OUT.c_str(), "w");
			int n;
			fscanf(f_in, "%d\n", &n);
			for (int j = 0; j < n; ++j)
			{
				char c[20];
				fscanf(f_in, "%s", c);
				std::string s = c;
				if (s == "AddHeap")
				{
					int key;
					fscanf(f_in, "%d\n", &key);
					prime.push_back(PrimitiveHeap(key));
				}
				if (s == "Insert")
				{
					int index, key;
					fscanf(f_in, "%d %d\n", &index, &key);
					prime[index].Insert(key);
				}
				if (s == "GetMin")
				{
					int index;
					fscanf(f_in, "%d\n", &index);
					fprintf(f_out, "%d\n", prime[index].GetMin());
				}
				if (s == "Meld")
				{
					int index1, index2;
					fscanf(f_in, "%d %d\n", &index1, &index2);
					prime[index1].Meld(&prime[index2]);
				}
				if (s == "ExtractMin")
				{
					int index;
					fscanf(f_in, "%d\n", &index);
					prime[index].ExtractMin();
				}
			}
			fclose(f_in);
			fclose(f_out);
		}
		printf("Primitive heap is ready\n");
	}

	void MyTest_(int i, std::vector < IHeap* > my)
	{
		size_t size2 = 0, n_getmin = 0;
		std::string NAME_TEST = GenerateFilename(i, "test\\test");
		std::string NAME_ANS = GenerateFilename(i, "ans\\ans");
		FILE* f_test = fopen(NAME_TEST.c_str(), "r");
		FILE* f_ans = fopen(NAME_ANS.c_str(), "r");
		int n;
		fscanf(f_test, "%d\n", &n);
		for (int j = 0; j < n; ++j)
		{
			char c[20];
			fscanf(f_test, "%s", c);
			std::string s = c;
			if (s == "AddHeap")
			{
				int key;
				fscanf(f_test, "%d\n", &key);
				my[size2++]->Insert(key);
			}
			if (s == "Insert")
			{
				int index, key;
				fscanf(f_test, "%d %d\n", &index, &key);
				my[index]->Insert(key);
			}
			if (s == "GetMin")
			{
				int index, ans;
				fscanf(f_test, "%d\n", &index);
				fscanf(f_ans, "%d\n", &ans);
				ASSERT_EQ(ans, my[index]->GetMin()) << "test" << i << ".txt line: " << j + 2 << "\nGetMin " << index;
			}
			if (s == "Meld")
			{
				int index1, index2;
				fscanf(f_test, "%d %d\n", &index1, &index2);
				my[index1]->Meld(my[index2]);
			}
			if (s == "ExtractMin")
			{
				int index;
				fscanf(f_test, "%d\n", &index);
				my[index]->ExtractMin();
			}
		}
		fclose(f_test);
		fclose(f_ans);
		printf("test %d passed\n", i);
	}

	int step_;
	int k_tests_for_step_;
	int degree_;
	int min_border_;
	int max_border_;
	int max_n_heaps_;
};
