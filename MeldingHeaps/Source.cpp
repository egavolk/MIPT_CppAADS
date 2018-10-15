#pragma comment(linker, "/STACK:1048576000")
#include "gtest\gtest.h"
#include "BinomialHeap.h"
#include "LeftistHeapTemplate.h"
#include "HeapTester.h"
#include "HeapTimeTester.h"
#include <vector>
#include <cstdio>
#include <ctime>

class HeapTest : public ::testing::Test
{
public:
protected:
	void SetUp()
	{
		tester = HeapTester(10, 4, 4, 2, 6, 100);
		tester.CheckTests();
	}
	void TearDown()
	{
		
	}
	HeapTester tester;
};

TEST_F(HeapTest, BinomialHeap)
{
	tester.RunTest < BinomialHeap >(); //RunTest подготавливает кучу к запуску теста
	//внутри вызывается MyTest, который тестирует кучу на каждом тесте и не знает реализации кучи
}

TEST_F(HeapTest, LeftistHeap)
{
	tester.RunTest < LeftistHeap >(); //RunTest подготавливает кучу к запуску теста
	//внутри вызывается MyTest, который тестирует кучу на каждом тесте и не знает реализации кучи
}

TEST_F(HeapTest, SkewHeap)
{
	tester.RunTest < SkewHeap >(); //RunTest подготавливает кучу к запуску теста
	//внутри вызывается MyTest, который тестирует кучу на каждом тесте и не знает реализации кучи
}


class TimeTest : public ::testing::Test
{
public:
protected:
	void SetUp()
	{
		tester = HeapTimeTester(1e6, 100);
		tester.CheckTimeTest();
	}
	void TearDown()
	{

	}
	HeapTimeTester tester;
};

TEST_F(TimeTest, BinomialHeap)
{
	tester.RunTimeTest < BinomialHeap > ();
}

TEST_F(TimeTest, LeftistHeap)
{
	tester.RunTimeTest < LeftistHeap >();
}

TEST_F(TimeTest, SkewHeap)
{
	tester.RunTimeTest < SkewHeap >();
}

int main(int argc, char *argv[])
{
	::testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
}
