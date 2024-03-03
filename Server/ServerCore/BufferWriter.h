#pragma once
class FBufferWriter
{
public:
	FBufferWriter();
	FBufferWriter(BYTE* buffer, uint32 size, uint32 pos = 0);
	~FBufferWriter();

	BYTE* GetBuffer() { return Buffer; }
	uint32 GetSize() { return Size; }
	uint32 GetWriteSize() { return Pos; }
	uint32 GetFreeSize() { return Size - Pos; }

	template<typename T>
	bool Write(T* src) { return Write(src, sizeof(T)); }
	bool Write(void* src, uint32 len);

	template<typename T>
	T* Reserve(uint16 count = 1);

	template<typename T>
	FBufferWriter& operator<<(T&& src);

private:
	BYTE* Buffer = nullptr;
	uint32 Size = 0;
	uint32 Pos = 0;
};

template<typename T>
T* FBufferWriter::Reserve(uint16 count)
{
	if (GetFreeSize() < (sizeof(T) * count))
		return nullptr;

	T* ret = reinterpret_cast<T*>(&Buffer[Pos]);
	Pos += (sizeof(T) * count);
	return ret;
}

template<typename T>
FBufferWriter& FBufferWriter::operator<<(T&& src) //보편 참조
{
	using DataType = std::remove_reference_t<T>; // 참조 제거 예시) const int&에서 Ref를 빼서 const int로 만듬.
	*reinterpret_cast<DataType*>(&Buffer[Pos]) = std::forward<DataType>(src);
	Pos += sizeof(T);
	return *this;
}