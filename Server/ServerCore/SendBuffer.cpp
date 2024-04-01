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
