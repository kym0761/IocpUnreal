#include "pch.h"
#include "BufferReader.h"

FBufferReader::FBufferReader()
{
}

FBufferReader::FBufferReader(BYTE* buffer, uint32 size, uint32 pos)
	: Buffer(buffer), Size(size), Pos(pos)
{
}

FBufferReader::~FBufferReader()
{
}

bool FBufferReader::Peek(void* dest, uint32 len)
{
	
	if (GetFreeSize() < len)
		return false;

	::memcpy(dest, &Buffer[Pos], len);
	return true;
}

bool FBufferReader::Read(void* dest, uint32 len)
{
	if (Peek(dest, len) == false)
		return false;

	Pos += len;
	return true;
}
