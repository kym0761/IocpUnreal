#pragma once
#include "DBConnection.h"

template<int32 C>
struct FullBits { enum { value = (1 << (C - 1)) | FullBits<C - 1>::value }; };

//c = 3 = 1<<2 | 1<<1 | 1<< 0

template<>
struct FullBits<1> { enum { value = 1 }; };

template<>
struct FullBits<0> { enum { value = 0 }; };

template<int32 ParamCount, int32 ColumnCount>
class FDBBind
{
public:
	FDBBind(FDBConnection& dbConnection, const WCHAR* query)
		: DBConnection(dbConnection), Query(query)
	{
		//초기화
		::memset(ParamIndex, 0, sizeof(ParamIndex));
		::memset(ColumnIndex, 0, sizeof(ColumnIndex));
		ParamFlag = 0;
		ColumnFlag = 0;
		dbConnection.Unbind();
	}

	bool Validate()
	{
		return ParamFlag 
			== FullBits<ParamCount>::value && ColumnFlag == FullBits<ColumnCount>::value;
	}

	bool Execute()
	{
		ASSERT_CRASH(Validate());
		return DBConnection.Execute(Query);
	}

	bool Fetch()
	{
		return DBConnection.Fetch();
	}

public:
	template<typename T>
	void BindParam(int32 idx, T& value)
	{
		DBConnection.BindParam(idx + 1, &value, &ParamIndex[idx]);
		ParamFlag |= (1LL << idx);
	}

	void BindParam(int32 idx, const WCHAR* value)
	{
		DBConnection.BindParam(idx + 1, value, &ParamIndex[idx]);
		ParamFlag |= (1LL << idx);
	}

	template<typename T, int32 N>
	void BindParam(int32 idx, T(&value)[N])
	{
		DBConnection.BindParam(idx + 1, (const BYTE*)value, size32(T) * N, &ParamIndex[idx]);
		ParamFlag |= (1LL << idx);
	}

	template<typename T>
	void BindParam(int32 idx, T* value, int32 N)
	{
		DBConnection.BindParam(idx + 1, (const BYTE*)value, size32(T) * N, &ParamIndex[idx]);
		ParamFlag |= (1LL << idx);
	}

	template<typename T>
	void BindCol(int32 idx, T& value)
	{
		DBConnection.BindCol(idx + 1, &value, &ColumnIndex[idx]);
		ColumnFlag |= (1LL << idx);
	}

	template<int32 N>
	void BindCol(int32 idx, WCHAR(&value)[N])
	{
		DBConnection.BindCol(idx + 1, value, N - 1, &ColumnIndex[idx]);
		ColumnFlag |= (1LL << idx);
	}

	void BindCol(int32 idx, WCHAR* value, int32 len)
	{
		DBConnection.BindCol(idx + 1, value, len - 1, &ColumnIndex[idx]);
		ColumnFlag |= (1LL << idx);
	}

	template<typename T, int32 N>
	void BindCol(int32 idx, T(&value)[N])
	{
		DBConnection.BindCol(idx + 1, value, size32(T) * N, &ColumnIndex[idx]);
		ColumnFlag |= (1LL << idx);
	}

protected:
	FDBConnection& DBConnection;
	const WCHAR* Query;
	SQLLEN ParamIndex[ParamCount > 0 ? ParamCount : 1];
	SQLLEN ColumnIndex[ColumnCount > 0 ? ColumnCount : 1];
	uint64 ParamFlag;
	uint64 ColumnFlag;
};