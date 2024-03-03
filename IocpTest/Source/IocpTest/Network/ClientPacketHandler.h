#pragma once

#include "Protocol.pb.h"

//using PacketHandlerFunc = std::function<bool(PacketSessionRef&, BYTE*, int32)>; //함수 포인터
//extern PacketHandlerFunc GPacketHandler[UINT16_MAX]; //필요할지도 모르는 함수 포인터들을 미리 만든다.
//
//// TODO : 자동화
//enum : uint16
//{
//	PKT_C_LOGIN = 1000,
//	PKT_S_LOGIN = 1001,
//	PKT_C_ENTER_GAME = 1002,
//	PKT_S_ENTER_GAME = 1003,
//	PKT_C_CHAT = 1004,
//	PKT_S_CHAT = 1005,
//};
//
//
//
//// Custom Handlers
////이 함수 구현은 사용자가 직접 만들어야 함.
//// 이유? 자동화 시스템은 이 함수가 어떤 기능을 할지 알 수 없다..
//bool Handle_INVALID(PacketSessionRef& session, BYTE* buffer, int32 len);
//
////선언만 자동화
//bool Handle_S_LOGIN(PacketSessionRef& session, Protocol::S_LOGIN& pkt);
//bool Handle_S_ENTER_GAME(PacketSessionRef& session, Protocol::S_ENTER_GAME& pkt);
//bool Handle_S_CHAT(PacketSessionRef& session, Protocol::S_CHAT& pkt);
//
//
//class FClientPacketHandler
//{
//public:
//
//	// TODO : 자동화
//	static void Init()
//	{
//		for (int32 i = 0; i < UINT16_MAX; i++)
//		{
//			GPacketHandler[i] = Handle_INVALID; //함수에 일단 HANDLE INVALID를 등록
//		}
//
//		//패킷이 늘어날 떄마다 추가하는 자동화 필요.
//		//PKT_S_TEST에 대한 함수 등록
//		GPacketHandler[PKT_S_LOGIN] = [](PacketSessionRef& session, BYTE* buffer, int32 len) { return HandlePacket<Protocol::S_LOGIN>(Handle_S_LOGIN, session, buffer, len); };
//		GPacketHandler[PKT_S_ENTER_GAME] = [](PacketSessionRef& session, BYTE* buffer, int32 len) { return HandlePacket<Protocol::S_ENTER_GAME>(Handle_S_ENTER_GAME, session, buffer, len); };
//		GPacketHandler[PKT_S_CHAT] = [](PacketSessionRef& session, BYTE* buffer, int32 len) { return HandlePacket<Protocol::S_CHAT>(Handle_S_CHAT, session, buffer, len); };
//	}
//
//	//어떤 클라이언트가 패킷을 보낸 것인지 확인하기 위해 sessionref를 받는다.
//	static bool HandlePacket(PacketSessionRef& session, BYTE* buffer, int32 len)
//	{
//		FPacketHeader* header = reinterpret_cast<FPacketHeader*>(buffer);
//
//		//헤더 id에 맞는 패킷 handle 함수를 호출
//		//이상한 id..예를 들면 아예 존재하지 않는 id라면
//		//애초에 HANDLE_INVALID로 막아놓으니 상관없음.
//		return GPacketHandler[header->id](session, buffer, len);
//	}
//
//	// TODO : 자동화
//	static SendBufferRef MakeSendBuffer(Protocol::C_LOGIN& pkt) { return MakeSendBuffer(pkt, PKT_C_LOGIN); }
//	static SendBufferRef MakeSendBuffer(Protocol::C_ENTER_GAME& pkt) { return MakeSendBuffer(pkt, PKT_C_ENTER_GAME); }
//	static SendBufferRef MakeSendBuffer(Protocol::C_CHAT& pkt) { return MakeSendBuffer(pkt, PKT_C_CHAT); }
//
//private:
//
//	template<typename PacketType, typename ProcessFunc>
//	static bool HandlePacket(ProcessFunc func, PacketSessionRef& session, BYTE* buffer, int32 len)
//	{
//		//패킷을 받아주는 역할.
//		PacketType pkt;
//		if (pkt.ParseFromArray(
//			buffer + sizeof(FPacketHeader),
//			len - sizeof(FPacketHeader)) == false)
//		{
//			return false;
//		}
//
//		//만든 패킷을 func로 동작시킴.
//		return func(session, pkt);
//	}
//
//	template<typename T>
//	static SendBufferRef MakeSendBuffer(T& pkt, uint16 pktId)
//	{
//		const uint16 dataSize = static_cast<uint16>(pkt.ByteSizeLong());
//		const uint16 packetSize = dataSize + sizeof(FPacketHeader);
//
//		//SendBufferRef sendBuffer = GSendBufferManager->Open(packetSize);
//		//FPacketHeader* header = reinterpret_cast<FPacketHeader*>(sendBuffer->GetBuffer());
//		SendBufferRef sendBuffer = make_shared<FSendBuffer>(packetSize);
//		FPacketHeader* header = reinterpret_cast<FPacketHeader*>(sendBuffer->GetBuffer());
//		header->size = packetSize;
//		header->id = pktId;
//		ASSERT_CRASH(pkt.SerializeToArray(&header[1], dataSize));
//		sendBuffer->Close(packetSize);
//
//		return sendBuffer;
//	}
//};