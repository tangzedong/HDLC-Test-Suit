#include "framedef.h"
#include "ApplInterface.h"
#include <Windows.h>

u_char _sendbuf[2048] = { 0 };
u_int glen;
int ApplId(void)
{
	return OK;
}

int ApplSend(u_char *data, u_int len)
{
	gstpar->hWnd->SendMessage(WM_USER + 3, (WPARAM)data, len);
	return OK;
}

int ApplGetBuf(u_char **pdata, u_int *len)
{
	gstpar->hWnd->SendMessage(WM_USER + 4, (WPARAM)_sendbuf, (LPARAM)&glen);
	*pdata = _sendbuf;
	*len = glen;
	return READY;
}

