// DummyClient.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include "pch.h"
#include "ThreadManager.h"
#include "Service.h"
#include "Session.h"
#include "BufferReader.h"
#include "ClientPacketHandler.h"

	/* 사용하지 않음 */
	//언리얼 엔진이 클라이언트를 대체할 것.
	//만일 활성화하고 싶다면 build->config manager에서 dummyclient 빌드 체크

char sendData[] = "Hello World";

class FServerSession : public FPacketSession
{
public:
	~FServerSession()
	{
		cout << "~FServerSession" << endl;
	}

	virtual void OnConnected() override
	{
		//cout << "Connected To Server" << endl;

		//일단 로그인
		Protocol::C2S_LOGIN pkt;
		auto sendBuffer = FClientPacketHandler::MakeSendBuffer(pkt);
		Send(sendBuffer);
	}

	virtual void OnRecvPacket(BYTE* buffer, int32 len) override
	{
		PacketSessionRef session = GetPacketSessionRef();
		FPacketHeader* header = reinterpret_cast<FPacketHeader*>(buffer);

		// TODO : packetId 대역 체크
		FClientPacketHandler::HandlePacket(session, buffer, len);
	}

	virtual void OnSend(int32 len) override
	{
		//cout << "OnSend Len = " << len << endl;
	}

	virtual void OnDisconnected() override
	{
		//cout << "Disconnected" << endl;
	}
};

int main()
{

	FClientPacketHandler::Init();

	this_thread::sleep_for(1s);

	ClientServiceRef service = make_shared<FClientService>(
		FNetAddress(L"127.0.0.1", 7777),
		make_shared<FIocpCore>(),
		[=]() { return make_shared<FServerSession>(); }, // TODO : SessionManager 등
		1);

	ASSERT_CRASH(service->Start());

	for (int32 i = 0; i < 2; i++)
	{
		GThreadManager->Launch([=]()
			{
				while (true)
				{
					service->GetIocpCore()->Dispatch();
				}
			});
	}

	Protocol::C2S_CHAT chatPkt;
	chatPkt.set_msg(u8"Client's Hello World !");
	auto sendBuffer = FClientPacketHandler::MakeSendBuffer(chatPkt);

	//임시로 계속 메시지 보냄
	while (true)
	{
		service->Broadcast(sendBuffer);
		this_thread::sleep_for(1s);
	}

	GThreadManager->Join();

	return 0;
}