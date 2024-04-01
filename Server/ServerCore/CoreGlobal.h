#pragma once

extern class FThreadManager* GThreadManager;
extern class FGlobalQueue* GGlobalQueue;
extern class FJobTimer* GJobTimer;

//사용하지 않음. 사용하지 않고 있는 클래스에 엮인 부분이 많아서 지우지는 않음.
extern class ConsoleLog* GConsoleLogger;


class FCoreGlobal
{
public:

	FCoreGlobal();
	~FCoreGlobal();
};