#include "stdafx.h"
#include "TimeList.h"


CTimeList::CTimeList() :list(NULL), tail(NULL)
{
}


CTimeList::~CTimeList()
{
}

void CTimeList::Append(DWORD timing, int data){
	if (list == NULL)
	{
		list = new timeline{ timing, data, NULL };
		tail = list;
		return;
	}
	tail->next = new timeline{ timing, data, NULL };
	tail = tail->next;
}

void CTimeList::RemoveForward(){
	timeline *p;
	if (list == NULL)
		return;
	p = list;
	list = list->next;
	delete p;
}

int CTimeList::GetDataForward(){
	if (list != NULL)
	{
		return list->data;
	}
	return 0;
}

DWORD CTimeList::GetTimeForward(){
	if (list != NULL)
	{
		return list->timing;
	}
	return 0;
}

BOOL CTimeList::IsEmpty(){
	if (list == NULL)
		return TRUE;
	return FALSE;
}