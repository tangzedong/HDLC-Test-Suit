#pragma once
#include <afxstr.h>
class ISend
{
public:
	virtual int DoSend(CString strToSend) = 0;
};

class IGenFrame
{
public:
	virtual void GenFrameBuf(void) = 0;
};