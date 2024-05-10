#pragma once

enum class EEventType : uint8
{
	Connect,
	Disconnect,
	Accept,
	Recv,
	Send
};

/*--------------
	IocpEvent
---------------*/

//이벤트들의 함수는 virtual이면 안됨.
//cast를 사용할 때 의도한 방식대로 사용할 수가 없게 됨.
class FIocpEvent : public OVERLAPPED
{
public:
	FIocpEvent(EEventType type);

	void		Init();

	EEventType	GetEventType() { return EventType; }
	IocpObjectRef GetOwner() { return Owner; }
	void SetOwner(IocpObjectRef ownerIn) { Owner = ownerIn; }

protected:
	EEventType	EventType;
	IocpObjectRef Owner; // 이벤트의 owner

	//Owner의 예시
	//AcceptEvent의 Owner는 FListener거나 FSession일 것이다.

};

/*----------------
	ConnectEvent
-----------------*/

class FConnectEvent : public FIocpEvent
{
public:
	FConnectEvent() : FIocpEvent(EEventType::Connect) { }
};

/*--------------------
	DisconnectEvent
----------------------*/

class FDisconnectEvent : public FIocpEvent
{
public:
	FDisconnectEvent() : FIocpEvent(EEventType::Disconnect) { }
};


/*----------------
	AcceptEvent
-----------------*/

class FAcceptEvent : public FIocpEvent
{
public:
	FAcceptEvent() : FIocpEvent(EEventType::Accept) { }

	void SetSession(SessionRef session) { Session = session; }
	SessionRef GetSession() { return Session; }

private:
	SessionRef Session = nullptr;
};

/*----------------
	RecvEvent
-----------------*/

class FRecvEvent : public FIocpEvent
{
public:
	FRecvEvent() : FIocpEvent(EEventType::Recv) { }
};

/*----------------
	SendEvent
-----------------*/

class FSendEvent : public FIocpEvent
{
public:
	FSendEvent() : FIocpEvent(EEventType::Send) { }

	//레퍼런스 카운팅 관리를 Send이벤트에서 SendBuffer를 관리함
	vector<SendBufferRef> SendBuffers;
};