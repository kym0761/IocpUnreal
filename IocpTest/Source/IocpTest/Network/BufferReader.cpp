#include "BufferReader.h"

FIocpBufferReader::FIocpBufferReader()
{
}

FIocpBufferReader::FIocpBufferReader(BYTE* buffer, uint32 size, uint32 pos)
	: Buffer(buffer), Size(size), Pos(pos)
{
}

FIocpBufferReader::~FIocpBufferReader()
{
}

bool FIocpBufferReader::Peek(void* dest, uint32 len)
{
	
	if (GetFreeSize() < len)
		return false;

	::memcpy(dest, &Buffer[Pos], len);
	return true;
}

bool FIocpBufferReader::Read(void* dest, uint32 len)
{
	if (Peek(dest, len) == false)
		return false;

	Pos += len;
	return true;
}
