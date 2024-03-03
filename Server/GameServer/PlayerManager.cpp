#include "pch.h"
#include "PlayerManager.h"
#include "AccountManager.h"

FPlayerManager GPlayerManager;

void FPlayerManager::PlayerThenAccount()
{
	WRITE_LOCK;

		this_thread::sleep_for(1s);

		GPlayerManager.Lock();
}

void FPlayerManager::Lock()
{

	WRITE_LOCK;

}
