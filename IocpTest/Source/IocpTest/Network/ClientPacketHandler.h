#pragma once

#include "Protocol.pb.h"

#if UE_BUILD_DEBUG + UE_BUILD_DEVELOPMENT + UE_BUILD_TEST + UE_BUILD_SHIPPING >= 1
#include "IocpTest.h"
#endif

using PacketHandlerFunc = std::function<bool(PacketSessionRef&, BYTE*, int32)>; //�Լ� ������
extern PacketHandlerFunc GPacketHandler[UINT16_MAX]; //�ʿ������� �𸣴� �Լ� �����͵��� �̸� �����.

// ��Ŷ enum �ڵ�ȭ
enum : uint16
{
	PKT_C2S_LOGIN = 1000,
	PKT_S2C_LOGIN = 1001,
	PKT_C2S_ENTER_GAME = 1002,
	PKT_S2C_ENTER_GAME = 1003,
	PKT_C2S_LEAVE_GAME = 1004,
	PKT_S2C_LEAVE_GAME = 1005,
	PKT_S2C_SPAWN = 1006,
	PKT_S2C_DESPAWN = 1007,
	PKT_C2S_MOVE = 1008,
	PKT_S2C_MOVE = 1009,
	PKT_C2S_JUMP = 1010,
	PKT_S2C_JUMP = 1011,
	PKT_C2S_CHAT = 1012,
	PKT_S2C_CHAT = 1013,
};



// Custom Handlers

//INVALID�� ������ ������ ���� �ӽ� �Լ���.
bool Handle_INVALID(PacketSessionRef& session, BYTE* buffer, int32 len);

//���� �ڵ�ȭ
//�� �Լ� ������ ����ڰ� ���� ������ ��.
// ����? �ڵ�ȭ �ý����� �� �Լ��� � ����� ���� �� �� ����..
bool Handle_S2C_LOGIN(PacketSessionRef& session, Protocol::S2C_LOGIN& pkt);
bool Handle_S2C_ENTER_GAME(PacketSessionRef& session, Protocol::S2C_ENTER_GAME& pkt);
bool Handle_S2C_LEAVE_GAME(PacketSessionRef& session, Protocol::S2C_LEAVE_GAME& pkt);
bool Handle_S2C_SPAWN(PacketSessionRef& session, Protocol::S2C_SPAWN& pkt);
bool Handle_S2C_DESPAWN(PacketSessionRef& session, Protocol::S2C_DESPAWN& pkt);
bool Handle_S2C_MOVE(PacketSessionRef& session, Protocol::S2C_MOVE& pkt);
bool Handle_S2C_JUMP(PacketSessionRef& session, Protocol::S2C_JUMP& pkt);
bool Handle_S2C_CHAT(PacketSessionRef& session, Protocol::S2C_CHAT& pkt);


class FClientPacketHandler
{
public:

	static void Init()
	{
		for (int32 i = 0; i < UINT16_MAX; i++)
		{
			GPacketHandler[i] = Handle_INVALID; //�Լ��� �ϴ� HANDLE INVALID�� ���
		}

		//��Ŷ�� �þ ������ �߰��ϴ� �ڵ�ȭ ��ġ.
		//PKT_S_TEST�� ���� �Լ� ���
		GPacketHandler[PKT_S2C_LOGIN] = 
			[](PacketSessionRef& session, BYTE* buffer, int32 len) 
			{
				return HandlePacket<Protocol::S2C_LOGIN>(Handle_S2C_LOGIN, session, buffer, len); 
			};
		GPacketHandler[PKT_S2C_ENTER_GAME] = 
			[](PacketSessionRef& session, BYTE* buffer, int32 len) 
			{
				return HandlePacket<Protocol::S2C_ENTER_GAME>(Handle_S2C_ENTER_GAME, session, buffer, len); 
			};
		GPacketHandler[PKT_S2C_LEAVE_GAME] = 
			[](PacketSessionRef& session, BYTE* buffer, int32 len) 
			{
				return HandlePacket<Protocol::S2C_LEAVE_GAME>(Handle_S2C_LEAVE_GAME, session, buffer, len); 
			};
		GPacketHandler[PKT_S2C_SPAWN] = 
			[](PacketSessionRef& session, BYTE* buffer, int32 len) 
			{
				return HandlePacket<Protocol::S2C_SPAWN>(Handle_S2C_SPAWN, session, buffer, len); 
			};
		GPacketHandler[PKT_S2C_DESPAWN] = 
			[](PacketSessionRef& session, BYTE* buffer, int32 len) 
			{
				return HandlePacket<Protocol::S2C_DESPAWN>(Handle_S2C_DESPAWN, session, buffer, len); 
			};
		GPacketHandler[PKT_S2C_MOVE] = 
			[](PacketSessionRef& session, BYTE* buffer, int32 len) 
			{
				return HandlePacket<Protocol::S2C_MOVE>(Handle_S2C_MOVE, session, buffer, len); 
			};
		GPacketHandler[PKT_S2C_JUMP] = 
			[](PacketSessionRef& session, BYTE* buffer, int32 len) 
			{
				return HandlePacket<Protocol::S2C_JUMP>(Handle_S2C_JUMP, session, buffer, len); 
			};
		GPacketHandler[PKT_S2C_CHAT] = 
			[](PacketSessionRef& session, BYTE* buffer, int32 len) 
			{
				return HandlePacket<Protocol::S2C_CHAT>(Handle_S2C_CHAT, session, buffer, len); 
			};
	}

	//� Ŭ���̾�Ʈ�� ��Ŷ�� ���� ������ Ȯ���ϱ� ���� sessionref�� �޴´�.
	static bool HandlePacket(PacketSessionRef& session, BYTE* buffer, int32 len)
	{
		FPacketHeader* header = reinterpret_cast<FPacketHeader*>(buffer);

		//��� id�� �´� ��Ŷ handle �Լ��� ȣ��
		//�̻��� id..���� ��� �ƿ� �������� �ʴ� id���
		//���ʿ� HANDLE_INVALID�� ���Ƴ����� �������.
		return GPacketHandler[header->id](session, buffer, len);
	}

	// MakeSendBuffer �ڵ�ȭ
	static SendBufferRef MakeSendBuffer(Protocol::C2S_LOGIN& pkt)
	{
		return MakeSendBuffer(pkt, PKT_C2S_LOGIN); 
	}
	static SendBufferRef MakeSendBuffer(Protocol::C2S_ENTER_GAME& pkt)
	{
		return MakeSendBuffer(pkt, PKT_C2S_ENTER_GAME); 
	}
	static SendBufferRef MakeSendBuffer(Protocol::C2S_LEAVE_GAME& pkt)
	{
		return MakeSendBuffer(pkt, PKT_C2S_LEAVE_GAME); 
	}
	static SendBufferRef MakeSendBuffer(Protocol::C2S_MOVE& pkt)
	{
		return MakeSendBuffer(pkt, PKT_C2S_MOVE); 
	}
	static SendBufferRef MakeSendBuffer(Protocol::C2S_JUMP& pkt)
	{
		return MakeSendBuffer(pkt, PKT_C2S_JUMP); 
	}
	static SendBufferRef MakeSendBuffer(Protocol::C2S_CHAT& pkt)
	{
		return MakeSendBuffer(pkt, PKT_C2S_CHAT); 
	}

private:

	template<typename PacketType, typename ProcessFunc>
	static bool HandlePacket(ProcessFunc func, PacketSessionRef& session, BYTE* buffer, int32 len)
	{
		//��Ŷ�� �޾��ִ� ����.
		PacketType pkt;
		if (pkt.ParseFromArray(
			buffer + sizeof(FPacketHeader),
			len - sizeof(FPacketHeader)) == false)
		{
			return false;
		}

		//���� ��Ŷ�� func�� ���۽�Ŵ.
		return func(session, pkt);
	}

	template<typename T>
	static SendBufferRef MakeSendBuffer(T& pkt, uint16 pktId)
	{
		//����ȭ �ڵ�

		const uint16 dataSize = static_cast<uint16>(pkt.ByteSizeLong());
		const uint16 packetSize = dataSize + sizeof(FPacketHeader);

		//if�� �𸮾� �������� ���� .. else�� c++ iocp �������� ����
#if UE_BUILD_DEBUG + UE_BUILD_DEVELOPMENT + UE_BUILD_TEST + UE_BUILD_SHIPPING >= 1
		SendBufferRef sendBuffer = MakeShared<FSendBuffer>(packetSize);
#else
		SendBufferRef sendBuffer = make_shared<FSendBuffer>(packetSize);
#endif

		FPacketHeader* header = reinterpret_cast<FPacketHeader*>(sendBuffer->GetBuffer());
		header->size = packetSize;
		header->id = pktId;
		pkt.SerializeToArray(&header[1], dataSize);
		sendBuffer->Close(packetSize);

		return sendBuffer;
	}
};