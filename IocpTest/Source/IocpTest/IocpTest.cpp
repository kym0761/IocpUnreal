// Copyright Epic Games, Inc. All Rights Reserved.

#include "IocpTest.h"
#include "Modules/ModuleManager.h"

IMPLEMENT_PRIMARY_GAME_MODULE( FDefaultGameModuleImpl, IocpTest, "IocpTest" );

FSendBuffer::FSendBuffer(int32 bufferSize)
{
	Buffer.SetNum(bufferSize);
}

FSendBuffer::~FSendBuffer()
{
}

void FSendBuffer::CopyData(void* data, int32 len)
{
	::memcpy(Buffer.GetData(), data, len);
	WriteSize = len;
}

void FSendBuffer::Close(uint32 writeSize)
{
	WriteSize = writeSize;
}
