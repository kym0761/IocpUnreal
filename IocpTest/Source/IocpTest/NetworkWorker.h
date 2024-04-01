// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Containers/Queue.h"
#include "IocpTest.h"

class FSocket;

/**
 * 
 */
class IOCPTEST_API FRecvWorker : public FRunnable
{
public:
	FRecvWorker(FSocket* Socket, TSharedPtr<class FPacketSession> Session);
	~FRecvWorker();

	virtual bool Init() override;
	virtual uint32 Run() override;
	virtual void Exit() override;

	void Destroy();

private:
	bool ReceivePacket(TArray<uint8>& OutPacket);
	bool ReceiveDesiredBytes(uint8* Results, int32 Size);

protected:
	FRunnableThread* Thread = nullptr;
	bool bRunning = true;
	FSocket* Socket;
	TWeakPtr<class FPacketSession> WeakSessionRef;
};

class IOCPTEST_API FSendWorker : public FRunnable
{
public:
	FSendWorker(FSocket* Socket, TSharedPtr<class FPacketSession> Session);
	~FSendWorker();

	virtual bool Init() override;
	virtual uint32 Run() override;
	virtual void Exit() override;

	bool SendPacket(SendBufferRef SendBuffer);

	void Destroy();

private:
	bool SendDesiredBytes(const uint8* Buffer, int32 Size);

protected:
	FRunnableThread* Thread = nullptr;
	bool bRunning = true;
	FSocket* Socket;
	TWeakPtr<class FPacketSession> WeakSessionRef;
};
