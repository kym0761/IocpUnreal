#include "pch.h"
#include "MemoryPool.h"

FMemoryPool::FMemoryPool(int32 allocSize) : AllocSize(allocSize)
{
	InitializeSListHead(&Header);
}

FMemoryPool::~FMemoryPool()
{
	//lockfreestack 기반 interlocked Pop
	while (FMemoryHeader* memory = 
		static_cast<FMemoryHeader*>(InterlockedPopEntrySList(&Header)))
	{
		_aligned_free(memory); // 실제 메모리 반환
	}
}

void FMemoryPool::Push(FMemoryHeader* ptr)
{
	//메모리 반납

	ptr->AllocSize = 0;

	//lockfreestack 기반 interlocked Push
	InterlockedPushEntrySList(&Header, static_cast<PSLIST_ENTRY>(ptr));

	UseCount.fetch_sub(1);
	ReserveCount.fetch_add(1);
}

//~~

FMemoryHeader* FMemoryPool::Pop()
{
	//메모리 할당

	//lockfreestack 기반 interlocked Pop
	FMemoryHeader* memory = static_cast<FMemoryHeader*>(
		InterlockedPopEntrySList(&Header));

	// 없으면 새로 만든다
	if (memory == nullptr)
	{
		memory = reinterpret_cast<FMemoryHeader*>(
			_aligned_malloc(AllocSize, SLIST_ALIGNMENT)); //실제 메모리 할당
	}
	else
	{
		ASSERT_CRASH(memory->AllocSize == 0);
		ReserveCount.fetch_sub(1);
	}

	UseCount.fetch_add(1);

	return memory;
}
