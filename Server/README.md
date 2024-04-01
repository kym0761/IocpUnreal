# Server 분석



# 시작

## CoreGlobal

다음 글로벌 클래스를 생성함.
GThreadManager
GGlobalQueue
GJobTimer

FSocketUtils::Init() 초기화

### FSocketUtils::Init()

임시 소켓을 생성하고 Accept, Connect, Disconnect에 해당하는 함수포인터에 미리 함수를 Bind함.
커널 객체이므로, 100% 이해보다는 커널에 동작하는 함수가 있다는 것을 이해하면 됨. 
이 함수포인터는 추후 Session의 RegisterXXX에서 사용함

## GameServerMain 01

FServerPacketHandler::Init();

### FServerPacketHandler

Protobuffer에 의해 생성된 Packet에 맞는 PacketHandler Bind함.
이 바인드 된 Packet은 아래에 있는 HandlePacket에 의해
헤더의 id를 읽고 GPacketHandler[id]를 실행함


## GameServerMain 02

FServerService 생성
{
	ip port
	IocpCore 생성
	[=]() { return make_shared<FGameSession>(); //SessionFactory
}

service -> start(); //서비스 시작

### FServerService

서버 기준으로 FServerService를 봐야함.

실행시에 SessionFactory가 존재해야함

Listener를 생성

Listener의 StartAccept() 실행 // 서비스의 shared_ptr이 필요함.

#### Listener

Listener는 IocpObject다.

소켓 생성

IocpCore에 Listener를 등록

주소 재사용 여부

옵션

Bind

Listen

MaxSessionCount 개수만큼 acceptEvent를 생성하고 RegisterAccept();로 accept 이벤트 등록

## GameServerMain 03

ThreadManager에게 DoWorkerJob 실행

GRoom-> updatetick 최초 실행

### DoWorkerJob

각 쓰레드마다 시간제한을 부여하고
service -> iocpcore -> dispatch()를 10ms 안에 실행하도록 함.

FThreadManager에 의해 예약된 Job을 분배함.

FThreadManager에 의해 JobQueue를 불러와 Job을 실행함.

#### IocpCore->Dispatch

Iocp에 등록된 이벤트를 불러와서 이 이벤트의 Owner인 IocpObject가 처리가 가능한지 확인하고 실행함.

IocpObject 객체는 (IocpCore와 IocpObject는 다른 것)
1. FListener
2. FSession
3. FPacketSession
4. FGameSession
이 있다.


