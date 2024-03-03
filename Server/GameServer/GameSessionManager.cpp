#include "pch.h"
#include "GameSessionManager.h"
#include "GameSession.h"

FGameSessionManager GSessionManager;


void FGameSessionManager::Add(GameSessionRef session)
{
	WRITE_LOCK;
	Sessions.insert(session);
}

void FGameSessionManager::Remove(GameSessionRef session)
{
	WRITE_LOCK;
	Sessions.erase(session);
}

void FGameSessionManager::Broadcast(SendBufferRef sendBuffer)
{
	WRITE_LOCK;
	for (GameSessionRef session : Sessions)
	{
		session->Send(sendBuffer);
	}
}
