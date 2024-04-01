#pragma once

#include "Protocol.pb.h"

#if UE_BUILD_DEBUG + UE_BUILD_DEVELOPMENT + UE_BUILD_TEST + UE_BUILD_SHIPPING >= 1
#include "IocpTest.h"
#endif

using PacketHandlerFunc = std::function<bool(PacketSessionRef&, BYTE*, int32)>; //함수 포인터
extern PacketHandlerFunc GPacketHandler[UINT16_MAX]; //필요할지도 모르는 함수 포인터들을 미리 만든다.

// 패킷 enum 자동화
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

//INVALID는 실제론 사용되지 않을 임시 함수임.
bool Handle_INVALID(PacketSessionRef& session, BYTE* buffer, int32 len);

//선언만 자동화
//이 함수 구현은 사용자가 직접 만들어야 함.
// 이유? 자동화 시스템은 이 함수가 어떤 기능을 할지 알 수 없다..
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
			GPacketHandler[i] = Handle_INVALID; //함수에 일단 HANDLE INVALID를 등록
		}

		//패킷이 늘어날 때마다 추가하는 자동화 위치.
		//PKT_S_TEST에 대한 함수 등록
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

	//어떤 클라이언트가 패킷을 보낸 것인지 확인하기 위해 sessionref를 받는다.
	static bool HandlePacket(PacketSessionRef& session, BYTE* buffer, int32 len)
	{
		FPacketHeader* header = reinterpret_cast<FPacketHeader*>(buffer);

		//헤더 id에 맞는 패킷 handle 함수를 호출
		//이상한 id..예를 들면 아예 존재하지 않는 id라면
		//애초에 HANDLE_INVALID로 막아놓으니 상관없음.
		return GPacketHandler[header->id](session, buffer, len);
	}

	// MakeSendBuffer 자동화
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
		//패킷을 받아주는 역할.
		PacketType pkt;
		if (pkt.ParseFromArray(
			buffer + sizeof(FPacketHeader),
			len - sizeof(FPacketHeader)) == false)
		{
			return false;
		}

		//만든 패킷을 func로 동작시킴.
		return func(session, pkt);
	}

	template<typename T>
	static SendBufferRef MakeSendBuffer(T& pkt, uint16 pktId)
	{
		//직렬화 코드

		const uint16 dataSize = static_cast<uint16>(pkt.ByteSizeLong());
		const uint16 packetSize = dataSize + sizeof(FPacketHeader);

		//if는 언리얼 엔진에서 동작 .. else는 c++ iocp 서버에서 동작
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