#pragma once

#include "Protocol.pb.h"

#if UE_BUILD_DEBUG + UE_BUILD_DEVELOPMENT + UE_BUILD_TEST + UE_BUILD_SHIPPING >= 1
#include "IocpTest.h"
#endif

using PacketHandlerFunc = std::function<bool(PacketSessionRef&, BYTE*, int32)>; //함수 포인터
extern PacketHandlerFunc GPacketHandler[UINT16_MAX]; //필요할지도 모르는 함수 포인터들을 미리 만든다.

// TODO : 자동화
enum : uint16
{
{%- for pkt in parser.total_pkt %}
	PKT_{{pkt.name}} = {{pkt.id}},
{%- endfor %}
};



// Custom Handlers

//INVALID는 실제론 사용되지 않을 임시 함수임.
bool Handle_INVALID(PacketSessionRef& session, BYTE* buffer, int32 len);

//선언만 자동화
//이 함수 구현은 사용자가 직접 만들어야 함.
// 이유? 자동화 시스템은 이 함수가 어떤 기능을 할지 알 수 없다..
{%- for pkt in parser.recv_pkt %}
bool Handle_{{pkt.name}}(PacketSessionRef& session, Protocol::{{pkt.name}}& pkt);
{%- endfor %}


class F{{output}}
{
public:

	// TODO : 자동화
	static void Init()
	{
		for (int32 i = 0; i < UINT16_MAX; i++)
		{
			GPacketHandler[i] = Handle_INVALID; //함수에 일단 HANDLE INVALID를 등록
		}

		//패킷이 늘어날 때마다 추가하는 자동화 위치.
		//PKT_S_TEST에 대한 함수 등록
{%- for pkt in parser.recv_pkt %}
		GPacketHandler[PKT_{{pkt.name}}] = [](PacketSessionRef& session, BYTE* buffer, int32 len) { return HandlePacket<Protocol::{{pkt.name}}>(Handle_{{pkt.name}}, session, buffer, len); };
{%- endfor %}
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

	// TODO : 자동화
{%- for pkt in parser.send_pkt %}
	static SendBufferRef MakeSendBuffer(Protocol::{{pkt.name}}& pkt) { return MakeSendBuffer(pkt, PKT_{{pkt.name}}); }
{%- endfor %}

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
		const uint16 dataSize = static_cast<uint16>(pkt.ByteSizeLong());
		const uint16 packetSize = dataSize + sizeof(FPacketHeader);

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
