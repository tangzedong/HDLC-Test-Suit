#include "stdafx.h"
#include "hdlc.h"
#include "hdlcutil.h"
#include "hdlcFSM.h"
#include "hdlceventhandler.h"
//HdlcTcb *fsmstack = NULL;

HdlcTcb* FSMinit(HdlcTcb *fsmstack)
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
	return fsmstack;
}

HdlcTcb* FSMreturn(HdlcTcb *fsmstack)
{
	assert(0);
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
		return fsmstack;
	}
	return fsmstack;
}

//int FSMenter2(u_int fsmtype)
//{
//	HdlcTcb *tcb = new HdlcTcb();
//	tcb->fsmtype = fsmtype;
//	tcb->listhandler = StateHandlers[fsmtype];
//	tcb->curstate = 0;
//	tcb->prev = NULL;
//	tcb->next = NULL;
//	if (fsmstack != NULL)
//	{
//		fsmstack->next = tcb;
//		tcb->prev = fsmstack;
//		tcb->next = NULL;
//		fsmstack = tcb;
//	}
//	else
//	{
//		fsmstack = tcb;
//		fsmstack->next = NULL;
//		fsmstack->prev = NULL;
//	}
//	return 0;
//}

HdlcTcb* FSMenter(HdlcTcb *fsmstack, u_int fsmtype)
{
	HdlcTcb *tcb = new HdlcTcb();
	tcb->fsmtype = fsmtype;
	tcb->listhandler = StateHandlers[fsmtype];
	tcb->curstate = 0;
	tcb->prev = NULL;
	tcb->next = NULL;
	if (fsmstack != NULL  )
	{
		if (fsmstack->prev != NULL)
		{
			delete fsmstack->prev;
			fsmstack->prev = NULL;
			fsmstack->next = tcb;
			tcb->prev = fsmstack;
			fsmstack = tcb;
		}
		else
		{
			fsmstack->next = tcb;
			tcb->prev = fsmstack;
			fsmstack = tcb;
		}
	}
	else
	{
		fsmstack = tcb;
	}
	return fsmstack;
}

HdlcTcb* FSMclean(HdlcTcb *fsmstack)
{
	if (fsmstack != NULL)
	{
		for (HdlcTcb *h = fsmstack; h != NULL; h = fsmstack->prev)
		{
			delete h;
		}

	}
	return fsmstack;
}