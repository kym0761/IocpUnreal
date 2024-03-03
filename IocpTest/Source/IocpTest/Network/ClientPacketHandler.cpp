#include "ClientPacketHandler.h"
#include "BufferReader.h"

//PacketHandlerFunc GPacketHandler[UINT16_MAX];
//
//bool Handle_INVALID(PacketSessionRef& session, BYTE* buffer, int32 len)
//{
//	//FPacketHeader* header = reinterpret_cast<FPacketHeader*>(buffer);
//	// TODO : Log
//	return false;
//}
//
//bool Handle_S_LOGIN(PacketSessionRef& session, Protocol::S_LOGIN& pkt)
//{
//	//if (pkt.success() == false)
//	//	return true;
//
//	//if (pkt.players().size() == 0)
//	//{
//	//	//캐릭터가 없으면 캐릭터 생성창으로 간다고 가정
//	//}
//
//	//// 입장 UI버튼을 눌러서 게임을 입장한다고 가정
//	//Protocol::C_ENTER_GAME enterGamePkt;
//	//enterGamePkt.set_playerindex(0); // 일단 0번째 캐릭터로 입장한다고 생각
//	//auto sendBuffer = FClientPacketHandler::MakeSendBuffer(enterGamePkt);
//	//session->Send(sendBuffer);
//
//	return true;
//}
//
//bool Handle_S_ENTER_GAME(PacketSessionRef& session, Protocol::S_ENTER_GAME& pkt)
//{
//	return true;
//}
//
//bool Handle_S_CHAT(PacketSessionRef& session, Protocol::S_CHAT& pkt)
//{
//	////받은 메시지 보여줌
//	//std::cout << pkt.msg() << endl;
//	return true;
//
//}
//
