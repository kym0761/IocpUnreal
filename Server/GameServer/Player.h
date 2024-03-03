#pragma once
class FPlayer
{
public:

	uint64					PlayerId = 0;
	string					Name;
	Protocol::PlayerType	Type = Protocol::PLAYER_TYPE_NONE;
	GameSessionRef			OwnerSession; // Cycle 문제가 발생할 수 있음.
};

