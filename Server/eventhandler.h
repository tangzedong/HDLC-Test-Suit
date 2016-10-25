#ifndef _EVENT_HANDLER
#define _EVENT_HANDLER
#include "hdlcFSM.h"
#include "framedef.h"

#define DEFHANDLER(name) int\
	H##name(HdlcTcb* tcb, hdlc* frame, hdlc* outframe)
#define _HANDLER(name) H##name
#define GETHANDLER(state) (*(fsmstack->listhandler[state]))

typedef int(*statehandler)(HdlcTcb*, hdlc*, hdlc*);

#define StateHandler int(*)(HdlcTcb*, hdlc*, hdlc*)
//statehandler listStateHandler[];
statehandler PrimaryStateHandler[];

statehandler PrehandleStateHandler[];

statehandler NRMStateHandler[];

statehandler IRStateHandler[];
DEFHANDLER(hdlcStateNDM);
DEFHANDLER(hdlcStateWCN);
DEFHANDLER(hdlcStateNRM);
DEFHANDLER(hdlcStateWDC);

#endif