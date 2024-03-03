#pragma once

#define OUT

/*-- Lock --*/

//#define USE_MANY_LOCKS(count)	FLock Locks[count];
//#define USE_LOCK				USE_MANY_LOCKS(1)
//#define READ_LOCK_IDX(idx)		FReadLockGuard ReadLockGuard_##idx(Locks[idx], typeid(this).name());
//#define READ_LOCK				READ_LOCK_IDX(0)
//#define WRITE_LOCK_IDX(idx)		FWriteLockGuard WriteLockGuard_##idx(Locks[idx], typeid(this).name());
//#define WRITE_LOCK				WRITE_LOCK_IDX(0)


#define USE_MANY_LOCKS(count)	mutex Locks[count];
#define USE_LOCK				USE_MANY_LOCKS(1)
//#define READ_LOCK_IDX(idx)		FReadLockGuard ReadLockGuard_##idx(Locks[idx], typeid(this).name());
//#define READ_LOCK				READ_LOCK_IDX(0)
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

///*--Memory--*/
//
//#ifdef _DEBUG
//#define nxalloc(size) FStompAllocator::Alloc(size)
//#define nxrelease(ptr) FStompAllocator::Release(ptr);
//#else
//#define Xalloc(size) FBaseAllocator::Alloc(size)
//#define xRelease(ptr) FBaseAllocator::Release(ptr);
//#endif // _DEBUG

#define NAMESPACE_BEGIN(name)	namespace name {
#define NAMESPACE_END			}