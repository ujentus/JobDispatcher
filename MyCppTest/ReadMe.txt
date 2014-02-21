## WARNING!

1. you shoud use x64 mode due to std::atomic variable alignment
2. be careful! when using blockable LOCKs in an async job... (which can cause deadlock.. )

## TODO


http://en.cppreference.com/w/cpp/memory/enable_shared_from_this

class JobDispatcher : std::enable_shared_from_this<JobDispatcher>

DoAsync 안에서 shared_from_this()로 넘기기?

Job안의 ObjType*이 shared_ptr이 되어야 함.

--> mRefCount 제거 가능
