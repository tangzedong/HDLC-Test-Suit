#ifndef _APPLINT
#define _APPLINT
enum
{
	OK = 1,
	DENY,
	WAITFOR,
	READY
};
extern u_char _sendbuf[];
extern u_int _sendlen;
int ApplId(void);
int ApplSend(HdlcStationParam *stpar, hdlc *frame, u_char *data, u_int len);
int ApplGetBuf(HdlcStationParam *stpar, u_char **pdata, u_int* len);
int ApplHdlcSend(HdlcStationParam *stpar, u_char *data, u_int len);
#endif