#include "stdafx.h"
#include "hdlc.h"
#include "hdlcutil.h"
#include "hdlcFSM.h"
#include "ApplInterface.h"
#include "ServerDlg.h"


u_char _sendbuf[2048] = { 0 };
u_int _sendlen;

//************************************
// Method:    ApplId
// FullName:  Application layer connection identifying.
// Access:    public 
// Returns:   int
// Qualifier:
// Parameter: void
//************************************
int ApplId(void)
{
	return OK;
}

//************************************
// Method:    ApplSend
// FullName:  Send frame to application layer.
// Access:    public 
// Returns:   int
// Qualifier:
// Parameter: hdlc * frame
// Parameter: u_char * data
// Parameter: u_int len
//************************************
int ApplSend(HdlcStationParam *stpar, hdlc *frame, u_char *data, u_int len)
{
	stpar->hWnd->SendMessage(WM_APPLMSG, (WPARAM)data, len);
	return OK;
}

int ApplGetBuf(HdlcStationParam *stpar, u_char **pdata, u_int *len)
{
	stpar->hWnd->SendMessage(WM_APPLGETDATA, (WPARAM)_sendbuf, (LPARAM)&glen);
	*pdata = _sendbuf;
	*len = _sendlen;
	return READY;
}

//************************************
// Method:    ApplHdlcSend
// FullName:  Send I frame called from application layer.
// Access:    public 
// Returns:   int
// Qualifier:
// Parameter: HdlcStationParam * ccb
// Parameter: u_char * data
// Parameter: u_int len
//************************************
int ApplHdlcSend(HdlcStationParam *stpar, u_char *data, u_int len)
{
	int nSentLen;
	hdlc frame, outframe;
	HdlcTcb *fsmstack = stpar->fsmstack;
	u_char *write_str = new u_char[2048];
	for (int i = 0; i < len; i++)
	{
		_sendbuf[i] = data[i];
	}
	_sendlen = len;
	stpar->sendbuf = _sendbuf;
	stpar->sendlen = _sendlen;
	fsmstack->curstate = STATE_WAIT_RES; 
	do{
		stpar->isTransFinish = 1;
		GETHANDLER(fsmstack)(stpar, NULL, &outframe);


		if (stpar->isUIWaiting == 1 && stpar->canUISend == 1)
		{
			_TCHAR  sendStr[1024];
			u_char sendData[255];
			convFrameHex(stpar, &(stpar->hWnd->m_UIFrame), write_str);
			convFrameStr(stpar, &(stpar->hWnd->m_UIFrame), sendStr);
			CString *m_strToServer = new CString(sendStr);
			//writetxt((FILE*)wc, write_str, outframe.f_format.frame_sublen);
			stpar->hWnd->SendMessage(WM_WRITETOFILE, (WPARAM)write_str, (LPARAM)outframe.f_format.frame_sublen);
			nSentLen = stpar->hWnd->pParent->DoSend(*m_strToServer);
			if (nSentLen != SOCKET_ERROR)
			{
				//发送成功
				stpar->hWnd->SendMessage(WM_INFO, (WPARAM)_T("发送UI帧"));
				stpar->hWnd->SendMessage(WM_SENDLOG, (WPARAM)(m_strToServer));
			}
			stpar->canUISend = 0;
			stpar->isUIWaiting = 0;
		}

		if (stpar->send_flag == 1)
		{
			_TCHAR  sendStr[1024];
			u_char sendData[255];
			convFrameHex(stpar,&outframe, write_str);
			convFrameStr(stpar,&outframe, sendStr);
			CString *m_strToServer = new CString(sendStr);
			stpar->hWnd->SendMessage(WM_WRITETOFILE, (WPARAM)write_str, (LPARAM)outframe.f_format.frame_sublen);
			nSentLen = stpar->hWnd->pParent->DoSend(*m_strToServer);
			if (nSentLen != SOCKET_ERROR)
			{
				//发送成功
				stpar->hWnd->SendMessage(WM_SENDLOG, (WPARAM)(m_strToServer));
			}
			stpar->send_flag = 0;
		}

	} while (!stpar->isTransFinish);
	return 0;
}
