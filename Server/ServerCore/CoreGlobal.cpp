#include "pch.h"
#include "CoreGlobal.h"
#include "ThreadManager.h"
#include "SocketUtils.h"
#include "GlobalQueue.h"
#include "JobTimer.h"
#include "ConsoleLog.h"

FThreadManager* GThreadManager = nullptr;
FGlobalQueue* GGlobalQueue = nullptr;
FJobTimer* GJobTimer = nullptr;

//사용하지 않음.
ConsoleLog* GConsoleLogger = nullptr;

/* 이와 같음..
class CoreGlobal
{
 //...
} GCoreGlobal;
*/
FCoreGlobal GCoreGlobal;

FCoreGlobal::FCoreGlobal()
{
	GThreadManager = new FThreadManager();
	GGlobalQueue = new FGlobalQueue();
	GJobTimer = new FJobTimer();
	GConsoleLogger = new ConsoleLog();
	FSocketUtils::Init();
}

FCoreGlobal::~FCoreGlobal()
{
	delete GThreadManager;
	delete GGlobalQueue;
	delete GJobTimer;
	delete GConsoleLogger;
	FSocketUtils::Clear();
}