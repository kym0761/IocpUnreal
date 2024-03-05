#pragma once

class FGameSession;
class FRoom;

class FPlayer : public enable_shared_from_this<FPlayer>
{
public:
	FPlayer();
	virtual ~FPlayer();

public:
	Protocol::PlayerInfo* PlayerInfo;
	weak_ptr<FGameSession> Session;

public:
	atomic<weak_ptr<FRoom>> Room;
};

