#pragma once
class FObjectUtils
{
public:
	static PlayerRef CreatePlayer(GameSessionRef session);

private:
	static atomic<int64> S_IdGenerator;
};

