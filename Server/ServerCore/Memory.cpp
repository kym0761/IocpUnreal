#include "pch.h"
#include "Memory.h"
#include "MemoryPool.h"

FMemory::FMemory()
{
	int32 size = 0;
	int32 tableIndex = 0;

	//32~1024 짜리 사이즈 메모리 풀 Add
	for (size = 32; size <= 1024; size += 32)
	{
		FMemoryPool* pool = new FMemoryPool(size);
		Pools.push_back(pool);

		while (tableIndex <= size)
		{
			PoolTable[tableIndex] = pool;
			tableIndex++;
		}
	}

	//~~2048까지
	for (; size <= 2048; size += 128)
	{
		FMemoryPool* pool = new FMemoryPool(size);
		Pools.push_back(pool);

		while (tableIndex <= size)
		{
			PoolTable[tableIndex] = pool;
			tableIndex++;
		}
	}

	//~~4096까지
	for (; size <= 4096; size += 256)
	{
		FMemoryPool* pool = new FMemoryPool(size);
		Pools.push_back(pool);

		while (tableIndex <= size)
		{
			PoolTable[tableIndex] = pool;
			tableIndex++;
		}
	}
}

FMemory::~FMemory()
{
	for (FMemoryPool* pool : Pools)
		delete pool;

	Pools.clear();
}

void* FMemory::Allocate(int32 size)
{
	FMemoryHeader* header = nullptr;
	const int32 allocSize = size + sizeof(FMemoryHeader);

#ifdef _STOMP
	header = reinterpret_cast<FMemoryHeader*>(
		FStompAllocator::Alloc(allocSize));
#else
	if (allocSize > MAX_ALLOC_SIZE)
	{
		// 메모리 풀링 최대 크기를 벗어나면 일반 할당
		header = reinterpret_cast<FMemoryHeader*>(::_aligned_malloc(allocSize, SLIST_ALIGNMENT));
	}
	else
	{
		// 메모리 풀에서 꺼내온다
		header = PoolTable[allocSize]->Pop();
	}
#endif	

	return FMemoryHeader::AttachHeader(header, allocSize);
}

void FMemory::Release(void* ptr)
{
	FMemoryHeader* header = FMemoryHeader::DetachHeader(ptr);

	const int32 allocSize = header->AllocSize;
	ASSERT_CRASH(allocSize > 0);

#ifdef _STOMP
	FStompAllocator::Release(header);
#else
	if (allocSize > MAX_ALLOC_SIZE)
	{
		// 메모리 풀링 최대 크기를 벗어나면 일반 해제
		::_aligned_free(header);
	}
	else
	{
		// 메모리 풀에 반납한다
		PoolTable[allocSize]->Push(header);
	}
#endif	
}
