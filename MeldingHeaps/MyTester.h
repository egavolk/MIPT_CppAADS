#pragma once
#include <string>
#include <vector>

std::string IntToStr(int i)
{
	std::string cur;
	while (i > 0)
	{
		cur += ('0' + i % 10);
		i /= 10;
	}
	std::reverse(cur.begin(), cur.end());
	return cur;
}

std::string GenerateFilename(int i, const std::string &s)
{
	return s + IntToStr(i) + ".txt";
}

class TestGenerator
{
public:
	TestGenerator()
		: TEST_NUMBER_(0)
	{
		
	}

	void GenerateTest(int n, int min_n_lines, int max_n_lines, const std::string &s, int max_n_heaps)
	{
		s_ = s;
		for (int i = 0; i < n; ++i)
		{
			int lines = MyRand_() % (max_n_lines - min_n_lines + 1);
			lines += min_n_lines;
			GenerateOneTest(lines, max_n_heaps);
			++TEST_NUMBER_;
		}
	}

private:
	enum CaseTest { add_heap = 0, insert = 1, get_min = 2, extract_min = 3, meld = 4 };

	int MyRand_()
	{
		return rand() ^ (rand() << 15);
	}

	void Write_(FILE *f, int max_n_heaps)
	{
		int key = MyRand_() % MAX_HEAP_KEY_;
		if (a_.empty())
		{
			fprintf(f, "AddHeap %d\n", key);
			a_.push_back(1);
			return;
		}

		int k = MyRand_() % 5;
		int index = MyRand_() % a_.size();
		while (((a_[index] == 0) && ((k == get_min) || (k == extract_min))) ||
			((a_.size() <= 1) && (k == meld)) || ((k == add_heap) && (a_.size() >= max_n_heaps)))
			k = MyRand_() % 5;

		switch (k)
		{
		case add_heap:
			fprintf(f, "AddHeap %d\n", key);
			a_.push_back(0);
			break;
		case insert:
			fprintf(f, "Insert %d %d\n", index, key);
			++a_[index];
			break;
		case get_min:
			fprintf(f, "GetMin %d\n", index);
			break;
		case extract_min:
			fprintf(f, "ExtractMin %d\n", index);
			--a_[index];
			break;
		case meld:
			int index2 = index + 1 + MyRand_() % (a_.size() - 1);
			index2 %= (a_.size());
			fprintf(f, "Meld %d %d\n", index, index2);
			a_[index] += a_[index2];
			a_[index2] = 0;
			break;
		}
	}
	
	void GenerateOneTest(int lines, int max_n_heaps)
	{
		a_.resize(0);
		std::string NAME = GenerateFilename(TEST_NUMBER_, s_);
		FILE* f = fopen(NAME.c_str(), "w");
		fprintf(f, "%d\n", lines);
		for (int i = 0; i < lines; ++i)
			Write_(f, max_n_heaps);
		fclose(f);
	}

	static const int MAX_N_HEAPS = 100;
	int TEST_NUMBER_;
	const int MAX_HEAP_KEY_ = 1e9;
	std::vector < int > a_;
	std::string s_;
};

