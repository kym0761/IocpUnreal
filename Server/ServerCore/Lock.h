#pragma once

#include "Types.h"

//read & write spinlock

class FLock
{
	enum :uint32
	{
		ACQUIRE_TIMEOUT_TICK = 10000,
		MAX_SPIN_COUNT = 5000,
		WRITE_THREAD_MASK = 0xFFFF'0000,
		READ_COUNT_MASK = 0x0000'FFFF,
		EMPTY_FLAG = 0x0000'0000
	};

public:

	void WriteLock(const char* Name);
	void WriteUnlock(const char* Name);
	void ReadLock(const char* Name);
	void ReadUnlock(const char* Name);



private:

	//구조 : [w 16bit][r 16bit]
	//w : write를 사용하는 중인 threadid
	//r : read count
	Atomic<uint32> LockFlag = EMPTY_FLAG;
	uint16 WriteCount = 0;

};

class FReadLockGuard
{
public:

	FReadLockGuard(FLock& Lock, const char* NameVal) : LockRef(Lock), Name(NameVal)
	{
		LockRef.ReadLock(Name);
	}

	~FReadLockGuard()
	{
		LockRef.ReadUnlock(Name);
	}

private:

	FLock& LockRef;
	const char* Name;
};

class FWriteLockGuard
{
public:

	FWriteLockGuard(FLock& Lock, const char* NameVal) : LockRef(Lock), Name(NameVal)
	{
		LockRef.WriteLock(Name);
	}

	~FWriteLockGuard()
	{
		LockRef.WriteUnlock(Name);
	}

private:

	FLock& LockRef;
	const char* Name;
};