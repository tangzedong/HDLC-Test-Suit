#pragma once
#include <windows.h>
class IAppendLog
{
public:
	virtual void AppendLog(LPTSTR str) = 0;
};