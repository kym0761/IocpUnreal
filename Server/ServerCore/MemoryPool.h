#pragma once

enum
{
	SLIST_ALIGNMENT = 16
};

/*-----------------
	MemoryHeader
------------------*/

//16바이트 정렬
DECLSPEC_ALIGN(SLIST_ALIGNMENT)
struct FMemoryHeader : public SLIST_ENTRY //lockfreestack 기반으로 동작 가능한 SList_Entry
{
	// [MemoryHeader(할당한 크기가 얼마인지 등등..)][실제 Data]
	FMemoryHeader(int32 size) : AllocSize(size) { }

	static void* AttachHeader(FMemoryHeader* header, int32 size)
	{
		new(header)FMemoryHeader(size); // placement new
		return reinterpret_cast<void*>(++header); //실제 데이터 위치로 이동
	}

	static FMemoryHeader* DetachHeader(void* ptr)
	{
		//실제 데이터 주소에서 -1만큼 이동해 메모리 헤더로 이동.
		FMemoryHeader* header = reinterpret_cast<FMemoryHeader*>(ptr) - 1;
		return header;
	}

	int32 AllocSize;
	// TODO : 필요한 추가 정보
};

/*-----------------
	MemoryPool
------------------*/

//메모리 할당은 커널 모드 전환하여 요청하기 때문에 오버헤드가 발생할 가능성이 있음
//할당 해제할 메모리를 할당 해제하지 않고 따로 보관하여 필요할 때 다시 사용함
//요즘 서버에서 굳이 사용하지는 않는다고는 함.
//이유는 C++ new 의 성능이 과거에 비해 매우 좋아졌다고 함.

DECLSPEC_ALIGN(SLIST_ALIGNMENT)
class FMemoryPool
{
public:
	FMemoryPool(int32 allocSize);
	~FMemoryPool();

	void Push(FMemoryHeader* ptr);
	FMemoryHeader* Pop();

private:
	SLIST_HEADER	Header;
	int32			AllocSize = 0;
	atomic<int32>	UseCount = 0;
	atomic<int32>	ReserveCount = 0;
};

