#include "framedef.h"
#include "hdlcFSM.h"
#include "eventhandler.h"
HdlcTcb *fsmstack = NULL;

int FSMinit()
{
	if (fsmstack != NULL)
	{
		for (HdlcTcb *h = fsmstack->prev; fsmstack != NULL; (fsmstack = h) && (h = fsmstack->prev))
		{
			delete fsmstack;
		}

	}
	HdlcTcb *pTcb = new HdlcTcb();
	pTcb->listhandler = (StateHandler)PrimaryStateHandler;
	fsmstack = pTcb;
	fsmstack->curstate = STATE_NDM;//³õÊ¼»¯×´Ì¬»ú
	pTcb->prev = NULL;
	pTcb->next = NULL;
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

int FSMenter(u_int fsmtype)
{
	HdlcTcb *tcb = new HdlcTcb();
	tcb->fsmtype = fsmtype;
	tcb->listhandler = StateHandlers[fsmtype];
	tcb->curstate = 0;
	tcb->prev = NULL;
	tcb->next = NULL;
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

int FSMclean(HdlcTcb *tcb)
{
	if (fsmstack != NULL)
	{
		for (HdlcTcb *h = fsmstack; h != NULL; h = fsmstack->prev)
		{
			delete h;
		}

	}
	return 0;
}