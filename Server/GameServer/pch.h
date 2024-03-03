#pragma once

#define WIN32_LEAN_AND_MEAN  // 거의 사용되지 않을 내용을 Windows 헤더에서 제외한다..

#ifdef _DEBUG
#pragma comment(lib, "ServerCore\\Debug\\ServerCore.lib") // Debug
#pragma comment(lib, "Protobuf\\Debug\\libprotobufd.lib") // Debug
#else //_RELEASE
#pragma comment(lib, "ServerCore\\Release\\ServerCore.lib") // Release
#pragma comment(lib, "Protobuf\\Release\\libprotobuf.lib") // Release
#endif

#include "CorePch.h"
#include "Enum.pb.h"
#include "Struct.pb.h"
#include "ServerPacketHandler.h"

using GameSessionRef = shared_ptr<class FGameSession>;
//using PlayerRef = shared_ptr<class FPlayer>;