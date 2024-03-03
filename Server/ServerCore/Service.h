#pragma once
#include "NetAddress.h"
#include "IocpCore.h"
#include "Listener.h"
#include <functional>

enum class EServiceType : uint8
{
	Server,
	Client
};

using FSessionFactory = function<SessionRef(void)>;

class FService : public enable_shared_from_this<FService>
{
public:
	FService(EServiceType type, 
		FNetAddress address, 
		IocpCoreRef core, 
		FSessionFactory factory, 
		int32 maxSessionCount = 1);

	virtual ~FService();

	virtual bool Start() abstract;
	bool CanStart() { return SessionFactory != nullptr; }

	virtual void CloseService();
	void SetSessionFactory(FSessionFactory func) { SessionFactory = func; }

	void Broadcast(SendBufferRef sendBuffer);
	SessionRef CreateSession();
	void AddSession(SessionRef session);
	void ReleaseSession(SessionRef session);
	int32 GetCurrentSessionCount() { return SessionCount; }
	int32 GetMaxSessionCount() { return MaxSessionCount; }

public:
	EServiceType GetServiceType() { return ServiceType; }
	FNetAddress	GetNetAddress() { return NetAddress; }
	IocpCoreRef& GetIocpCore() { return IocpCore; }

protected:
	USE_LOCK;

	EServiceType		ServiceType;
	FNetAddress			NetAddress = {};
	IocpCoreRef			IocpCore;

	set<SessionRef>		Sessions;
	int32				SessionCount = 0;
	int32				MaxSessionCount = 0;
	FSessionFactory		SessionFactory;
};

class FClientService : public FService
{
public:
	FClientService(FNetAddress targetAddress, IocpCoreRef core, FSessionFactory factory, int32 maxSessionCount = 1);
	virtual ~FClientService() {}

	virtual bool	Start() override;
};

class FServerService : public FService
{
public:
	FServerService(FNetAddress targetAddress, IocpCoreRef core, FSessionFactory factory, int32 maxSessionCount = 1);
	virtual ~FServerService() {}

	virtual bool	Start() override;
	virtual void	CloseService() override;

private:
	ListenerRef		Listener = nullptr;
};