#ifndef _EVENT_HANDLER
#define _EVENT_HANDLER
#include "hdlcFSM.h"
#include "framedef.h"

#define StateHandler int(**)(HdlcStationParam*, hdlc*, hdlc*)
#define DEFHANDLER(name) int H##name(HdlcStationParam* stpar, hdlc* frame, hdlc* outframe)
#define _HANDLER(name) H##name
#define GETHANDLER(state) (*(fsmstack->listhandler[state]))

typedef int(*statehandler)(HdlcStationParam*, hdlc*, hdlc*);


//statehandler listStateHandler[];
statehandler PrimaryStateHandler[];
statehandler PrehandleStateHandler[];
statehandler NRMStateHandler[];
statehandler IRStateHandler[];
statehandler *StateHandlers[];
//״̬��������

int HhdlcStateNDM(HdlcStationParam* stpar, hdlc* frame, hdlc* outframe);
int HhdlcStateWCN(HdlcStationParam* stpar, hdlc* frame, hdlc* outframe);
int HhdlcStateNRM(HdlcStationParam* stpar, hdlc* frame, hdlc* outframe);
int HhdlcStateWDC(HdlcStationParam* stpar, hdlc* frame, hdlc* outframe);
int HhdlcStateFRMR(HdlcStationParam* stpar, hdlc* frame, hdlc* outframe);

int HhdlcStateNRMIdle(HdlcStationParam* stpar, hdlc* frame, hdlc* outframe);
int HhdlcStateNRMHandle2(HdlcStationParam* stpar, hdlc* frame, hdlc* outframe);
int HhdlcStateNRMIR(HdlcStationParam* stpar, hdlc* frame, hdlc* outframe);


int HhdlcStateIRWaitCmd(HdlcStationParam* stpar, hdlc* frame, hdlc* outframe);
int HhdlcStateIRWaitRes(HdlcStationParam* stpar, hdlc* frame, hdlc* outframe);
int HhdlcStateIRHandle3(HdlcStationParam* stpar, hdlc* frame, hdlc* outframe);
int HhdlcStateIRSendData(HdlcStationParam* stpar, hdlc* frame, hdlc* outframe);
#endif