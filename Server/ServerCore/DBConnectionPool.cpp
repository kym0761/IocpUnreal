#include "pch.h"
#include "DBConnectionPool.h"

FDBConnectionPool::FDBConnectionPool()
{
}

FDBConnectionPool::~FDBConnectionPool()
{
	Clear();
}

bool FDBConnectionPool::Connect(int32 connectionCount, const WCHAR* connectionString)
{
	WRITE_LOCK;

	if (::SQLAllocHandle(
		SQL_HANDLE_ENV,
		SQL_NULL_HANDLE,
		&Environment) != SQL_SUCCESS)
		return false;

	if (::SQLSetEnvAttr(Environment,
		SQL_ATTR_ODBC_VERSION,
		reinterpret_cast<SQLPOINTER>(SQL_OV_ODBC3),
		0) != SQL_SUCCESS)
		return false;

	for (int32 i = 0; i < connectionCount; i++)
	{
		FDBConnection* connection = new FDBConnection();
		if (connection->Connect(Environment, connectionString) == false)
			return false;

		Connections.push_back(connection);
	}

	return true;
}

void FDBConnectionPool::Clear()
{
	WRITE_LOCK;

	if (Environment != SQL_NULL_HANDLE)
	{
		::SQLFreeHandle(SQL_HANDLE_ENV, Environment);
		Environment = SQL_NULL_HANDLE;
	}

	for (FDBConnection* connection : Connections)
	{
		delete(connection);
	}

	Connections.clear();
}

FDBConnection* FDBConnectionPool::Pop()
{
	WRITE_LOCK;

	if (Connections.empty())
		return nullptr;

	FDBConnection* connection = Connections.back();
	Connections.pop_back();
	return connection;
}

void FDBConnectionPool::Push(FDBConnection* connection)
{
	WRITE_LOCK;
	Connections.push_back(connection);
}
