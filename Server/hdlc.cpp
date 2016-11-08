#include "stdafx.h"
#include "framedef.h"

u_char settingdata[23] = { 0x81, 0x80, 0x14, 0x05, 0x02, 0x00, 0x80, 0x06, 0x02, 0x00,
0x80, 0x07, 0x04, 0x00, 0x00, 0x00, 0x01, 0x08, 0x04, 0x00, 0x00, 0x00, 0x01 };
int glen=3;
u_char errorcode[3]={ 0x10, 0x00, 0x20};
int gUAdatalen=23;
hdlc gUIFrame;
u_char gUIInfoBuf[255];
u_int gUIInfoLen;

HdlcStationParam *gstpar;

int convFrameStr(hdlc *frame, _TCHAR* S)
{
	genFrameData(S, NULL, frame);
	return 0;
}
int convFrameHex(hdlc *frame, u_char* pData)
{
	genFrameData(NULL, pData, frame);
	return 0;
}
int convStrFrame(_TCHAR* S, hdlc *frame)
{
	CString tempstr(S);
	u_char *pData = new u_char[tempstr.GetLength()];
	convStrHex(S, pData);
	convHexFrame(pData, frame);
	return 0;
}


int convHexFrame(u_char *pData, hdlc *hdlc_p)
{
	int infolen;
	int d_addr_length, s_addr_length;
	u_char *pfront = pData;
	u_char _byte, _word, _longword;
	hdlc_p->start_flag = *pfront;//将标志位存入
	if (hdlc_p->start_flag != STARTFLAG)
	{
		//m_liststateinfo.AddString(_T("开始标志位错误！"));
		return ERROR_STARTFLAG;
	}
	pfront++;
	//hdlc_p->f_format.frame_type = (str[1]&160)/16;
	hdlc_p->f_format.frame_type = (*pfront >> 4) << 4;//取出前4位并右移
	hdlc_p->f_format.frame_seg = (*pfront & 0x08) >> 3;//取出分段位右移
	hdlc_p->f_format.frame_sublen = *((u_short*)pfront);
	hdlc_p->f_format.frame_sublen = (hdlc_p->f_format.frame_sublen >> 8 | hdlc_p->f_format.frame_sublen << 8) & 0x07FF;//11位帧长度子域
	pfront += 2;
	//得到目的地址的长度, 对目的地址进行赋值
	if ((*pfront & 1) == 1)
	{
		d_addr_length = 1;
		hdlc_p->dst_addrlen = d_addr_length;
		hdlc_p->dst_addr = *pfront >> 1;
		pfront ++;
	}
	else if (((*pfront & 1) == 0) && (*(pfront+1) & 1) == 1)
	{
		d_addr_length = 2;
		hdlc_p->dst_addrlen = d_addr_length;
		hdlc_p->dst_addr = u_short(*((u_short*)pfront) >> 9 | (*((u_short*)pfront) >> 1) << 7);//强制类型转换成小端
		pfront += 2;
	}
	else if (((*pfront & 1) == 0) && ((*(pfront+1) & 1) == 0) && ((*(pfront+2) & 1) == 0) && ((*(pfront+3) & 1) == 1))
	{
		d_addr_length = 4;
		hdlc_p->dst_addrlen = d_addr_length;
		u_int temp = *((u_int*)pfront);
		hdlc_p->dst_addr = (temp >> 25) | ((temp & 0xFE0000)  >> 11) | ((temp & 0xFE00) << 6) | ((temp & 0xFE) << 21);
		pfront += 4;
	}
	else
	{
		// m_liststateinfo.AddString(_T("目的地址长度不正确！"));
		return ERROR_DST_ADDR_LEN;
	}

	if ((*pfront & 1) == 1)
	{
		s_addr_length = 1;
		hdlc_p->src_addrlen = s_addr_length;
		hdlc_p->src_addr = *pfront >> 1;
		pfront++;
	}
	else if (((*pfront & 1) == 0) && (*(pfront + 1) & 1) == 1)
	{
		s_addr_length = 2;
		hdlc_p->src_addrlen = s_addr_length;
		hdlc_p->src_addr = u_short(*((u_short*)pfront) >> 9 | (*((u_short*)pfront) >> 1) << 7);//强制类型转换成小端
		pfront += 2;
	}
	else if (((*pfront & 1) == 0) && ((*(pfront + 1) & 1) == 0) && ((*(pfront + 2) & 1) == 0) && ((*(pfront + 3) & 1) == 1))
	{
		s_addr_length = 4;
		hdlc_p->src_addrlen = s_addr_length;
		u_int temp = *((u_int*)pfront);
		hdlc_p->src_addr = (temp >> 25) | ((temp & 0xFE0000) >> 11) | ((temp & 0xFE00) << 6) | ((temp & 0xFE) << 21);
		pfront += 4;
	}
	else
	{
		// m_liststateinfo.AddString(_T("源地址长度不正确！"));
		return ERROR_SRC_ADDR_LEN;
	}

	hdlc_p->frame_ctl = *pfront;//取出控制域的数据
	if (hdlc_p->frame_ctl & 0) //I帧 取出nr和ns域
	{
		hdlc_p->nr = hdlc_p->frame_ctl >> 5;
		hdlc_p->ns = hdlc_p->frame_ctl >> 1 & 0x07;
		
	}
	else if ((hdlc_p->frame_ctl & 0x0A) == 0) //RR和RNR帧 取出nr域
	{
		hdlc_p->nr = hdlc_p->frame_ctl >> 5;
		hdlc_p->ns = 0;
	}
	hdlc_p->pollfin = hdlc_p->frame_ctl >> 4 & 1; //取出p\f位
	pfront++;

	hdlc_p->h_cs = *((u_short*)pfront) >> 8 | *((u_short*)pfront) << 8;//2字节的头校验序列
	pfront += 2;

	if (hdlc_p->f_format.frame_sublen <= 10)//信息域为空
	{
		infolen = 0;
		memset(hdlc_p->info_buff, 0, MAX_LEN);
		hdlc_p->end_flag = *((u_short*)(pfront + infolen));//帧结束标志
		return 0;
	}

	//信息域不为空
	infolen = (hdlc_p->f_format.frame_sublen) - d_addr_length - s_addr_length - 7;
	hdlc_p->infolen = infolen;
	if (infolen > 0x80)
	{
		//m_liststateinfo.AddString(_T("信息域超长，超出协商的最大信息长度！"));
		return ERROR_INF_OVERFLOW;
	}
	for (u_char rcv_cnt = 0; rcv_cnt < infolen; rcv_cnt++)
	{
		hdlc_p->info_buff[rcv_cnt] = *pfront;
		pfront++;
	}   //将信息存入
		
	hdlc_p->f_cs = *(u_short*)pfront << 8 | *(u_short*)pfront >> 8;
	hdlc_p->end_flag = *(pfront + 2);//帧结束标志

	return 0;
}
int convHexStr(u_char *pData, _TCHAR* S, int len)
{
	int index = 0;
	for (int i = 0; i < len; i++)
	{
		index = outHexStr(S, NULL, index, NULL, pData[i]);
	}
	return 0;
}

int convStrHex(_TCHAR* S, u_char* pData)
{
	CString buff(S);
	_TCHAR curchar;
	int tmp;
	int  nReceivedLen = buff.GetLength();
	int f_val = 0;
	for (u_int i = 0; i < (nReceivedLen/2); i++)
	{
		//高四位
		curchar = buff.GetAt(f_val);
		if (isdigit(curchar))
			tmp = curchar - 48;					//读取的字符为数字
		else if (isalpha(curchar))
			tmp = curchar - 55;					//读取的字符为（大写）字母
		pData[i] = (tmp << 4);
		f_val++;
		curchar = buff.GetAt(f_val);
		//低四位
		if (isdigit(curchar))
			tmp = (curchar)-48;					//读取的字符为数字
		else if (isalpha(curchar))
			tmp = (curchar)-55;					//读取的字符为（大写）字母
		pData[i] |= (u_char)tmp;
		f_val++;
	}
	return 0;
}

int outHexStr(_TCHAR* S, u_char *pRawData, int index, int *bytesidx, u_char byte){
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

void genFrameData(_TCHAR *S, u_char *pRawData, hdlc *frame){
	int rs;
	int index = 0;
	int bytesidx = 0;
	u_short typebytes = 0;
	index = outHexStr(S, pRawData, index, &bytesidx, frame->start_flag);

	typebytes = ((u_int)frame->f_format.frame_type << 8) | (frame->f_format.frame_sublen & 0x3F);
	typebytes = (typebytes & 0xBFFF) | ((u_int)frame->f_format.frame_seg << 13);
	index = outHexStr(S, pRawData, index, &bytesidx, (u_char)(typebytes >> 8));
	index = outHexStr(S, pRawData, index, &bytesidx, (u_char)(typebytes & 0xFF));

	u_char addrbyte, ctlbyte;
	int i;
	for (i = frame->dst_addrlen - 1; i >= 0; i--)
	{
		addrbyte = (frame->dst_addr >> (i * 7)) & 0x7F;
		addrbyte = addrbyte << 1;
		if (i == 0) addrbyte |= 1;
		index = outHexStr(S, pRawData, index, &bytesidx, addrbyte);
	}

	for (i = frame->src_addrlen - 1; i >= 0; i--)
	{
		addrbyte = (frame->src_addr >> (i * 7)) & 0x7F;
		addrbyte = addrbyte << 1;
		if (i == 0) addrbyte |= 1;
		index = outHexStr(S, pRawData, index, &bytesidx, addrbyte);
	}

	ctlbyte = frame->frame_ctl | (frame->nr << 5) | (frame->ns << 1) | (frame->pollfin << 4);
	index = outHexStr(S, pRawData, index, &bytesidx, ctlbyte);

	pRawData&&(frame->h_cs = cs_cal(pRawData + 1, frame->dst_addrlen + frame->src_addrlen + 3));
	index = outHexStr(S, pRawData, index, &bytesidx, frame->h_cs >> 8);
	index = outHexStr(S, pRawData, index, &bytesidx, frame->h_cs & 0xFF);

	if (frame->infolen == 0)//info field empty.
	{
		index = outHexStr(S, pRawData, index, &bytesidx, 0x7E);
		S ? S[index] = '\0' : 0;
		return;
	}

	//construct information field.
	for (int i = 0; i < frame->infolen; i++)
	{
		index = outHexStr(S, pRawData, index, &bytesidx, frame->info_buff[i]);
	}
	pRawData && (frame->f_cs = cs_cal(pRawData + 1, frame->infolen + frame->dst_addrlen + frame->src_addrlen + 5));
	index = outHexStr(S, pRawData, index, &bytesidx, (frame->f_cs >> 8));
	index = outHexStr(S, pRawData, index, &bytesidx, frame->f_cs & 0xFF);
	
	index = outHexStr(S, pRawData, index, &bytesidx, frame->end_flag);
	S?S[index] = '\0':0;
	return;
}

u_int GetTypes(hdlc &frame)
{
	if ((frame.frame_ctl & MASKI) == 0)
	{
		return TYPEI;
	}
	else if ((frame.frame_ctl & MASKRR) == TYPERR)
	{
		return TYPERR;
	}
	else if ((frame.frame_ctl & MASKRNR) == TYPERNR)
	{
		return TYPERNR;
	}
	else if ((frame.frame_ctl & MASKSNRM) == TYPESNRM)
	{
		return TYPESNRM;
	}
	else if ((frame.frame_ctl & MASKDISC) == TYPEDISC)
	{
		return TYPEDISC;
	}
	else if ((frame.frame_ctl & MASKUA) == TYPEUA)
	{
		return TYPEUA;
	}
	else if ((frame.frame_ctl & MASKDM) == TYPEDM)
	{
		return TYPEDM;
	}
	else if ((frame.frame_ctl & MASKFRMR) == TYPEFRMR)
	{
		return TYPEFRMR;
	}
	else if ((frame.frame_ctl & MASKUI) == TYPEUI)
	{
		return TYPEUI;
	}
	return 0xFFFFFFFF;
}


int HdlcSetOpt(HdlcStationParam *par, hdlcpointer frame){
	return 0;
}

void HdlcParamInit(){
	gstpar->nr = 0;
	gstpar->ns = 0;
	gstpar->started = 0;
	gstpar->frame_p_f = 0;
	gstpar->rcv_num = 0;
	gstpar->send_num = 0;
	gstpar->frmr_flag = 0;
	gstpar->disc = 1;
	gstpar->isUIWaiting = 0;
	gstpar->canUISend = 0;
	gstpar->isTransFinish = 1;
}

int HdlcSetParam(u_char *paramstr, u_int len)
{
	return 0;
}