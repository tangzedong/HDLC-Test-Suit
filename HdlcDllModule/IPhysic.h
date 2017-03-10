#pragma once
class ISend
{
public:
	virtual int DoSend(char* strToSend) = 0;
};

class IGenFrame
{
public:
	virtual void GenFrameBuf(void) = 0;
};