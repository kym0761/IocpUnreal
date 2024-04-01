#pragma once
#include <stack>

//thread 각각이 보유한 thread local storage 변수다.

extern thread_local uint32 LThreadId; //Lock.h ThreadManager.h
extern thread_local std::stack<int32>	LLockStack; //DeadlockProfiler.h
extern thread_local class FJobQueue* LCurrentJobQueue; //이 쓰레드가 어떤 JobQueue를 실행중인지?
extern thread_local uint64				LEndTickCount;