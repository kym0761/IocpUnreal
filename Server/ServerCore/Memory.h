#pragma once
#include "Allocator.h"

class FMemoryPool;

class FMemory //메모리 풀 관리용 매니저
{
	enum
	{
		// ~1024까지 32단위, ~2048까지 128단위, ~4096까지 256단위
		POOL_COUNT = (1024 / 32) + (1024 / 128) + (2048 / 256),
		MAX_ALLOC_SIZE = 4096
	};

public:
	FMemory();
	~FMemory();

	void* Allocate(int32 size);
	void Release(void* ptr);

private:
	vector<FMemoryPool*> Pools;

	// 메모리 크기 <-> 메모리 풀
	// O(1) 빠르게 찾기 위한 테이블
	FMemoryPool* PoolTable[MAX_ALLOC_SIZE + 1];
};




template<typename Type, typename... Args>
Type* xnew(Args&&... args)
{

	Type* memory = static_cast<Type*>(FPoolAllocator::Alloc(sizeof(Type)));
	
	//생성자 호출
	new(memory)Type(forward<Args>(args)...);
	
	return memory;

}

template<typename Type>
void xdelete(Type* obj)
{
	//소멸자 호출
	obj->~Type();

	FPoolAllocator::Release(obj);
}

template<typename Type, typename... Args>
shared_ptr<Type> MakeShared(Args&&... args) // makeshared에서 parameter를 넘겨줄 수 있음.
{
	return shared_ptr<Type>{ xnew<Type>(forward<Args>(args)...), xdelete<Type> };
}