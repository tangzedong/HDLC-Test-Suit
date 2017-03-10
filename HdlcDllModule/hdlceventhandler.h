#ifndef _EVENT_HANDLER
#define _EVENT_HANDLER

//×´Ì¬´¦Àíº¯Êý

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