#pragma once
class CTimeList
{
	typedef struct timeline 
	{
		DWORD timing;
		int data;
		struct timeline *next;
	}timeline;
public:
	CTimeList();
	~CTimeList();
	void Append(DWORD timing, int data);
	void RemoveForward();
	int GetDataForward();
	DWORD GetTimeForward();
	BOOL IsEmpty();

private:
	timeline *list;
	timeline *tail;
};

