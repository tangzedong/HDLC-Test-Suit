#ifndef _EVENT_HANDLER
#define _EVENT_HANDLER
#include "hdlcFSM.h"
#include "framedef.h"

//event handler format: 
//int do(STATE)_(EVENT)(HdlcTcb *tcb, hdlc *frame, u_char *packetdata); 
//output: return result;
//STATE: INIT, NDM, NRM, WAITCON, WAITDIS

//#define DEFHANDLER(STATE, EVENT) int\
//		do##STATE_##EVENT(\
//			HdlcTcb *tcb, \
//			hdlc *frame,  \
//			hdlc *outframe)
//
//#define DOHANDLER(STATE, EVENT) do##STATE_##EVENT
//
//DEFHANDLER(INIT, OverLength);
//
////SNRM packet recieve;
//DEFHANDLER(NDM, SNRM);
//DEFHANDLER(WAITCON, SNRM);
//DEFHANDLER(NRM, SNRM);
//DEFHANDLER(WAITDIS, SNRM);
//
////DISC packet recieve
//DEFHANDLER(NDM, DISC);
//DEFHANDLER(WAITCON, DISC);
//DEFHANDLER(NRM, DISC);
//DEFHANDLER(WAITDIS, DISC);
//
////RR 
//DEFHANDLER(NDM, RR);
//DEFHANDLER(WAITCON, RR);
//DEFHANDLER(NRM, RR);
//DEFHANDLER(WAITDIS, RR);
////RNR
//DEFHANDLER(NDM, RNR);
//DEFHANDLER(WAITCON, RNR);
//DEFHANDLER(NRM, RNR);
//DEFHANDLER(WAITDIS, RNR);
////I 
//DEFHANDLER(NDM, I);
//DEFHANDLER(WAITCON, I);
//DEFHANDLER(NRM, I);
//DEFHANDLER(WAITDIS, I);
#endif