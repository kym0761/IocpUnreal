#pragma once

#include "Session.h"

class FGameSession : public FPacketSession
{
public:

	FGameSession();
	~FGameSession();


	virtual void OnConnected() override;
	virtual void OnDisconnected() override;
	virtual void OnRecvPacket(BYTE* buffer, int32 len) override;
	virtual void OnSend(int32 len) override;

public:
	atomic<PlayerRef> Player;
};
