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

	�� �κп���

	mPool[insPos & QUEUE_SIZE_MASK]�� nullptr�� ��쿡 obj�� �ִ°ɷ� 
	std:: compare_exchange_strong ����.
