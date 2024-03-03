#pragma once
#include "Types.h"
#include "MemoryPool.h"

//_STOMP로 StompAllocator가 활성했을 시의 코드를 선택 동작함.
//types.h에 있음

template<typename Type>
class FObjectPool
{
public:
	template<typename... Args>
	static Type* Pop(Args&&... args)
	{
#ifdef _STOMP
		FMemoryHeader* ptr = reinterpret_cast<FMemoryHeader*>(FStompAllocator::Alloc(s_AllocSize));
		Type* memory = static_cast<Type*>(FMemoryHeader::AttachHeader(ptr, s_AllocSize));
#else
		Type* memory = static_cast<Type*>(FMemoryHeader::AttachHeader(s_Pool.Pop(), s_AllocSize));
#endif		
		new(memory)Type(forward<Args>(args)...); // placement new
		return memory;
	}

	static void Push(Type* obj)
	{
		obj->~Type();
#ifdef _STOMP
		FStompAllocator::Release(FMemoryHeader::DetachHeader(obj));
#else
		s_Pool.Push(FMemoryHeader::DetachHeader(obj));
#endif
	}

	//shared_ptr을 사용할 때 객체를 얻을 때 / 객체를 지워야할 때를 지정하고 ptr을 넘기는 법
	// shared_ptr<T> p = ObjectPool<T>::MakeShared();
	template<typename... Args>
	static shared_ptr<Type> MakeShared(Args&&... args)
	{
		shared_ptr<Type> ptr = { Pop(forward<Args>(args)...), Push };
		return ptr;
	}

private:
	static int32		s_AllocSize;
	static FMemoryPool	s_Pool;
};

template<typename Type>
int32 FObjectPool<Type>::s_AllocSize = sizeof(Type) + sizeof(FMemoryHeader);

template<typename Type>
FMemoryPool FObjectPool<Type>::s_Pool{ s_AllocSize };
