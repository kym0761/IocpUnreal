#pragma once

#define OUT

/*-- Lock --*/

#define USE_MANY_LOCKS(count)	mutex Locks[count];
#define USE_LOCK				USE_MANY_LOCKS(1)
#define WRITE_LOCK_IDX(idx)		lock_guard<mutex> LockGuard_##idx(Locks[idx]);
#define WRITE_LOCK				WRITE_LOCK_IDX(0)


/*--------CRASH--------*/

#define CRASH(cause)						\
{											\
	uint32* crash = nullptr;				\
	__analysis_assume(crash != nullptr);	\
	*crash = 0XDEADBEEF;					\
} 

#define ASSERT_CRASH(expr)					\
{											\
	if (!(expr))							\
	{										\
		CRASH("ASSERT CRASH");				\
		__analysis_assume(expr);			\
	}										\
}

///*-namespace--*/
#define NAMESPACE_BEGIN(name)	namespace name {
#define NAMESPACE_END			}