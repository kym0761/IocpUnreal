#pragma once
#include <vector>
#include "Types.h"

class FFileUtils
{
public:
	static vector<BYTE>		ReadFile(const WCHAR* path);
	static wstring			Convert(string str);
};