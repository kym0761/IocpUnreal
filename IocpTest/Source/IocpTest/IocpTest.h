// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"


struct IOCPTEST_API FPacketHeader
{
	FPacketHeader() : size(0), id(0)
	{
	}

	FPacketHeader(uint16 PacketSize, uint16 PacketID)
		: size(PacketSize), id(PacketID)
	{
	}

	friend FArchive& operator<<(FArchive& Ar, FPacketHeader& Header)
	{
		Ar << Header.size;
		Ar << Header.id;
		return Ar;
	}

	/*순서 중요함*/
	uint16 size;
	uint16 id;
};

//struct FPacketHeader
//{
//	uint16 size;
//	uint16 id; // 프로토콜ID (ex. 1=로그인, 2=이동요청)
//};

class FSendBuffer : public TSharedFromThis<FSendBuffer>
{
public:
	FSendBuffer(int32 bufferSize);
	~FSendBuffer();

	BYTE* GetBuffer() { return Buffer.GetData(); }
	int32 GetWriteSize() { return WriteSize; }
	int32 GetCapacity() { return static_cast<int32>(Buffer.Num()); }

	void CopyData(void* data, int32 len);
	void Close(uint32 writeSize);

private:
	TArray<BYTE>	Buffer;
	int32			WriteSize = 0;
};

//Shared Ptr
#define USING_SHARED_PTR(name)	using name##Ref = TSharedPtr<class F##name>;

USING_SHARED_PTR(Session); // SessionRef = TSharedPTr<FSession>;
USING_SHARED_PTR(PacketSession);
USING_SHARED_PTR(SendBuffer);