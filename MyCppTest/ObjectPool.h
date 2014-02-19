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


#include <windows.h>

template <class T>
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
		QUEUE_MAX_SIZE = PowerOfTwo<12>::value, ///< must be power of 2
		QUEUE_SIZE_MASK = QUEUE_MAX_SIZE - 1
	};

	static void PrepareAllocation()
	{
		for (int i = 0; i < QUEUE_MAX_SIZE; ++i)
			mPool[i] = malloc(sizeof(T));

		mTailPos.fetch_add(QUEUE_MAX_SIZE);
	}

	static void* operator new(size_t objSize)
	{
		uint64_t popPos = mHeadPos.fetch_add(1);

		void* popVal = std::atomic_exchange(&mPool[popPos & QUEUE_SIZE_MASK], nullptr);
		if (popVal != nullptr)
			return popVal;
	
		_ASSERT_CRASH(objSize == sizeof(T));

		return malloc(objSize);
	}

	static void	operator delete(void* obj)
	{
		uint64_t insPos = mTailPos.fetch_add(1);

		if (insPos - mHeadPos < QUEUE_MAX_SIZE)
			mPool[insPos & QUEUE_SIZE_MASK] = obj;
		else
			free(obj);
			
	}


private:

	static std::atomic<void*>		mPool[QUEUE_MAX_SIZE];
	static std::atomic<uint64_t>	mHeadPos;
	static std::atomic<uint64_t>	mTailPos;

};

template <class T>
std::atomic<void*> ObjectPool<T>::mPool[QUEUE_MAX_SIZE] = { 0, };

template <class T>
std::atomic<uint64_t> ObjectPool<T>::mHeadPos = 0;

template <class T>
std::atomic<uint64_t> ObjectPool<T>::mTailPos = 0;
