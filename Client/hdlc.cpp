#include "stdafx.h"
#include "hdlc.h"
int outHexStr(_TCHAR* S, u_char *pRawData, u_int index, u_int *bytesidx, u_char byte){
	if (S!=NULL)
	{
		u_int rs = _stprintf(S + index, _T("%02x"), byte);
		index += rs;
	}
	if (pRawData!=NULL)
	{
		pRawData[(*bytesidx)++] = (u_char)byte;
	}
	return index;
}