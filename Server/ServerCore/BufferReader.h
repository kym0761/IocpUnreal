#pragma once
class FBufferReader
{
public:
	FBufferReader();
	FBufferReader(BYTE* buffer, uint32 size, uint32 pos = 0);
	~FBufferReader();

	BYTE* GetBuffer() { return Buffer; }
	uint32	GetSize() { return Size; }
	uint32	GetReadSize() { return Pos; }
	uint32	GetFreeSize() { return Size - Pos; }

	template<typename T>
	bool			Peek(T* dest) { return Peek(dest, sizeof(T)); }
	bool			Peek(void* dest, uint32 len);

	template<typename T>
	bool			Read(T* dest) { return Read(dest, sizeof(T)); }
	bool			Read(void* dest, uint32 len);

	template<typename T>
	FBufferReader& operator>>(OUT T& dest);

private:
	BYTE* Buffer = nullptr;
	uint32			Size = 0;
	uint32			Pos = 0;
};

template<typename T>
inline FBufferReader& FBufferReader::operator>>(OUT T& dest)
{
	dest = *reinterpret_cast<T*>(&Buffer[Pos]);
	Pos += sizeof(T); //커서를 T의 사이즈만큼 ++해주는 것
	return *this;
}