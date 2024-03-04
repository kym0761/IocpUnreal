// Fill out your copyright notice in the Description page of Project Settings.


#include "PacketSession.h"
#include "NetworkWorker.h"
#include "Sockets.h"
#include "Common/TcpSocketBuilder.h"
#include "Serialization/ArrayWriter.h"
#include "SocketSubsystem.h"
#include "ClientPacketHandler.h"


FPacketSession::FPacketSession(class FSocket* Socket) : Socket(Socket)
{
	FClientPacketHandler::Init();
}

FPacketSession::~FPacketSession()
{
}

void FPacketSession::Run()
{
	//쓰레드 생성.
	//AsShared()로 자기 자신에 대한 스마트 포인터 레퍼런스 카운팅 증가
	RecvWorkerThread = MakeShared<FRecvWorker>(Socket, AsShared());
	SendWorkerThread = MakeShared<FSendWorker>(Socket, AsShared());
}

void FPacketSession::HandleRecvPackets()
{

	while (true)
	{
		TArray<uint8> Packet;
		if (RecvPacketQueue.Dequeue(OUT Packet) == false)
		{
			break;
		}
			
		PacketSessionRef ThisPtr = AsShared();
		FClientPacketHandler::HandlePacket(ThisPtr, Packet.GetData(), Packet.Num());
	}


}

void FPacketSession::SendPacket(SendBufferRef SendBuffer)
{
	SendPacketQueue.Enqueue(SendBuffer);
}

void FPacketSession::Disconnect()
{
	if (RecvWorkerThread)
	{
		RecvWorkerThread->Destroy();
		RecvWorkerThread = nullptr;
	}

	if (SendWorkerThread)
	{
		SendWorkerThread->Destroy();
		SendWorkerThread = nullptr;
	}
}
