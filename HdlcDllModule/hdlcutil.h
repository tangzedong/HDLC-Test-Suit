#ifndef __HDLC_UTIL
#define __HDLC_UTIL

//convert functionss
int convFrameStr(HdlcStationParam *stpar, hdlc *frame, _TCHAR* S);
int convFrameHex(HdlcStationParam *stpar, hdlc *frame, u_char* pData);
int convStrFrame(HdlcStationParam *stpar, _TCHAR* S, hdlc *frame);
int convHexFrame(HdlcStationParam *stpar, u_char *pData, hdlc *hdlc_p, int *exception);
int convHexStr(HdlcStationParam *stpar, u_char *pData, _TCHAR* S, int len);
int convStrHex( _TCHAR* S, u_char* pData);
int outHexStr(_TCHAR* S, u_char *pRawData, int index, int *bytesidx, u_char byte);
void genFrameData(_TCHAR *S, u_char *pRawData, hdlc *frame);

//read frame and error checing
int readFrame(u_char *pData, u_int len, hdlc *outframe, int *exception);
int RuntimeFrameCheck(HdlcStationParam *stpar, hdlc *pframe);

//frame generation functions
int makeRR(HdlcStationParam *stpar, hdlc *frame, hdlc *outframe, u_int nr, u_int pf);
int makeRNR(HdlcStationParam *stpar, hdlc *frame, hdlc *outframe, u_int nr, u_int pf);
int makeUI(HdlcStationParam *stpar, hdlc *frame, hdlc *outframe, u_char* data, u_int infolen);
int makeSNRM(HdlcStationParam *stpar, hdlc *frame, hdlc *outframe);
int makeDISC(HdlcStationParam *stpar, hdlc *frame, hdlc *outframe);
int makeDM(HdlcStationParam *stpar, hdlc *frame, hdlc *outframe);
int makeFRMR(HdlcStationParam *stpar, hdlc *frame, hdlc *outframe, u_char *infobuf, u_int infolen);
int makeUA(HdlcStationParam *stpar, hdlc *frame, hdlc *outframe, u_char *settingdata, u_int len);
int makeI(HdlcStationParam *stpar, hdlc *frame, hdlc *outframe, u_char *infobuf, u_int infolen, u_int seg, u_int pf, u_char rcv = 0, u_char snd = 0);
u_int GetTypes(hdlc &frame);
char *GetTypestr(hdlc &frame);
//transfer parameters setting
int HdlcSetOpt(HdlcStationParam *stpar, hdlcpointer frame);
void HdlcParamInit(HdlcStationParam *stpar);
int HdlcSetParam(HdlcStationParam *stpar, u_char* paramstr, u_int len);
int HdlcTransParamInit(HdlcStationParam *stpar);
#endif