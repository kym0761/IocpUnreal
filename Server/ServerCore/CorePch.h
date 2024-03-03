#pragma once
#pragma warning(disable : 4819)

#include "Types.h"
#include "CoreMacro.h"
#include "CoreTLS.h"
#include "CoreGlobal.h"

//#include "Container.h"


#include <array>
#include <vector>
#include <list>
#include <queue>
#include <stack>
#include <map>
#include <set>
#include <unordered_map>
#include <unordered_set>

#include <Windows.h>
#include <iostream>
#include <assert.h>
using namespace std;

#include <winsock2.h>
#include <mswsock.h>
#include <ws2tcpip.h>
#pragma comment(lib, "ws2_32.lib")

//#include "Lock.h"
//#include "ObjectPool.h"
//#include "TypeCast.h"
//#include "Memory.h"

#include "SendBuffer.h"
#include "Session.h"
#include "JobQueue.h"
#include "ConsoleLog.h"

//이 프로젝트는 정적 라이브러리다.
//다른 프로젝트들 모두 이 pch를 통해 동작함.
//ServerCore가 변경되면 무조건 ServerCore부터 다시 빌드하고 동작시켜야함.