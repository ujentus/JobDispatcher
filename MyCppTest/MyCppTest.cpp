// MyCppTest.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"


#include "JobDispatcher.h"

#include <vector>
#include <iostream>
#include <thread>


class TestObject
{
public:
	TestObject() : mTestCount(0)
	{}

	void TestFunc0()
	{
		++mTestCount;
		printf("[%d] TestFunc0 \n", mTestCount);
	}

	void TestFunc1(int b)
	{
		++mTestCount;
		printf("[%d] TestFunc1 %d\n", mTestCount, b);
	}

	void TestFunc2(double a, int b)
	{
		++mTestCount;
		printf("[%d] TestFunc2 %f\n", mTestCount, a + b);
	}


	int GetTestCount() { return mTestCount; }
private:

	int mTestCount;

	USE_OBJECT_BOUND_DISPATCHER;
};

TestObject* GTestObject[4] = {0,};

void TestWorkerThread(int tid)
{
	LJobDispatcherList = new std::deque<JobDispatcher*>;

	for (int i = 0; i < 10; ++i)
	{
		
		GTestObject[rand() % 4]->DoAsync(&TestObject::TestFunc2, double(tid)*100, i);
		GTestObject[rand() % 4]->DoAsync(&TestObject::TestFunc1, 100);
		GTestObject[rand() % 4]->DoAsync(&TestObject::TestFunc0);
	}

}

int _tmain(int argc, _TCHAR* argv[])
{
	for (int i = 0; i < 4; ++i)
		GTestObject[i] = new TestObject;

	std::vector<std::thread> threadList;

	for (int i = 0; i <4; ++i)
	{
		threadList.push_back(std::thread(TestWorkerThread, i+1));
	}

	
	for (auto& thread : threadList)
	{
		if (thread.joinable())
			thread.join();
	}

	int total = 0;
	for (int i = 0; i < 4; ++i)
		total += GTestObject[i]->GetTestCount();

	printf("TOTAL %d\n", total);

	getchar();
	return 0;
}
