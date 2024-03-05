#include "pch.h"
#include "ObjectUtils.h"
#include "Player.h"
#include "GameSession.h"

//1부터 시작
atomic<int64> FObjectUtils::S_IdGenerator = 1;

PlayerRef FObjectUtils::CreatePlayer(GameSessionRef session)
{	
	// ID 생성기
	const int64 newId = S_IdGenerator.fetch_add(1);

	PlayerRef player = make_shared<FPlayer>();
	player->PlayerInfo->set_object_id(newId);

	player->Session = session;
	session->Player.store(player);

	return player;
}
