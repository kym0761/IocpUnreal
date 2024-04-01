#include "pch.h"
#include "GameSession.h"
#include "GameSessionManager.h"
#include "ServerPacketHandler.h"

#include "Room.h"
#include "Player.h"

FGameSession::FGameSession()
{
	//cout << "FGameSession()" << endl;
}

FGameSession::~FGameSession()
{
	cout << "~FGameSession" << endl;
}

void FGameSession::OnConnected()
{
	GSessionManager.Add(
		static_pointer_cast<FGameSession>(shared_from_this()));
}

void FGameSession::OnDisconnected()
{
	GSessionManager.Remove(
		static_pointer_cast<FGameSession>(shared_from_this()));
}

void FGameSession::OnRecvPacket(BYTE* buffer, int32 len)
{

	PacketSessionRef session = GetPacketSessionRef();
	FPacketHeader* header = reinterpret_cast<FPacketHeader*>(buffer);

	//TODO : 헤더를 확인해서, 클라이언트 패킷인지, 서버 패킷인지 구분해야함.
	FServerPacketHandler::HandlePacket(session, buffer, len);

}

void FGameSession::OnSend(int32 len)
{
	//cout << "OnSend Len = " << len << endl;
}
