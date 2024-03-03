#include "pch.h"
#include "RecvBuffer.h"

FRecvBuffer::FRecvBuffer(int32 bufferSize) : BufferSize(bufferSize)
{
	//의도한 버퍼크기보다 더 크게 버퍼의 크기를 세팅한다.
	//이렇게 한다면, 데이터를 계속 읽고 쓰고 반복할 때 R위치와 W위치가 겹쳐서 같아질 확률이 크다
	//이러면 clean에서 R W = 0으로 세팅할텐데 memcpy를 쓰는 성능저하를 최대한 피할 수 있다.
	Capacity = bufferSize * BUFFER_COUNT;
	Buffer.resize(Capacity);

}

FRecvBuffer::~FRecvBuffer()
{
}

void FRecvBuffer::Clean()
{
	int32 dataSize = GetDataSize();
	if (dataSize == 0)
	{
		// 읽기+쓰기 커서가 동일한 위치라면, 둘 다 0으로 리셋
		ReadPos = WritePos = 0;
	}
	else
	{
		// 여유 공간이 버퍼 1개 크기 미만이면, 데이터를 앞으로 땅긴다.
		if (GetFreeSize() < BufferSize)
		{
			::memcpy(&Buffer[0], &Buffer[ReadPos], dataSize);
			ReadPos = 0;
			WritePos = dataSize;
		}
	}
}

bool FRecvBuffer::OnRead(int32 numOfBytes)
{
	//얻어올 데이터가 데이터 사이즈보다 크면 X 
	if (numOfBytes > GetDataSize())
		return false;


	//ReadPos ++
	ReadPos += numOfBytes;
	return true;
}

bool FRecvBuffer::OnWrite(int32 numOfBytes)
{
	//여유 공간보다 써야할 데이터가 크면 X
	if (numOfBytes > GetFreeSize())
		return false;

	//WritePos ++
	WritePos += numOfBytes;
	return true;
}
