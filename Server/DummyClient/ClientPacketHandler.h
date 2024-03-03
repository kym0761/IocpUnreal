#pragma once

#include "Protocol.pb.h"

using PacketHandlerFunc = std::function<bool(PacketSessionRef&, BYTE*, int32)>; //�Լ� ������
extern PacketHandlerFunc GPacketHandler[UINT16_MAX]; //�ʿ������� �𸣴� �Լ� �����͵��� �̸� �����.

// TODO : �ڵ�ȭ
enum : uint16
{
	PKT_C_LOGIN = 1000,
	PKT_S_LOGIN = 1001,
	PKT_C_ENTER_GAME = 1002,
	PKT_S_ENTER_GAME = 1003,
	PKT_C_CHAT = 1004,
	PKT_S_CHAT = 1005,
};



// Custom Handlers
//�� �Լ� ������ ����ڰ� ���� ������ ��.
// ����? �ڵ�ȭ �ý����� �� �Լ��� � ����� ���� �� �� ����..
bool Handle_INVALID(PacketSessionRef& session, BYTE* buffer, int32 len);

//���� �ڵ�ȭ
bool Handle_S_LOGIN(PacketSessionRef& session, Protocol::S_LOGIN& pkt);
bool Handle_S_ENTER_GAME(PacketSessionRef& session, Protocol::S_ENTER_GAME& pkt);
bool Handle_S_CHAT(PacketSessionRef& session, Protocol::S_CHAT& pkt);


class FClientPacketHandler
{
public:

	// TODO : �ڵ�ȭ
	static void Init()
	{
		for (int32 i = 0; i < UINT16_MAX; i++)
		{
			GPacketHandler[i] = Handle_INVALID; //�Լ��� �ϴ� HANDLE INVALID�� ���
		}

		//��Ŷ�� �þ ������ �߰��ϴ� �ڵ�ȭ �ʿ�.
		//PKT_S_TEST�� ���� �Լ� ���
		GPacketHandler[PKT_S_LOGIN] = [](PacketSessionRef& session, BYTE* buffer, int32 len) { return HandlePacket<Protocol::S_LOGIN>(Handle_S_LOGIN, session, buffer, len); };
		GPacketHandler[PKT_S_ENTER_GAME] = [](PacketSessionRef& session, BYTE* buffer, int32 len) { return HandlePacket<Protocol::S_ENTER_GAME>(Handle_S_ENTER_GAME, session, buffer, len); };
		GPacketHandler[PKT_S_CHAT] = [](PacketSessionRef& session, BYTE* buffer, int32 len) { return HandlePacket<Protocol::S_CHAT>(Handle_S_CHAT, session, buffer, len); };
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

	// TODO : �ڵ�ȭ
	static SendBufferRef MakeSendBuffer(Protocol::C_LOGIN& pkt) { return MakeSendBuffer(pkt, PKT_C_LOGIN); }
	static SendBufferRef MakeSendBuffer(Protocol::C_ENTER_GAME& pkt) { return MakeSendBuffer(pkt, PKT_C_ENTER_GAME); }
	static SendBufferRef MakeSendBuffer(Protocol::C_CHAT& pkt) { return MakeSendBuffer(pkt, PKT_C_CHAT); }

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
		const uint16 dataSize = static_cast<uint16>(pkt.ByteSizeLong());
		const uint16 packetSize = dataSize + sizeof(FPacketHeader);

		//SendBufferRef sendBuffer = GSendBufferManager->Open(packetSize);
		//FPacketHeader* header = reinterpret_cast<FPacketHeader*>(sendBuffer->GetBuffer());
		SendBufferRef sendBuffer = make_shared<FSendBuffer>(packetSize);
		FPacketHeader* header = reinterpret_cast<FPacketHeader*>(sendBuffer->GetBuffer());
		header->size = packetSize;
		header->id = pktId;
		ASSERT_CRASH(pkt.SerializeToArray(&header[1], dataSize));
		sendBuffer->Close(packetSize);

		return sendBuffer;
	}
};