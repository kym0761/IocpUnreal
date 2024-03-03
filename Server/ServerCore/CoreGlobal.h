#pragma once

extern class FThreadManager* GThreadManager;
//extern class FMemory* GMemory;
//extern class FDeadLockProfiler* GDeadLockProfiler;
//extern class FSendBufferManager* GSendBufferManager;
extern class FGlobalQueue* GGlobalQueue;
extern class FJobTimer* GJobTimer;

//extern class FDBConnectionPool* GDBConnectionPool;
extern class ConsoleLog* GConsoleLogger;


class FCoreGlobal
{
public:

	FCoreGlobal();
	~FCoreGlobal();
};