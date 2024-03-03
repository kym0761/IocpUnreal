#pragma once

#include "Protocol.pb.h"

using PacketHandlerFunc = std::function<bool(PacketSessionRef&, BYTE*, int32)>; //�Լ� ������
extern PacketHandlerFunc GPacketHandler[UINT16_MAX]; //�ʿ������� �𸣴� �Լ� �����͵��� �̸� �����.

// TODO : �ڵ�ȭ
enum : uint16
{
	PKT_C_TEST = 1000,
	PKT_S_TEST = 1001,
	PKT_S_LOGIN = 1002,
};



// Custom Handlers
//�� �Լ� ������ ����ڰ� ���� ������ ��.
// ����? �ڵ�ȭ �ý����� �� �Լ��� � ����� ���� �� �� ����..
bool Handle_INVALID(PacketSessionRef& session, BYTE* buffer, int32 len);

//���� �ڵ�ȭ
bool Handle_C_TEST(PacketSessionRef& session, Protocol::C_TEST& pkt);


class TestPacketHandler
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
		GPacketHandler[PKT_C_TEST] = [](PacketSessionRef& session, BYTE* buffer, int32 len) { return HandlePacket<Protocol::C_TEST>(Handle_C_TEST, session, buffer, len); };
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
	static SendBufferRef MakeSendBuffer(Protocol::S_TEST& pkt) { return MakeSendBuffer(pkt, PKT_S_TEST); }
	static SendBufferRef MakeSendBuffer(Protocol::S_LOGIN& pkt) { return MakeSendBuffer(pkt, PKT_S_LOGIN); }

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

		SendBufferRef sendBuffer = GSendBufferManager->Open(packetSize);
		FPacketHeader* header = reinterpret_cast<FPacketHeader*>(sendBuffer->GetBuffer());
		header->size = packetSize;
		header->id = pktId;
		ASSERT_CRASH(pkt.SerializeToArray(&header[1], dataSize));
		sendBuffer->Close(packetSize);

		return sendBuffer;
	}
};