#include "pch.h"
#include "Player.h"

FPlayer::FPlayer()
{
	PlayerInfo = new Protocol::PlayerInfo();
}

FPlayer::~FPlayer()
{
	delete PlayerInfo;
}
