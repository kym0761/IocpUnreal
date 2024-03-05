// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "IocpTest.h"

/**
 * 
 */
class IOCPTEST_API FPacketSession 
	: public TSharedFromThis<FPacketSession>
{
public:
	FPacketSession(class FSocket* Socket);
	~FPacketSession();

	void Run();

	UFUNCTION(BlueprintCallable)
	void HandleRecvPackets();

	void SendPacket(SendBufferRef SendBuffer);

	void Disconnect();

public:
	class FSocket* Socket;

	/*실제로 Send Receive를 할 스레드*/
	TSharedPtr<class FRecvWorker> RecvWorkerThread;
	TSharedPtr<class FSendWorker> SendWorkerThread;

	// GameThread와 NetworkThread가 데이터 주고 받는 공용 큐.
	//TQueue는 Thread Safe다.
	TQueue<TArray<uint8>> RecvPacketQueue;
	TQueue<SendBufferRef> SendPacketQueue;
};
