#pragma once

#include <windows.h>
#include <deque>
#include <assert.h>
#include "Job.h"

class JobQueue
{
public:
	JobQueue() : mHead(&mStub), mTail(&mStub)
	{
		mOffset = reinterpret_cast<__int64>(&((reinterpret_cast<JobEntry*>(0))->mNodeEntry));
	}
	~JobQueue() {}

	/// mutiple produce
	void Push(JobEntry* newData)
	{
		NodeEntry* prevNode = (NodeEntry*)InterlockedExchangePointer((void**)&mHead, (void*)&(newData->mNodeEntry));
		prevNode->mNext = &(newData->mNodeEntry);
	}

	/// single consume
	JobEntry* Pop()
	{
		NodeEntry* tail = mTail;
		NodeEntry* next = tail->mNext;

		if (tail == &mStub)
		{
			/// 데이터가 없을 때
			if (nullptr == next)
				return nullptr;

			/// 처음 꺼낼 때
			mTail = next;
			tail = next;
			next = next->mNext;
		}

		/// 대부분의 경우에 데이터를 빼낼 때
		if (next)
		{
			mTail = next;

			return reinterpret_cast<JobEntry*>(reinterpret_cast<__int64>(tail)-mOffset);
		}

		NodeEntry* head = mHead;
		if (tail != head)
			return nullptr;

		/// 마지막 데이터 꺼낼 때
		mStub.mNext = nullptr;
		NodeEntry* prev = (NodeEntry*)InterlockedExchangePointer((void**)&mHead, (void*)&mStub);
		prev->mNext = &mStub;

		next = tail->mNext;
		if (next)
		{
			mTail = next;

			return reinterpret_cast<JobEntry*>(reinterpret_cast<__int64>(tail)-mOffset);
		}

		return nullptr;
	}


private:

	NodeEntry* volatile	mHead;
	NodeEntry*			mTail;
	NodeEntry			mStub;

	__int64				mOffset;

};


class JobDispatcher;

__declspec(thread) extern std::deque<JobDispatcher*>* LJobDispatcherList;

__declspec(thread) extern JobDispatcher*	LCurrentJobDispatcherOccupyingThisThread;


class JobDispatcher
{
public:

	JobDispatcher() : mRemainTaskCount(0), mRefCount(0) {}
	virtual ~JobDispatcher() 
	{
		assert(mRefCount == 0);
	}


	template <class T, class... Args>
	void DoAsync(void (T::*memfunc)(Args...), Args... args) 
	{ 
		Job<T, Args...>* job = new Job<T, Args...>(static_cast<T*>(this), memfunc, args...); 
		DoTask(job); 
	} 

	void AddRefForThis()
	{
		InterlockedIncrement(&mRefCount);
	}

	void ReleaseRefForThis()
	{
		InterlockedDecrement(&mRefCount);
	}

private:
	// Push a task into Job Queue, and then Execute tasks if possible
	void DoTask(JobEntry* task)
	{
		if (InterlockedIncrement64(&mRemainTaskCount) != 1)
		{
			// register the task in this dispatcher
			mJobQueue.Push(task);
		}
		else
		{
			// register the task in this dispatcher
			mJobQueue.Push(task);

			AddRefForThis(); ///< refcount +1 for this object

			// Does any dispathcer exist occupying this worker-thread at this moment?
			if (LCurrentJobDispatcherOccupyingThisThread != nullptr)
			{
				// just register this dispatcher in this worker-thread
				LJobDispatcherList->push_back(this);
			}
			else
			{
				// acquire
				LCurrentJobDispatcherOccupyingThisThread = this;

				// invokes all tasks of this dispatcher
				Flush();

				// invokes all tasks of other dispatchers registered in this thread
				while (!LJobDispatcherList->empty())
				{
					JobDispatcher* dispacher = LJobDispatcherList->front();
					LJobDispatcherList->pop_front();
					dispacher->Flush();
					dispacher->ReleaseRefForThis();
				}

				// release 
				LCurrentJobDispatcherOccupyingThisThread = nullptr;
				ReleaseRefForThis(); ///< refcount -1 for this object
			}
		}
	}

	// Execute all tasks registered in JobQueue of this dispatcher
	void Flush()
	{
		while ( true )
		{
			if (JobEntry* job = mJobQueue.Pop())
			{
				job->OnExecute();
				delete job;

				if ( InterlockedDecrement64(&mRemainTaskCount) == 0 )
					break;
			}
		}
	}


private:
	// member variables
	JobQueue	mJobQueue;

	volatile LONGLONG mRemainTaskCount;

	// should not release this object when it is in the dispatcher
	volatile long mRefCount;
};


