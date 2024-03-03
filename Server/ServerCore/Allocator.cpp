#include "pch.h"
#include "Allocator.h"
#include "Memory.h"

void* FBaseAllocator::Alloc(int32 size)
{
	return malloc(size);
}

void FBaseAllocator::Release(void* ptr)
{
	free(ptr);
}

//~~

void* FStompAllocator::Alloc(int32 size)
{
	//size가 4096보다 작을 때
	// size/4096을 하면 0이 나오게 됨
	// 4095를 더하면 올림처리되어 1이 됨 ... ex : (4095 + (size)) / 4096 = 1
	//그 외에도 size가 4096보다 크다면 초과분을 올림 처리함.
	const int64 pageCount = (size + PAGE_SIZE - 1) / PAGE_SIZE;
	
	//기존 [data ....] -> 개선[.... data]
	//페이지의 메모리 공간 뒤에 데이터를 배치하여 메모리 영역 침범 확인
	//예시) Knight* k = (knight*)new Player();
	//플레이어를 할당했는데, 페이지 뒤의 공간까지 배정받았으므로, HP 같은 변수가 나이트에 존재했다면
	//해당 위치는 사실 건드리면 안되지만 건드릴 수도 있게 되며 오류를 감지하지 못한다.
	const int64 dataOffset = pageCount * PAGE_SIZE - size;

	void* baseAddress =  VirtualAlloc(
		NULL,
		pageCount * PAGE_SIZE,
		MEM_RESERVE | MEM_COMMIT, // 예약 및 사용 허용
		PAGE_READWRITE); //해당 위치 읽기 쓰기 허용

	//구조 : [dataoffset|baseAddress]
	//dataoffset만큼 주소를 이동한 후, baseaddress를 return한다.
	//staticcast를 하는 이유는 1바이트씩 이동할 수 있게 만들기 위함이다.
	return static_cast<void*>(
		static_cast<int8*>(baseAddress) + dataOffset);

}

void FStompAllocator::Release(void* ptr)
{
	const int64 address = reinterpret_cast<int64>(ptr);
	const int64 baseAddress = address - (address % PAGE_SIZE); //address에 pagesize의 나머지만큼 빼면 할당 공간 위치로 이동함.

	VirtualFree(reinterpret_cast<void*>(baseAddress), 0, MEM_RELEASE);

}

//--

void* FPoolAllocator::Alloc(int32 size)
{
	//GMemory 비활성화 상태임.
	//return GMemory->Allocate(size);
	return nullptr;
}

void FPoolAllocator::Release(void* ptr)
{
	//GMemory 비활성화 상태임.
	//GMemory->Release(ptr);
}
