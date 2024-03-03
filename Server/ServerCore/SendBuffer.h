#pragma once

class FSendBufferChunk;

class FSendBuffer
{
public:
	//FSendBuffer(SendBufferChunkRef owner, BYTE* buffer, uint32 allocSize);
	FSendBuffer(int32 bufferSize);
	~FSendBuffer();

	BYTE* GetBuffer() { return Buffer.data(); }
	//uint32 GetAllocSize() { return AllocSize; }
	int32 GetWriteSize() { return WriteSize; }
	int32 GetCapacity() { return static_cast<int32>(Buffer.size()); }

	void CopyData(void* data, int32 len);
	void Close(uint32 writeSize);


private:
	
	vector<BYTE> Buffer;
	uint32 WriteSize = 0;
	
	//uint32				AllocSize = 0;
	//BYTE* Buffer; //이제 버퍼 위치만 알면 됨.
	//SendBufferChunkRef	Owner; // 사용하는 동안은 버퍼 청크가 삭제되지 않도록 버퍼가 레퍼런스 카운팅
};


////TLS 영역에서 동작하기 때문에 멀티쓰레드를 고려할 필요가 없음.(쓰레드마다 독립적으로 사용하므로)
//class FSendBufferChunk : public enable_shared_from_this<FSendBufferChunk>
//{
//	enum
//	{
//		SEND_BUFFER_CHUNK_SIZE = 6000
//	};
//
//public:
//	FSendBufferChunk();
//	~FSendBufferChunk();
//
//	void				Reset();
//	SendBufferRef		Open(uint32 allocSize);
//	void				Close(uint32 writeSize);
//
//	bool IsOpen() { return bOpen; }
//	BYTE* GetBuffer() { return &Buffer[UsedSize]; } //버퍼의 위치
//	uint32 GetFreeSize() { return static_cast<uint32>(Buffer.size()) - UsedSize; }
//
//private:
//	Array<BYTE, SEND_BUFFER_CHUNK_SIZE>		Buffer = {};
//	bool									bOpen = false;
//	uint32									UsedSize = 0;
//};
//
//
//class FSendBufferManager
//{
//public:
//	SendBufferRef		Open(uint32 size);
//
//private:
//	SendBufferChunkRef	Pop();
//	void				Push(SendBufferChunkRef buffer);
//
//	static void			PushGlobal(FSendBufferChunk* buffer);
//
//private:
//	USE_LOCK;
//	Vector<SendBufferChunkRef> SendBufferChunks;
//};
