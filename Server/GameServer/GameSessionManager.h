#pragma once

class FGameSession;

using GameSessionRef = shared_ptr<FGameSession>;

class FGameSessionManager
{
public:
	void Add(GameSessionRef session);
	void Remove(GameSessionRef session);
	void Broadcast(SendBufferRef sendBuffer);

private:
	USE_LOCK;
	set<GameSessionRef> Sessions;
};

extern FGameSessionManager GSessionManager;
