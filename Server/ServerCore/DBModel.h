#pragma once
#include "Types.h"

NAMESPACE_BEGIN(DBModel)

USING_SHARED_PTR(Column);
USING_SHARED_PTR(Index);
USING_SHARED_PTR(Table);
USING_SHARED_PTR(Procedure);

/*-------------
	DataType
--------------*/


//windows DB 환경에서 사용하는 데이터 타입 번호
//DB 시스템에 따라 데이터 타입 번호가 아예 다를 수도 있으니 주의할 것
enum class DataType
{
	None = 0,
	TinyInt = 48,
	SmallInt = 52,
	Int = 56,
	Real = 59,
	DateTime = 61,
	Float = 62,
	Bit = 104,
	Numeric = 108,
	BigInt = 127,
	VarBinary = 165,
	Varchar = 167,
	Binary = 173,
	NVarChar = 231,
};

/*-------------
	Column
--------------*/

class Column
{
public:
	wstring				CreateText();

public:
	wstring				_name;
	int32				_columnId = 0; // DB
	DataType			_type = DataType::None;
	wstring				_typeText;
	int32				_maxLength = 0;
	bool				_nullable = false;
	bool				_identity = false;
	int64				_seedValue = 0;
	int64				_incrementValue = 0;
	wstring				_default;
	wstring				_defaultConstraintName; // DB
};

/*-----------
	Index
------------*/

enum class IndexType
{
	Clustered = 1,
	NonClustered = 2
};

class Index
{
public:
	wstring				GetUniqueName();
	wstring				CreateName(const wstring& tableName);
	wstring				GetTypeText();
	wstring				GetKeyText();
	wstring				CreateColumnsText();
	bool				DependsOn(const wstring& columnName);

public:
	wstring				_name; // DB
	int32				_indexId = 0; // DB
	IndexType			_type = IndexType::NonClustered;
	bool				_primaryKey = false;
	bool				_uniqueConstraint = false;
	vector<ColumnRef>	_columns;
};

/*-----------
	Table
------------*/

class Table
{
public:
	ColumnRef			FindColumn(const wstring& columnName);

public:
	int32				_objectId = 0; // DB
	wstring				_name;
	vector<ColumnRef>	_columns;
	vector<IndexRef>	_indexes;
};

/*----------------
	Procedures
-----------------*/

struct Param
{
	wstring				_name;
	wstring				_type;
};

class Procedure
{
public:
	wstring				GenerateCreateQuery();
	wstring				GenerateAlterQuery();
	wstring				GenerateParamString();

public:
	wstring				_name;
	wstring				_fullBody; // DB
	wstring				_body; // XML
	vector<Param>		_parameters;  // XML
};

/*-------------
	Helpers
--------------*/

class Helpers
{
public:
	static wstring		Format(const WCHAR* format, ...);
	static wstring		DataType2String(DataType type);
	static wstring		RemoveWhiteSpace(const wstring& str);
	static DataType		String2DataType(const WCHAR* str, OUT int32& maxLen);
};

NAMESPACE_END