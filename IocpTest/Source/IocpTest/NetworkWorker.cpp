// Fill out your copyright notice in the Description page of Project Settings.


#include "NetworkWorker.h"
#include "Sockets.h"
#include "Serialization/ArrayWriter.h"
#include "PacketSession.h"


FRecvWorker::FRecvWorker(FSocket* Socket, TSharedPtr<class FPacketSession> Session)
	: Socket(Socket) , WeakSessionRef(Session)
{
	Thread = FRunnableThread::Create(this, TEXT("RecvWorkerThread"));
}

FRecvWorker::~FRecvWorker()
{

}

bool FRecvWorker::Init()
{
	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, FString::Printf(TEXT("Recv Thread Init")));

	return true;
}

uint32 FRecvWorker::Run()
{
	while (bRunning)
	{
		TArray<uint8> Packet;

		if (ReceivePacket(OUT Packet))
		{
			//Tshared로 변환하면서, 세션은 사용하는 동안은 절대 레퍼런스 해제가 되지 않을 것이다.
			if (TSharedPtr<FPacketSession> session = WeakSessionRef.Pin())
			{
				session->RecvPacketQueue.Enqueue(Packet);
			}
		}
	}

	return 0;
}

void FRecvWorker::Exit()
{
}

void FRecvWorker::Destroy()
{
	bRunning = false;
}

bool FRecvWorker::ReceivePacket(TArray<uint8>& OutPacket)
{
	// 패킷 헤더 파싱
	const int32 headerSize = sizeof(FPacketHeader);
	TArray<uint8> headerBuffer;
	headerBuffer.AddZeroed(headerSize);

	if (ReceiveDesiredBytes(headerBuffer.GetData(), headerSize) == false)
	{
		return false;
	}

	// ID, Size 추출
	FPacketHeader header;
	{
		FMemoryReader reader(headerBuffer);
		reader << header;
		UE_LOG(LogTemp, Log, 
			TEXT("Recv PacketID : %d, PacketSize : %d"),
			header.id, header.size);
	}

	// 패킷 헤더 복사
	OutPacket = headerBuffer;


	// 패킷 내용 파싱
	TArray<uint8> payloadBuffer;
	const int32 payloadSize = header.size - headerSize;

	//내용물이 하나도 안들어있으면, 아래에 그 이후 부분을 접근하려는 오류를 냄
	//그러니 0일 때 예외처리 필요함.
	if (payloadSize == 0)
	{
		return true;
	}
	
	OutPacket.AddZeroed(payloadSize);

	if (ReceiveDesiredBytes(&OutPacket[headerSize], payloadSize))
	{
		return true;
	}
		
	return false;
}

bool FRecvWorker::ReceiveDesiredBytes(uint8* Results, int32 Size)
{
	uint32 pendingDataSize; //pending중인 데이터 사이즈

	if (Socket->HasPendingData(pendingDataSize) == false || pendingDataSize <= 0)
	{
		return false;
	}

	int32 offset = 0;

	while (Size > 0)
	{
		int32 numRead = 0;//얼마나 읽었는가?

		Socket->Recv(Results + offset, Size, OUT numRead);
		check(numRead <= Size);

		if (numRead <= 0)
		{
			return false;
		}
		
		offset += numRead;
		Size -= numRead;
	}

	return true;
}

//~~

FSendWorker::FSendWorker(FSocket* Socket, TSharedPtr<class FPacketSession> Session)
	: Socket(Socket), WeakSessionRef(Session)
{
	Thread = FRunnableThread::Create(this, TEXT("SendWorkerThread"));
}

FSendWorker::~FSendWorker()
{
}

bool FSendWorker::Init()
{
	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, FString::Printf(TEXT("Send Thread Init")));

	return true;
}

uint32 FSendWorker::Run()
{
	while (bRunning)
	{
		SendBufferRef sendBuffer;

		if (TSharedPtr<FPacketSession> session = WeakSessionRef.Pin())
		{
			if (session->SendPacketQueue.Dequeue(OUT sendBuffer))
			{
				SendPacket(sendBuffer);
			}
		}

		// Sleep?
	}

	return 0;
}

void FSendWorker::Exit()
{
}

bool FSendWorker::SendPacket(SendBufferRef SendBuffer)
{
	if (SendDesiredBytes(SendBuffer->GetBuffer(), SendBuffer->GetWriteSize()) == false)
		return false;

	return true;
}

void FSendWorker::Destroy()
{
	bRunning = false;
}

bool FSendWorker::SendDesiredBytes(const uint8* Buffer, int32 Size)
{
	while (Size > 0)
	{
		int32 bytesSent = 0; //얼마나 보냈는가?

		if (Socket->Send(Buffer, Size, bytesSent) == false)
			return false;

		Size -= bytesSent;
		Buffer += bytesSent;
	}

	return true;
}
