#include "stdafx.h"
#include "JobDispatcher.h"


__declspec(thread) std::deque<JobDispatcher*>* LJobDispatcherList = nullptr ;

__declspec(thread) JobDispatcher*	LCurrentJobDispatcherOccupyingThisThread = nullptr ;