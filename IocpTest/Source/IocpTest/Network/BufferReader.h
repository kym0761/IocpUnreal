#pragma once


//언리얼에 이미 있는 클래스 명칭이라 다른 명칭으로 변경함.
//FBufferReader->FIocpBufferReader
class FIocpBufferReader
{
public:
	FIocpBufferReader();
	FIocpBufferReader(BYTE* buffer, uint32 size, uint32 pos = 0);
	~FIocpBufferReader();

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
	FIocpBufferReader& operator>>(OUT T& dest);

private:
	BYTE* Buffer = nullptr;
	uint32			Size = 0;
	uint32			Pos = 0;
};

//read operator
template<typename T>
inline FIocpBufferReader& FIocpBufferReader::operator>>(OUT T& dest)
{
	dest = *reinterpret_cast<T*>(&Buffer[Pos]);
	Pos += sizeof(T); //커서를 T의 사이즈만큼 ++해주는 것
	return *this;
}