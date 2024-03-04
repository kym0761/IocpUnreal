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
}

bool FRecvWorker::ReceivePacket(TArray<uint8>& OutPacket)
{
	// 패킷 헤더 파싱
	const int32 HeaderSize = sizeof(FPacketHeader);
	TArray<uint8> HeaderBuffer;
	HeaderBuffer.AddZeroed(HeaderSize);

	if (ReceiveDesiredBytes(HeaderBuffer.GetData(), HeaderSize) == false)
		return false;

	// ID, Size 추출
	FPacketHeader Header;
	{
		FMemoryReader Reader(HeaderBuffer);
		Reader << Header;
		UE_LOG(LogTemp, Log, 
			TEXT("Recv PacketID : %d, PacketSize : %d"),
			Header.id, Header.size);
	}

	// 패킷 헤더 복사
	OutPacket = HeaderBuffer;


	// 패킷 내용 파싱
	TArray<uint8> PayloadBuffer;
	const int32 PayloadSize = Header.size - HeaderSize;
	OutPacket.AddZeroed(PayloadSize);

	if (ReceiveDesiredBytes(&OutPacket[HeaderSize], PayloadSize))
		return true;

	return false;


}

bool FRecvWorker::ReceiveDesiredBytes(uint8* Results, int32 Size)
{
	uint32 PendingDataSize;
	if (Socket->HasPendingData(PendingDataSize) == false || PendingDataSize <= 0)
		return false;

	int32 Offset = 0;

	while (Size > 0)
	{
		int32 NumRead = 0;
		Socket->Recv(Results + Offset, Size, OUT NumRead);
		check(NumRead <= Size);

		if (NumRead <= 0)
			return false;

		Offset += NumRead;
		Size -= NumRead;
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
		SendBufferRef SendBuffer;

		if (TSharedPtr<FPacketSession> session = WeakSessionRef.Pin())
		{
			if (session->SendPacketQueue.Dequeue(OUT SendBuffer))
			{
				SendPacket(SendBuffer);
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
		int32 BytesSent = 0;
		if (Socket->Send(Buffer, Size, BytesSent) == false)
			return false;

		Size -= BytesSent;
		Buffer += BytesSent;
	}

	return true;
}
