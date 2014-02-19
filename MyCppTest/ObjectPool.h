#pragma once
#include <atomic>
#include <inttypes.h>

#define _ASSERT_CRASH(expr) \
	{ \
		if (!(expr)) \
		{ \
			int* dummy = 0; \
			*dummy = 0xDEADBEEF; \
		} \
	}



template <class T, int ALLOC_COUNT = 100>
class ObjectPool
{
public:
	template<int E>
	struct PowerOfTwo
	{
		enum { value = 2 * PowerOfTwo<E - 1>::value };
	};

	template<>
	struct PowerOfTwo<0>
	{
		enum { value = 1 };
	};

	enum
	{
		QUEUE_MAX_SIZE = PowerOfTwo<16>::value, ///< must be power of 2
		QUEUE_SIZE_MASK = QUEUE_MAX_SIZE - 1
	};


	static void* operator new(size_t objSize)
	{
		void* popVal = std::atomic_exchange(&mPool[mHeadPos & QUEUE_SIZE_MASK], nullptr);

		if (popVal != nullptr)
		{
			mHeadPos.fetch_add(1);
		}
		else
		{
			_ASSERT_CRASH(objSize == sizeof(T));
			popVal = malloc(objSize);
		}
			
		return popVal;
	}

	static void	operator delete(void* obj)
	{
		uint64_t insPos = mTailPos.fetch_add(1);

		_ASSERT_CRASH(insPos - mHeadPos < QUEUE_MAX_SIZE); ///< overflow

		mPool[insPos & QUEUE_SIZE_MASK] = obj;
	}


private:

	static std::atomic<void*>		mPool[QUEUE_MAX_SIZE];
	static std::atomic<uint64_t>	mHeadPos;
	static std::atomic<uint64_t>	mTailPos;

};

template <class T, int ALLOC_COUNT>
std::atomic<void*> ObjectPool<T, ALLOC_COUNT>::mPool[QUEUE_MAX_SIZE] = { 0, };

template <class T, int ALLOC_COUNT>
std::atomic<uint64_t>	ObjectPool<T, ALLOC_COUNT>::mHeadPos = 0;

template <class T, int ALLOC_COUNT>
std::atomic<uint64_t>	ObjectPool<T, ALLOC_COUNT>::mTailPos = 0;
