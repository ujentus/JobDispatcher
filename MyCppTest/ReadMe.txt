## TODO


*job object pooling..

	static void	operator delete(void* obj)
	{
		uint64_t insPos = mTailPos.fetch_add(1);

		if (insPos - mHeadPos < QUEUE_MAX_SIZE)
			mPool[insPos & QUEUE_SIZE_MASK] = obj;
		else
			free(obj);
			
	}

	이 부분에서

	mPool[insPos & QUEUE_SIZE_MASK]이 nullptr일 경우에 obj를 넣는걸로 
	std:: compare_exchange_strong 쓰기.
