#ifndef _HDLC_FSM
#define _HDLC_FSM
#include "framedef.h"
#include "eventhandler.h"
enum{
	STATE_NDM=1,
	STATE_WAIT_CONNECT,
	STATE_NRM,
	STATE_WAIT_DISCONNECT
};
typedef struct
{
	int curstate;
	int nxtstate;
	int condition;
	int start;
	int term;
	int sendnum;
	int recnum;
}HdlcTcb;
#endif