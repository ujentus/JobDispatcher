## WARNING!

1. you shoud use x64 mode due to std::atomic variable alignment
2. be careful! when using blockable LOCKs in an async job... (which can cause deadlock.. )

## TODO


http://en.cppreference.com/w/cpp/memory/enable_shared_from_this

class JobDispatcher : std::enable_shared_from_this<JobDispatcher>

DoAsync �ȿ��� shared_from_this()�� �ѱ��?

Job���� ObjType*�� shared_ptr�� �Ǿ�� ��.

--> mRefCount ���� ����
