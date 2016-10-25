#include "framedef.h"
#include "hdlcFSM.h"

HdlcTcb *fsmstack = NULL;

int FSMinit(HdlcTcb *tcb)
{
	if (fsmstack != NULL)
	{
		for (HdlcTcb *h = fsmstack; h != NULL; h = fsmstack->next)
		{
			delete h;
		}

	}
	fsmstack = tcb;
	tcb->prev = NULL;
	tcb->next = NULL;
	return 0;
}

int FSMreturn(void)
{
	if (fsmstack != NULL)
	{
		if (fsmstack->next == NULL && fsmstack->prev == NULL)
		{
			delete fsmstack;
			fsmstack = NULL;
		}
		else if (fsmstack->prev != NULL)
		{
			fsmstack->prev->error = fsmstack->error;
			fsmstack = fsmstack->prev;
			delete fsmstack->next;
			fsmstack->next = NULL;
		}
	}
	else
	{
		return -1;
	}
	return 0;
}

int FSMenter(HdlcTcb *tcb)
{
	if (fsmstack != NULL)
	{
		fsmstack->next = tcb;
		tcb->prev = fsmstack;
		tcb->next = NULL;
		fsmstack = tcb;
	}
	else
	{
		fsmstack = tcb;
		fsmstack->next = NULL;
		fsmstack->prev = NULL;
	}
	return 0;
}