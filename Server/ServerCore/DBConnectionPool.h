#pragma once
#include "DBConnection.h"

class FDBConnectionPool
{
public:
	FDBConnectionPool();
	~FDBConnectionPool();

	bool					Connect(int32 connectionCount, const WCHAR* connectionString);
	void					Clear();

	FDBConnection* Pop();
	void Push(FDBConnection* connection);

private:

	USE_LOCK;

	SQLHENV	Environment = SQL_NULL_HANDLE;
	vector<FDBConnection*>	Connections;
};

