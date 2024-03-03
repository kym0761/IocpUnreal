#pragma once

#include "Session.h"

class FGameSession : public FPacketSession
{
public:
	~FGameSession()
	{
		cout << "~FGameSession" << endl;
	}


	virtual void OnConnected() override;
	virtual void OnDisconnected() override;
	virtual void OnRecvPacket(BYTE* buffer, int32 len) override;
	virtual void OnSend(int32 len) override;

public:

	weak_ptr<class FRoom> Room; //담당중인 Room
};
