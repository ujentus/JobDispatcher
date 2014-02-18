#pragma once



template <class TOBJECT, int ALLOC_COUNT = 100>
class ObjectPool
{
public:

	
	static void* operator new(size_t objSize)
	{
		return malloc(objSize);
	}

	static void	operator delete(void* obj)
	{
		free(obj);
	}


private:
	static uint8_t*	mFreeList;
	static int		mTotalAllocCount; ///< for tracing
	static int		mCurrentUseCount; ///< for tracing

};


template <class TOBJECT, int ALLOC_COUNT>
uint8_t* ObjectPool<TOBJECT, ALLOC_COUNT>::mFreeList = nullptr;

template <class TOBJECT, int ALLOC_COUNT>
int ObjectPool<TOBJECT, ALLOC_COUNT>::mTotalAllocCount = 0;

template <class TOBJECT, int ALLOC_COUNT>
int ObjectPool<TOBJECT, ALLOC_COUNT>::mCurrentUseCount = 0;


