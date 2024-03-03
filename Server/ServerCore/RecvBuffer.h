#pragma once

class FRecvBuffer
{

	enum { BUFFER_COUNT = 10 };

public:
	FRecvBuffer(int32 bufferSize);
	~FRecvBuffer();

	void			Clean();
	bool			OnRead(int32 numOfBytes);
	bool			OnWrite(int32 numOfBytes);

	BYTE* GetReadPos() { return &Buffer[ReadPos]; }
	BYTE* GetWritePos() { return &Buffer[WritePos]; }
	int32 GetDataSize() { return WritePos - ReadPos; }
	int32 GetFreeSize() { return Capacity - WritePos; }

private:
	int32			Capacity = 0;
	int32			BufferSize = 0;
	int32			ReadPos = 0;
	int32			WritePos = 0;
	vector<BYTE>	Buffer;
};

