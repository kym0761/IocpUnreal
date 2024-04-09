# IocpUnreal

언리얼 엔진과 C++ 서버를 통한 통신

언리얼 데디케이티드 서버가 아닌 C++ 소켓을 사용함.

# Server

IOCP를 기반으로 만들어진 서버다.

DummyClient는 테스트를 위한 클라이언트로, 언리얼 엔진을 클라이언트로 사용하게 되어 더이상 사용하지 않으므로 생략함.

ServerCore와 GameServer만 설명

클래스들의 명칭 앞에 F 붙은 이유는 언리얼 사용하면서 대부분의 사용자 클래스와 구조체에 F가 붙게 만들도록 한 것이 익숙하여 F를 붙임. 

예를 들면, FListener 클래스 이름을 설정하면 FListener Listener; 처럼 인스턴스 이름을 설정할 때 편함.

## ServerCore

### Main/CoreGlobal

싱글톤 클래스들을 사용하기 위해 선언 및 생성한 글로벌 클래스

cpp 파일에 FCoreGlobal GCoreGlobal;로 글로벌 클래스가 생성되면서 필요한 싱글톤 클래스가 자동으로 생성됨.

### Main/CoreMacro

Lock, CRASH 등을 매크로로 지정한 헤더 파일

### Main/CorePch.h

필요한 모든 헤더들을 정리해놓은 pch 파일

### Main/CoreTLS

Thread Local Storage 변수를 정리해놓은 .h 및 .cpp

### Main/Types.h

명칭이 긴 변수들을 짧은 이름으로 정의한 헤더 파일

### Network/IocpCore

IocpObject : 추후에 설명한 Session, Listener와 같은 클래스의 최상위 클래스로 IocpEvent에 의해 IO 완료된 Object의 Dispatch가 동작되도록 등록의 대상인 클래스

IocpCore : Iocp 동작에서 GetQueuedCompletionStatus()가 Call하여 IocpObject의 동작에 관여하는 메인 클래스. Service 클래스가 생성될 때 최초로 생성될 것. 생성은 GameServer/GameServer.cpp의 Main에 있음.

### Network/IocpEvent

Connect, Disconnect, Accept, Recv, Send 등의 소켓 기능을 IOCP 큐에 등록할 때 넣는 대상. IocpObject를 Owner로 보유하여 위에 설명한 IocpCore의 GetQueuedCompletionStatus()로 IO 완료시 어떤 이벤트가 완료되었는지 확인할 수 있음. 해당 Event가 완료되어 나왔을 때 Owner를 Dispatch하여 원하는 Iocp 동작을 하게됨.


### Network/Listener

소켓 통신에서 Listen을 실질적으로 담당하는 IocpObject다.

1. Accept 이벤트 생성한 뒤, Owner를 Listener로 설정함.
2. Owner가 설정된 Accept 이벤트는 RegisterAccept로 Iocp에 등록하면 추후 IocpCore에 의해 Accept가 IO완료된 뒤 처리에 접근할 수 있음.
3. Accept이벤트의 Dispatch로 ProcessAccept가 동작하여 연결된 클라이언트 세션의 얻어 서버가 클라이언트와 연결을 완료.

### Network/NetAddress

소켓 통신에 사용될 소켓 주소 및 Ip 주소 등의 정보 등록에 사용될 Wrap클래스

사용 예시는 GameServer/GameServer.cpp와 FListener, FSession 클래스에서 볼 수 있음.

### Network/RecvBuffer

Recv(리시브)에 사용될 버퍼

리시브 버퍼는 충분한 크기가 지정되고, 데이터를 받아 쓰여진 정도를 WritePos, 얻어올 데이터를 읽은 정도를 ReadPos 위치를 보면서 받은 데이터의 처리를 도와준다.

RecvBuffer에 정리된 내용대로, ReadPos 와 WritePos의 위치가 같아지면 받은 데이터들을 일단 완료된 것으로 판단하여 초기 지점으로 초기화를 하여 Recv에 필요한 공간을 확보한다.

Recv와 아래 Send 버퍼는 Session에서 사용될 것이다.

Recv 동작 설명

1. RegisterRecv에서 RecvBuffer 주소와 공간을 WSABUF에 넣은 뒤, RecvEvent로 복귀한 이벤트를 WSARecv()로 IOCP에 등록한다.
2. IocpCore에서 GetQueued...()를 통해 복구된 RecvEvent의 Owner인 Session의 Dispatch가 동작하여 ProcessRecv()가 동작됨.
3. ProcessRecv()에서 OnWrite()를 통해 RecvBuffer WritePos 세팅.
4. ProcessRecv()에서 OnRecv()를 통해 받은 패킷을 해석 및 조립함. (패킷은 추후에 설명)
5. RecvBuffer의 OnRead()를 통해 ReadPos 세팅.

### Network/SendBuffer

Send에 사용할 버퍼.

Send의 구조 자체는 복잡하지 않다. 서버에서 보낼 데이터 크기만큼 Buffer를 지정한 뒤 WSASend()하면 된다.

다만, Send는 1개의 SendBuffer에 대해서만 Send() 동작하는 것이 아니라, Queue에 SendBuffer 담아둔 뒤 Queue에 있는 내용 전부를 WSABUF에 넣어 한번에 여러개의 SendBuffer의 내용을 WSASend()한다.

### Network/Service

Service는 소켓 통신을 담당해주는 IOCP, 다수의 Session을 보유한 서버의 단위다.

Factory로 session을 생성, 필요없는 session을 제거 & Listener 동작 등을 담당함.

### Network/Session

클라이언트의 등록 정보를 나타낸다.

연결된 클라이언트에 맞춰 Recv, Send, Connect, Disconnect 등을 담당한다.

#### FPacketHeader

패킷을 읽을 때 필요한 헤더다.

size를 읽은 뒤에 id를 읽어야 하는 방식으로 size와 id의 순서가 중요함.

데이터를 읽기 위해서는 최소한 이 헤더의 크기만큼의 데이터를 받아야할 필요가 있다.

#### FPacketSession

패킷 조립을 목표로 OnRecv가 실질적으로 구현된 Session 클래스다.

OnRecv를 보면 위에 PacketHeader 설명대로 RecvBuffer의 데이터를 읽을 때 최소 헤더 크기를 확인한 뒤에 패킷의 크기를 확인하여 올바른 패킷이 되었는지 확인한 뒤에 RecvBuffer ReadPos를 갱신함.

이 FPacketSession 클래스로 FGameSession 만들어 컨텐츠 구현을 GameServer에서 한다.

FGameSession의 OnRecvPacket()을 통해 받은 패킷의 헤더를 읽어 패킷을 분석하여 FServerPacketHandler::HandlePacket()을 통해 분석된 패킷에 따라 다른 동작을 하게 될 것이다.

이후 내용은 GameSession과 ServerPacketHandler를 설명할 때 함.

### NetWork/ServerUtils

소켓 프로그래밍에 필요한 winsock 함수들을 래핑한 클래스로 소켓 bind listen close 등의 기능을 정리함.

### Thread/ThreadManager

다수의 쓰레드를 운용하기 위한 싱글톤 Manager 클래스.

### Utils/BufferReader

### Utils/BufferReader

### Utils/LockQueue

서버의 일 처리를 Job 단위로 처리할 때 필요한 Lock 기능을 포함한 Queue

### Job/GlobalQueue

서버의 일처리에 필요한 JobQueue를 관리해줄 LockQueue Wrap클래스

### Job/Job

Job은 해야할 일을 Callback을 받은 일처리의 단위다.

JobQueue를 보면 DoAsync에서 Callback을 받아 Job을 생성하여 queue에 집어넣고 추후에 한번에 Job들의 일을 처리해준다.

이 방식을 사용하면 JobQueue에 Job을 넣고 뺄 때만 Lock을 걸고, 서버에서 Lock을 거는 비중이 줄어든다.

### Job/JobQueue

처리를 해야할 Job을 넣고 어느 쓰레드에게 Job 처리를 맡게 하기 위한 클래스

먼저, 최초 Job이 안들어가 있을 상태에 어떤 쓰레드가 Job을 queue에 넣는다면, 그 쓰레드는 Job을 처리하는 역할을 맡음.

위에서 어떤 쓰레드가 execute를 맡기로 했으면, 다른 쓰레드가 Job을 queue에 넣을 때는 그냥 Job을 넣기만 해준다.

Job 처리를 담당할 쓰레드는 queue에 들어간 모든 Job들을 얻어낸 뒤에 Job->Execute()한다.
queue안에 있는 Job들을 모두 처리했거나, 쓰레드에 걸린 일처리 제한 시간이 끝났다면 해당 쓰레드의 Execute()를 종료하고 다음 타자에게 넘긴다.

이때 타자를 넘긴다는 의미로 GlobalQueue->Push()하면, 추후에 다른 쓰레드가 ThreadManager에 의해 JobQueue를 받아내 Execute한다. GameServer.cpp를 참고할 것.

이 JobQueue는 GameServer의 Room이다.

### Job/JobTimer

JobTimer는 예약시간을 정하여 JobQueue에 원하는 Job을 넣어 처리하도록 만든 싱글톤 클래스다.

예약된 Job은 ThreadManager의 DistributeReservedJobs()에서 처리된다.

## GameServer

### Main/GameServer.cpp

Service 생성 및 실행.

ThreadManager를 통해 각 쓰레드마다 제한시간 부여 및 Dispatch, 예약된 Job 분배(JobTimer), JobQueue 실행을 실질적으로 실행시켜준다.

Room의 UpdateTick도 최초 실행시켜준다.

### Main/GameSession

HandlePacket()으로 해석된 패킷에 따라 서버의 행동을 실행함.

### Main/GameSessionManager

접속한 클라이언트의 게임세션을 관리해주는 매니저.

GameSession에서 OnConnect() OnDisconnect()에서 GameSession을 추가/제거한다.

만약 SendBuffer의 내용을 전체 클라이언트에게 보낼 계획이면 Broadcast()를 사용하여 전체 클라이언트에서 패킷을 보낼 수 있음.

### ServerPacketHandler

ProtoBuffer를 사용하여 생성된 패킷을 처리한다.

방대한 내용이라 직접 .h .cpp를 보고 해석해야하는데,

패킷의 id를 보고 각 패킷에 맞는 Handle_XXX() 함수가 어떤 행동을 해야 하는지 .cpp에 구현해야함.

이 ServerPacketHandler.h은 Protobuffer의 패킷이 추가될 때마다 자동으로 적절한 패킷 함수 선언을 추가해준다.

Tools/PacketGenerator로 Python 코드로 패킷 핸들러 코드 자동화처리가 되어 있음.

