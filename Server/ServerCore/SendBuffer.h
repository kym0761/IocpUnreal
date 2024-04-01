#pragma once

class FSendBuffer
{
public:

	FSendBuffer(int32 bufferSize);
	~FSendBuffer();

	BYTE* GetBuffer() { return Buffer.data(); }
	int32 GetWriteSize() { return WriteSize; }
	int32 GetCapacity() { return static_cast<int32>(Buffer.size()); }

	void CopyData(void* data, int32 len);
	void Close(uint32 writeSize);


private:
	
	vector<BYTE> Buffer;
	uint32 WriteSize = 0;
	
};
