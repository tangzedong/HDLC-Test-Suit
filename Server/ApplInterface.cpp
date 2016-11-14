#include "framedef.h"
#include "ApplInterface.h"
#include "ServerDlg.h"
#include "eventhandler.h"
#include <Windows.h>

u_char _sendbuf[2048] = { 0 };
u_int _sendlen;
int ApplId(void)
{
	return OK;
}

int ApplSend(hdlc *frame, u_char *data, u_int len)
{
	gstpar->hWnd->SendMessage(WM_APPLMSG, (WPARAM)data, len);
	return OK;
}

int ApplGetBuf(u_char **pdata, u_int *len)
{
	gstpar->hWnd->SendMessage(WM_APPLGETDATA, (WPARAM)_sendbuf, (LPARAM)&glen);
	*pdata = _sendbuf;
	*len = _sendlen;
	return READY;
}

int ApplHdlcSend(HdlcStationParam *ccb, u_char *data, u_int len)
{
	int nSentLen;
	hdlc frame, outframe;
	u_char *write_str = new u_char[2048];
	for (int i = 0; i < len; i++)
	{
		_sendbuf[i] = data[i];
	}
	_sendlen = len;
	ccb->sendbuf = _sendbuf;
	ccb->sendlen = _sendlen;
	fsmstack->curstate = STATE_WAIT_RES; 
	do{
		ccb->isTransFinish = 1;
		GETHANDLER(fsmstack->curstate)(ccb, NULL, &outframe);


		if (ccb->isUIWaiting == 1 && ccb->canUISend == 1)
		{
			_TCHAR  sendStr[1024];
			u_char sendData[255];
			convFrameHex(&gUIFrame, write_str);
			convFrameStr(&gUIFrame, sendStr);
			CString *m_strToServer = new CString(sendStr);
			//writetxt((FILE*)wc, write_str, outframe.f_format.frame_sublen);
			ccb->hWnd->SendMessage(WM_WRITETOFILE, (WPARAM)write_str, (LPARAM)outframe.f_format.frame_sublen);
			nSentLen = ccb->hWnd->m_sClientSocket.Send(*m_strToServer, 1024);
			if (nSentLen != SOCKET_ERROR)
			{
				//发送成功
				ccb->hWnd->SendMessage(WM_INFO, (WPARAM)_T("发送UI帧"));
				ccb->hWnd->SendMessage(WM_SENDLOG, (WPARAM)(m_strToServer));
			}
			ccb->canUISend = 0;
			ccb->isUIWaiting = 0;
		}

		if (ccb->send_flag == 1)
		{
			_TCHAR  sendStr[1024];
			u_char sendData[255];
			convFrameHex(&outframe, write_str);
			convFrameStr(&outframe, sendStr);
			CString *m_strToServer = new CString(sendStr);
			ccb->hWnd->SendMessage(WM_WRITETOFILE, (WPARAM)write_str, (LPARAM)outframe.f_format.frame_sublen);
			nSentLen = ccb->hWnd->m_sClientSocket.Send(*m_strToServer, 1024);
			if (nSentLen != SOCKET_ERROR)
			{
				//发送成功
				ccb->hWnd->SendMessage(WM_SENDLOG, (WPARAM)(m_strToServer));
			}
			ccb->send_flag = 0;
		}

	} while (!ccb->isTransFinish);
	return 0;
}
