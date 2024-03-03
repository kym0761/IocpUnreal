#include "BufferWriter.h"

FIocpBufferWriter::FIocpBufferWriter()
{
}

FIocpBufferWriter::FIocpBufferWriter(BYTE* buffer, uint32 size, uint32 pos)
	: Buffer(buffer), Size(size), Pos(pos)
{
}

FIocpBufferWriter::~FIocpBufferWriter()
{
}

bool FIocpBufferWriter::Write(void* src, uint32 len)
{
	if (GetFreeSize() < len)
		return false;

	::memcpy(&Buffer[Pos], src, len);
	Pos += len;
	return true;
}
