#include "pch.h"
#include "Service.h"
#include "Session.h"
#include "Listener.h"

FService::FService(EServiceType type, FNetAddress address, IocpCoreRef core, FSessionFactory factory, int32 maxSessionCount)
	: ServiceType(type), NetAddress(address), IocpCore(core), SessionFactory(factory), MaxSessionCount(maxSessionCount)
{
}

FService::~FService()
{
}

void FService::CloseService()
{
	cout << "Service Closed." << endl;
}

void FService::Broadcast(SendBufferRef sendBuffer)
{
	WRITE_LOCK;

	for (const auto& session : Sessions)
	{
		//더미 클라이언트에서 C2S_CHAT 브로드캐스트 용으로 사용될 것.
		//즉, 현재는 쓸모 없다는 의미!
		session->Send(sendBuffer);
	}

}

SessionRef FService::CreateSession()
{
	//세션 만든 뒤 세션의 서비스 등록
	//SesssionFactory는 Service를 생성할 때 등록한 생성용 함수다.
	//이 GameServer.cpp에선 람다로 FGameSession이 생성될 함수가 등록됨.
	SessionRef session = SessionFactory();
	session->SetService(shared_from_this());

	if (IocpCore->Register(session) == false)
	{
		return nullptr;
	}

	return session;
}

void FService::AddSession(SessionRef session)
{
	WRITE_LOCK;
	SessionCount++;
	Sessions.insert(session);

}

void FService::ReleaseSession(SessionRef session)
{
	WRITE_LOCK;
	ASSERT_CRASH(Sessions.erase(session) != 0);
	SessionCount--;
}

FClientService::FClientService(FNetAddress targetAddress, IocpCoreRef core, FSessionFactory factory, int32 maxSessionCount)
	: FService(EServiceType::Client, targetAddress, core, factory, maxSessionCount)
{
}

bool FClientService::Start()
{
	//클라이언트 서비스는 더미 클라이언트에서 n개의 접속을 가정하기에
	//start에서 세션을 미리 만들어 클라이언트가 있는 것처럼 작동함.

	if (CanStart() == false)
		return false;

	const int32 sessionCount = GetMaxSessionCount();
	for (int32 i = 0; i < sessionCount; i++)
	{
		SessionRef session = CreateSession();
		if (session->Connect() == false)
		{
			return false;
		}

	}

	return true;
}

FServerService::FServerService(FNetAddress targetAddress, IocpCoreRef core, FSessionFactory factory, int32 maxSessionCount)
	: FService(EServiceType::Server, targetAddress, core, factory, maxSessionCount)
{
}

bool FServerService::Start()
{
	if (CanStart() == false)
	{
		return false;
	}

	//소켓 프로그래밍에 필요한 accept bind listen을 담당함.
	Listener = make_shared<FListener>();
	if (Listener == nullptr)
	{
		return false;
	}
		
	//리스너 - 소켓 생성 / iocp register / bind / listen ... 등 작업
	//static_pointer_cast == 스마트포인터 전용 static_cast
	ServerServiceRef service = static_pointer_cast<FServerService>(shared_from_this());
	if (Listener->StartAccept(service) == false)
	{
		return false;
	}

	return true;
}

void FServerService::CloseService()
{
	// TODO ? 현재는 딱히 할 일이 없어보임.

	FService::CloseService();

}
