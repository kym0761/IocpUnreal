#pragma once

class FBaseAllocator
{
public:

	static void* Alloc(int32 size);
	static void Release(void* ptr);

};


//메모리 할당 해제 후 사용 가능한 상황을 방지하고자 사용함 (use after free)
//page 단위로 데이터를 큰 공간에 배정하여 데이터 오염을 감지 및 방지
//실제로 어떤 주소에 공간을 배정하면 가상메모리를 사용하기 때문에 물리 메모리 주소와는 다름
// 이 점으로 virtualAlloc, virtualFree를 사용함.
class FStompAllocator
{

	enum { PAGE_SIZE = 0x1000 }; //4096 = 4kb

public:

	static void* Alloc(int32 size);
	static void Release(void* ptr);
};

//pool allocation
class FPoolAllocator
{
public:
	static void* Alloc(int32 size);
	static void	Release(void* ptr);
};

//STL에 넣을 수 있는 Allocator 예시
template<typename T>
class STL_Allocator
{
public:

	using value_type = T;

	STL_Allocator()
	{
	}

	template<typename Other>
	STL_Allocator(const STL_Allocator<Other>&)
	{
	}

	/* !! 아래 함수들 명칭 첫글자는 소문자여야함.*/
	T* allocate(size_t count)
	{
		const int32 size = static_cast<int32>(count * sizeof(T));
		return static_cast<T*>(FPoolAllocator::Alloc(size));
	}

	void deallocate(T* ptr, size_t count)
	{
		FPoolAllocator::Release(ptr);
	}

};