#pragma once
class FAccountManager
{
	USE_LOCK

public:

	void AccountThenPlayer();
	void Lock();

};

extern FAccountManager GAccountManager;
