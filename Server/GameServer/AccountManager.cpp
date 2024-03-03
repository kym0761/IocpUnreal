#include "pch.h"
#include "AccountManager.h"
#include "PlayerManager.h"

FAccountManager GAccountManager;

void FAccountManager::AccountThenPlayer()
{
	WRITE_LOCK;
		//this_thread::sleep_for(1s);
	GPlayerManager.Lock();

}

void FAccountManager::Lock()
{
	WRITE_LOCK;
}
