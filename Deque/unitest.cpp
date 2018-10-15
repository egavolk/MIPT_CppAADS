#include "gtest\gtest.h"
#include "deque.h"
#include <deque>
#include <algorithm>

const int N_LINES_IN_TEST = 1e5;

int MyRand()
{
	return rand() ^ (rand() << 15);
}

TEST(TestDeque_methods, push_back_pop_back)
{
	std::deque < int > d1;
	Deque < int > d2;
	for (int i = 0; i < N_LINES_IN_TEST; ++i)
	{
		int x = MyRand();
		d1.push_back(x);
		d2.push_back(x);
		ASSERT_EQ(d1.size(), d2.size());
		ASSERT_EQ(d1.empty(), d2.empty());
		ASSERT_EQ(d1.back(), d2.back());
		ASSERT_EQ(d1.front(), d2.front());
	}

	for (int i = 0; i < N_LINES_IN_TEST; ++i)
	{
		d1.pop_back();
		d2.pop_back();
		ASSERT_EQ(d1.size(), d2.size());
		ASSERT_EQ(d1.empty(), d2.empty());
		if (!d1.empty())
		{
			ASSERT_EQ(d1.back(), d2.back());
			ASSERT_EQ(d1.front(), d2.front());
		}
	}
}

TEST(TestDeque_methods, push_front_pop_front)
{
	std::deque < int > d1;
	Deque < int > d2;
	for (int i = 0; i < N_LINES_IN_TEST; ++i)
	{
		int x = MyRand();
		d1.push_front(x);
		d2.push_front(x);
		ASSERT_EQ(d1.size(), d2.size());
		ASSERT_EQ(d1.empty(), d2.empty());
		ASSERT_EQ(d1.back(), d2.back());
		ASSERT_EQ(d1.front(), d2.front());
	}

	for (int i = 0; i < N_LINES_IN_TEST; ++i)
	{
		d1.pop_front();
		d2.pop_front();
		ASSERT_EQ(d1.size(), d2.size());
		ASSERT_EQ(d1.empty(), d2.empty());
		if (!d1.empty())
		{
			ASSERT_EQ(d1.back(), d2.back());
			ASSERT_EQ(d1.front(), d2.front());
		}
	}
}

TEST(TestDeque_methods, push_front_pop_back)
{
	std::deque < int > d1;
	Deque < int > d2;
	for (int i = 0; i < N_LINES_IN_TEST; ++i)
	{
		int x = MyRand();
		d1.push_front(x);
		d2.push_front(x);
		ASSERT_EQ(d1.size(), d2.size());
		ASSERT_EQ(d1.empty(), d2.empty());
		ASSERT_EQ(d1.back(), d2.back());
		ASSERT_EQ(d1.front(), d2.front());
	}
	for (int i = 0; i < N_LINES_IN_TEST; ++i)
	{
		d1.pop_back();
		d2.pop_back();
		ASSERT_EQ(d1.size(), d2.size());
		ASSERT_EQ(d1.empty(), d2.empty());
		if (!d1.empty())
		{
			ASSERT_EQ(d1.back(), d2.back());
			ASSERT_EQ(d1.front(), d2.front());
		}
	}
}

TEST(TestDeque_methods, push_back_pop_front)
{
	std::deque < int > d1;
	Deque < int > d2;
	for (int i = 0; i < N_LINES_IN_TEST; ++i)
	{
		int x = MyRand();
		d1.push_back(x);
		d2.push_back(x);
		ASSERT_EQ(d1.size(), d2.size());
		ASSERT_EQ(d1.empty(), d2.empty());
		ASSERT_EQ(d1.back(), d2.back());
		ASSERT_EQ(d1.front(), d2.front());
	}

	for (int i = 0; i < N_LINES_IN_TEST; ++i)
	{
		d1.pop_front();
		d2.pop_front();
		ASSERT_EQ(d1.size(), d2.size());
		ASSERT_EQ(d1.empty(), d2.empty());
		if (!d1.empty())
		{
			ASSERT_EQ(d1.back(), d2.back());
			ASSERT_EQ(d1.front(), d2.front());
		}
	}
}

TEST(Testdeque_operator, access_operator)
{
	std::deque < int > d1;
	Deque < int > d2;
	for (int i = 0; i < N_LINES_IN_TEST; ++i)
	{
		int x = MyRand();
		int y = MyRand();
		if (x % 2)
		{
			d1.push_back(y);
			d2.push_back(y);
		} 
		else
		{
			d1.push_front(y);
			d2.push_front(y);
		}
	}
	for (int i = 0; i < N_LINES_IN_TEST; ++i)
	{
		ASSERT_EQ(d1[i], d2[i]);
	}
	std::random_shuffle(d1.begin(), d1.end());
	for (int i = 0; i < N_LINES_IN_TEST; ++i)
	{
		d2[i] = d1[i];
	}
	for (int i = 0; i < N_LINES_IN_TEST; ++i)
	{
		ASSERT_EQ(d1[i], d2[i]);
	}
}

class Testdeque_iterator : public ::testing::Test
{
public:
	Testdeque_iterator()
		: d1(N_LINES_IN_TEST)
		, d2(N_LINES_IN_TEST)
	{
		
	}
protected:
	void SetUp()
	{
		std::random_shuffle(d1.begin(), d1.end());
		for (int i = 0; i < N_LINES_IN_TEST; ++i)
		{
			d2[i] = d1[i];
		}
		itb1 = d1.begin();
		itb2 = d2.begin();
		ite1 = d1.end();
		ite2 = d2.end();
	}
	std::deque < int > d1;
	Deque < int > d2;
	std::deque < int >::iterator itb1;
	Deque < int >::iterator itb2;
	std::deque < int >::iterator ite1;
	Deque < int >::iterator ite2;
};

TEST_F(Testdeque_iterator, iterator_difference)
{
	ASSERT_EQ(d1.end() - d1.begin(), d2.end() - d2.begin());
}

TEST_F(Testdeque_iterator, iterator_postincrement)
{
	for (int i = 0; i < N_LINES_IN_TEST; ++i)
		ASSERT_EQ(*(itb1++), *(itb2++));
}

TEST_F(Testdeque_iterator, iterator_preincrement)
{
	for (int i = 1; i < N_LINES_IN_TEST; ++i)
		ASSERT_EQ(*(++itb1), *(++itb2));
}

TEST_F(Testdeque_iterator, iterator_postdencrement)
{
	ite1--;
	ite2--;
	for (int i = 1; i < N_LINES_IN_TEST; ++i)
		ASSERT_EQ(*(ite1--), *(ite2--));
}

TEST_F(Testdeque_iterator, iterator_predencrement)
{
	for (int i = 0; i < N_LINES_IN_TEST; ++i)
		ASSERT_EQ(*(--ite1), *(--ite2));
}

TEST_F(Testdeque_iterator, iterator_plus)
{
	while (itb1 != d1.end())
	{
		ASSERT_EQ(*itb1, *itb2);
		int i = itb1 - d1.begin();
		int x = MyRand() % (N_LINES_IN_TEST - i);
		ASSERT_EQ(itb1[x], itb2[x]);
		ASSERT_EQ(*(itb1 + x), *(itb2 + x));
		ASSERT_EQ(*(x + itb1), *(x + itb2));
		itb1 += x;
		itb2 += x;
		if (N_LINES_IN_TEST - i == 1)
			break;
	}
}

TEST_F(Testdeque_iterator, iterator_minus)
{
	--ite1;
	--ite2;
	while (ite1 != d1.begin())
	{
		ASSERT_EQ(*ite1, *ite2);
		int i = ite1 - d1.begin();
		int x = MyRand() % i;
		ASSERT_EQ(ite1[-x], ite2[-x]);
		ASSERT_EQ(*(ite1 - x), *(ite2 - x));
		ite1 -= x;
		ite2 -= x;
		if (i == 1)
			break;
	}
}

TEST_F(Testdeque_iterator, iterator_eq)
{
	for (int i = 0; i < N_LINES_IN_TEST; ++i)
	{
		ASSERT_EQ(itb2 + i, ite2 - (N_LINES_IN_TEST - i));
	}
}

TEST_F(Testdeque_iterator, iterator_nq)
{
	for (int i = 0; i < N_LINES_IN_TEST; ++i)
	{
		int x = MyRand() % N_LINES_IN_TEST;
		int y = MyRand() % N_LINES_IN_TEST;
		if (x - y)
			ASSERT_NE(itb2 + x, itb2 + y);
	}
}

TEST_F(Testdeque_iterator, iterator_lt)
{
	for (int i = 0; i < N_LINES_IN_TEST; ++i)
	{
		int x = MyRand() % N_LINES_IN_TEST;
		int y = MyRand() % N_LINES_IN_TEST;
		if (x - y)
			ASSERT_LT(itb2 + std::min(x, y), itb2 + std::max(y, x));
	}
}

TEST_F(Testdeque_iterator, iterator_le)
{
	for (int i = 0; i < N_LINES_IN_TEST; ++i)
	{
		int x = MyRand() % N_LINES_IN_TEST;
		int y = MyRand() % N_LINES_IN_TEST;
		ASSERT_LE(itb2 + std::min(x, y), itb2 + std::max(y, x));
	}
}

TEST_F(Testdeque_iterator, iterator_gt)
{
	for (int i = 0; i < N_LINES_IN_TEST; ++i)
	{
		int x = MyRand() % N_LINES_IN_TEST;
		int y = MyRand() % N_LINES_IN_TEST;
		if (x - y)
			ASSERT_GT(itb2 + std::max(x, y), itb2 + std::min(y, x));
	}
}

TEST_F(Testdeque_iterator, iterator_ge)
{
	for (int i = 0; i < N_LINES_IN_TEST; ++i)
	{
		int x = MyRand() % N_LINES_IN_TEST;
		int y = MyRand() % N_LINES_IN_TEST;
		ASSERT_GE(itb2 + std::max(x, y), itb2 + std::min(y, x));
	}
}

int main(int argc, char *argv[])
{
	::testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
}
