#include "pch.h"
#include "CoreGlobal.h"
#include "ThreadManager.h"
//#include "DeadLockProfiler.h"
//#include "Memory.h"
#include "SocketUtils.h"
#include "GlobalQueue.h"
#include "JobTimer.h"
//#include "DBConnectionPool.h"
#include "ConsoleLog.h"


FThreadManager* GThreadManager = nullptr;
//FMemory* GMemory = nullptr;
//FDeadLockProfiler* GDeadLockProfiler = nullptr;
//FSendBufferManager* GSendBufferManager = nullptr;
FGlobalQueue* GGlobalQueue = nullptr;
FJobTimer* GJobTimer = nullptr;
//FDBConnectionPool* GDBConnectionPool = nullptr;
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
	//GMemory = new FMemory();
	//GDeadLockProfiler = new FDeadLockProfiler();
	//GSendBufferManager = new FSendBufferManager();
	GGlobalQueue = new FGlobalQueue();
	GJobTimer = new FJobTimer();
	//GDBConnectionPool = new FDBConnectionPool();
	GConsoleLogger = new ConsoleLog();
	FSocketUtils::Init();
}

FCoreGlobal::~FCoreGlobal()
{
	delete GThreadManager;
	//delete GMemory;
	//delete GDeadLockProfiler;
	//delete GSendBufferManager;
	delete GGlobalQueue;
	delete GJobTimer;
	//delete GDBConnectionPool;
	delete GConsoleLogger;
	FSocketUtils::Clear();
}