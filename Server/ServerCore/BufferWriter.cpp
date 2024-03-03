#include "pch.h"
#include "BufferWriter.h"

FBufferWriter::FBufferWriter()
{
}

FBufferWriter::FBufferWriter(BYTE* buffer, uint32 size, uint32 pos)
	: Buffer(buffer), Size(size), Pos(pos)
{
}

FBufferWriter::~FBufferWriter()
{
}

bool FBufferWriter::Write(void* src, uint32 len)
{
	if (GetFreeSize() < len)
		return false;

	::memcpy(&Buffer[Pos], src, len);
	Pos += len;
	return true;
}
