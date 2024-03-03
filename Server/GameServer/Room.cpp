#include "pch.h"
#include "Room.h"
#include "Player.h"
#include "GameSession.h"

shared_ptr<FRoom> GRoom = make_shared<FRoom>();

//void FRoom::Enter(PlayerRef player)
//{
//	//플레이어 추가
//	Players[player->PlayerId] = player;
//}
//
//void FRoom::Leave(PlayerRef player)
//{
//	//나간 플레이어 제거
//	Players.erase(player->PlayerId);
//}
//
//void FRoom::Broadcast(SendBufferRef sendBuffer)
//{
//	//모든 플레이어에게 메시지 전달
//	for (auto& p : Players)
//	{
//		p.second->OwnerSession->Send(sendBuffer);
//	}
//}
