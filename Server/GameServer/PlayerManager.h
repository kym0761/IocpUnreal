#pragma once
class FPlayerManager
{
	USE_LOCK

public:

	void PlayerThenAccount();
	void Lock();


};

extern FPlayerManager GPlayerManager;