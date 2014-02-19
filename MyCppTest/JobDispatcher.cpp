#include "stdafx.h"
#include "JobDispatcher.h"


thread_local std::deque<JobDispatcher*>* LJobDispatcherList = nullptr;
thread_local JobDispatcher*	LCurrentJobDispatcherOccupyingThisThread = nullptr;

