#include "pch.h"
#include "SendBuffer.h"

FSendBuffer::FSendBuffer(int32 bufferSize)
{
	Buffer.resize(bufferSize);
}

FSendBuffer::~FSendBuffer()
{
}

void FSendBuffer::CopyData(void* data, int32 len)
{
	assert(GetCapacity() >= len);
	::memcpy(Buffer.data(), data, len);
	WriteSize = len;
}

void FSendBuffer::Close(uint32 writeSize)
{
	WriteSize = writeSize;
}

//FSendBuffer::FSendBuffer(SendBufferChunkRef owner, BYTE* buffer, uint32 allocSize)
//	:  Owner(owner), Buffer(buffer), AllocSize(allocSize)
//{
//
//}

////~~
//
//FSendBufferChunk::FSendBufferChunk()
//{
//}
//
//FSendBufferChunk::~FSendBufferChunk()
//{
//}
//
//void FSendBufferChunk::Reset()
//{
//	//초기화
//	bOpen = false;
//	UsedSize = 0;
//}
//
//SendBufferRef FSendBufferChunk::Open(uint32 allocSize)
//{
//	ASSERT_CRASH(allocSize <= SEND_BUFFER_CHUNK_SIZE);
//	ASSERT_CRASH(bOpen == false);
//
//	if (allocSize > GetFreeSize())
//		return nullptr;
//
//	bOpen = true;
//	return FObjectPool<FSendBuffer>::MakeShared(
//		shared_from_this(),
//		GetBuffer(),
//		allocSize);
//}
//
//void FSendBufferChunk::Close(uint32 writeSize)
//{
//	ASSERT_CRASH(bOpen == true); //open된 상태가 아니면 못함.
//	bOpen = false;
//	UsedSize += writeSize; //사용한 위치까지의 커서를 이동
//}
//
//
//
////~
//
//SendBufferRef FSendBufferManager::Open(uint32 size)
//{
//	if (LSendBufferChunk == nullptr)
//	{
//		LSendBufferChunk = Pop(); // WRITE_LOCK
//		LSendBufferChunk->Reset();
//	}
//
//	ASSERT_CRASH(LSendBufferChunk->IsOpen() == false);
//
//	// 다 썼으면 버리고 새거로 교체
//	//새거로 교체해버리면 기존의 것은 어떻게 되는가?
//	//세션의 Send에서 레퍼런스 카운팅이 되어 있으므로
//	//Send가 완료되기 전까지는 유지되어있다가
//	//Send가 완료되면 PushGlobal로 반납한다.
//	if (LSendBufferChunk->GetFreeSize() < size)
//	{
//		LSendBufferChunk = Pop(); // WRITE_LOCK
//		LSendBufferChunk->Reset();
//	}
//
//	//cout << "FREE : " << LSendBufferChunk->GetFreeSize() << endl;
//
//	return LSendBufferChunk->Open(size);
//}
//
//SendBufferChunkRef FSendBufferManager::Pop()
//{
//	//cout << "Pop SENDBUFFERCHUNK" << endl;
//
//	{
//		WRITE_LOCK;
//		if (SendBufferChunks.empty() == false)
//		{
//			SendBufferChunkRef sendBufferChunk = SendBufferChunks.back();
//			SendBufferChunks.pop_back();
//			return sendBufferChunk;
//		}
//	}
//
//	//sendbufferChunk를 delete할 때 소멸자 대신에 PushGlobal을 동작하도록 만듬.
//	return SendBufferChunkRef(xnew<FSendBufferChunk>(), PushGlobal);
//}
//
//void FSendBufferManager::Push(SendBufferChunkRef buffer)
//{
//	WRITE_LOCK;
//	SendBufferChunks.push_back(buffer);
//}
//
//void FSendBufferManager::PushGlobal(FSendBufferChunk* buffer)
//{
//	//cout << "PushGlobal SENDBUFFERCHUNK" << endl;
//
//	GSendBufferManager->Push(SendBufferChunkRef(buffer, PushGlobal));
//}