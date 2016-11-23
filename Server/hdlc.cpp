#include "stdafx.h"
#include <Windows.h>
#include "hdlc.h"
#include "hdlcutil.h"
//u_char settingdata[23] = { 0x81, 0x80, 0x14, 0x05, 0x02, 0x00, 0x80, 0x06, 0x02, 0x00,
//0x80, 0x07, 0x04, 0x00, 0x00, 0x00, 0x01, 0x08, 0x04, 0x00, 0x00, 0x00, 0x01 };
int glen = 3;
u_char errorcode[3] = { 0x10, 0x00, 0x20 };
int gUAdatalen = 23;
hdlc gUIFrame;
u_char gUIInfoBuf[255];
u_int gUIInfoLen;

//HdlcStationParam *stpar;

int convFrameStr(HdlcStationParam *stpar, hdlc *frame, _TCHAR* S)
{
	genFrameData(S, NULL, frame);
	return 0;
}
int convFrameHex(HdlcStationParam *stpar, hdlc *frame, u_char* pData)
{
	genFrameData(NULL, pData, frame);
	return 0;
}
int convStrFrame(HdlcStationParam *stpar, _TCHAR* S, hdlc *frame)
{
	int exception;
	CString tempstr(S);
	u_char *pData = new u_char[tempstr.GetLength()];
	convStrHex(S, pData);
	convHexFrame(stpar, pData, frame, &exception);
	return 0;
}


int convHexFrame(HdlcStationParam *stpar, u_char *pData, hdlc *hdlc_p, int *exception)
{
	int infolen;
	int d_addr_length, s_addr_length;
	u_char *pfront = pData;
	u_char _byte, _word, _longword;
	*exception = 0;

	hdlc_p->start_flag = *pfront;//将标志位存入
	if (hdlc_p->start_flag != STARTFLAG)
	{
		//m_liststateinfo.AddString(_T("开始标志位错误！"));
		return ERROR_STARTFLAG;
	}

	pfront++;
	//hdlc_p->f_format.frame_type = (str[1]&160)/16;
	hdlc_p->f_format.frame_type = (*pfront >> 4) << 4;//取出前4位并右移
	if (hdlc_p->f_format.frame_type != 0xA0)//帧类型正确
	{
		return ERROR_FORMAT_TYPE;
	}
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
		pfront++;
	}
	else if (((*pfront & 1) == 0) && (*(pfront + 1) & 1) == 1)
	{
		hdlc_p->dst_addr = 0;
		d_addr_length = 2;
		hdlc_p->dst_addrlen = d_addr_length;
		u_short tempa = (*(u_short*)pfront);
		hdlc_p->dst_addr = *((u_short*)pfront) >> 9; //强制类型转换成小端
		hdlc_p->dst_addr += ((*((u_short*)pfront) & 0xFF) >> 1) << 7;
		pfront += 2;
	}
	else if (((*pfront & 1) == 0) && ((*(pfront + 1) & 1) == 0) && ((*(pfront + 2) & 1) == 0) && ((*(pfront + 3) & 1) == 1))
	{
		d_addr_length = 4;
		hdlc_p->dst_addrlen = d_addr_length;
		u_int temp = *((u_int*)pfront);
		hdlc_p->dst_addr = (temp >> 25) | ((temp & 0xFE0000) >> 11) | ((temp & 0xFE00) << 6) | ((temp & 0xFE) << 21);
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
		hdlc_p->src_addr = (*((u_short*)pfront) >> 9);//强制类型转换成小端
		hdlc_p->src_addr += ((*((u_short*)pfront) & 0xFF) >> 1) << 7;
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

	//if (hdlc_p->f_format.frame_sublen <= 10)//信息域为空
	//{
	//	infolen = 0;
	//	memset(hdlc_p->info_buff, 0, MAX_LEN);
	//	hdlc_p->end_flag = *((u_short*)(pfront + infolen));//帧结束标志
	//	return 0;
	//}

	//信息域不为空
	infolen = (hdlc_p->f_format.frame_sublen) - d_addr_length - s_addr_length - 7;
	hdlc_p->infolen = infolen;
	if (infolen > stpar->max_rcv_info_size)
	{
		//m_liststateinfo.AddString(_T("信息域超长，超出协商的最大信息长度！"));
		return ERROR_INF_OVERFLOW;
	}
	memset(hdlc_p->info_buff, 0, MAX_LEN);
	for (u_char rcv_cnt = 0; rcv_cnt < infolen; rcv_cnt++)
	{
		hdlc_p->info_buff[rcv_cnt] = *pfront;
		pfront++;
	}   //将信息存入

	if (infolen != 0)//fcs校验字节
	{
		hdlc_p->f_cs = *(u_short*)pfront << 8 | *(u_short*)pfront >> 8;
		pfront += 2;
	}

	//进行HCS校验-----------------------计算h_cs_cal
	u_short h_cs_val = h_cs_cal(pData, hdlc_p->dst_addrlen);
	if (hdlc_p->h_cs != h_cs_val)
	{
		*exception = *exception | (1 << EXCEPTION_HCS);
	}

	//进行HCS校验-----------------------计算h_cs_cal
	u_short f_cs_val = f_cs_cal(pData, hdlc_p->dst_addrlen);
	if (hdlc_p->f_cs != f_cs_val)
	{
		*exception = *exception | (1 << EXCEPTION_FCS);
	}

	hdlc_p->end_flag = *pfront;//帧结束标志
	//帧结束标志检测
	if (hdlc_p->end_flag != 0x7E)
	{
		return ERROR_ENDFLAG;
	}

	return 0;
}


int readFrame(u_char *pData, u_int len, hdlc *outframe, int *exception)
{
	int state = WAIT_FOR_FLAG;
	int i = 0, count = 0;
	*exception = 0;
	while (i < len && pData[i] != 0x7E)i++;
	i++;
	if (i >= len){
		return NO_START_FLAG;
	}
	outframe->start_flag = 0x7E;
	state = READ_TYPE;
	u_char cbyte;
	while (i < len && (cbyte = pData[i]) != 0x7E){
		//code for flag check
#ifdef __FLAGCHECK
		if (pData[i] == 0x7D)
		{
			if (i+1 < len)
			{
				cbyte = pData[i + 1] ^ 0x20;
			}
			else
			{
				break;
			}
		}
#endif
		switch (state)
		{
		case READ_TYPE:
			switch (count)
			{
			case 0:
				outframe->f_format.frame_type = ((cbyte & 0xF0) >> 4) << 4;
				outframe->f_format.frame_seg = (cbyte & 0x08) >> 3;
				outframe->f_format.frame_sublen = (cbyte & 0x7) << 8;
				count++;
				i++;
				break;
			case 1:
				//outframe->f_format.frame_sublen &= 0xFF;
				outframe->f_format.frame_sublen |= cbyte;
				count++;
				i++;
				break;
			case 2:
				state = READ_DSTADDR;
				count = 0;
				break;
			}
			break;
		case READ_DSTADDR:
			if (count == 0){
				outframe->dst_addr = 0;
			}
			outframe->dst_addr |= (cbyte >> 1) & 0x7F;
			if (cbyte & 0x01)
			{
				outframe->dst_addrlen = count + 1;
				state = READ_SRCADDR;
				count = 0;
			}
			else
			{
				outframe->dst_addr <<= 7;
				count++;
			}
			i++;
			break;
		case READ_SRCADDR:
			if (count == 0){
				outframe->src_addr = 0;
			}
			outframe->src_addr |= (cbyte >> 1) & 0x7F;
			if (cbyte & 0x01)
			{
				outframe->src_addrlen = count + 1;
				state = READ_CONTROLBYTE;
				count = 0;
			}
			else
			{
				outframe->src_addr <<= 7;
				count++;
			}
			i++;
			break;
		case READ_CONTROLBYTE:
			if (cbyte & 0x01)
			{
				outframe->nr = 0;
				outframe->ns = 0;
				outframe->pollfin = (cbyte & 0x10) >> 4;
				if (((cbyte & MASKRR) == TYPERR) || ((cbyte & MASKRNR) == TYPERNR))
				{
					outframe->nr = (cbyte & 0xE0) >> 5;
				}
			}
			else
			{
				outframe->nr = (cbyte & 0xE0) >> 5;
				outframe->ns = (cbyte & 0x0E) >> 1;
				outframe->pollfin = (cbyte & 0x10) >> 4;
			}
			outframe->frame_ctl = cbyte;
			state = READ_HCS;
			count = 0;
			i++;
			break;
		case READ_HCS:
			count++;
			if (count == 1)
			{
				outframe->h_cs = cbyte;
				outframe->h_cs <<= 8;
			}
			else if (count == 2)
			{
				outframe->h_cs |= cbyte;
				state = READ_INFO;
				outframe->infolen = 0;
				count = 0;
			}
			i++;
			break;
		case READ_INFO:
			outframe->info_buff[count] = cbyte;
			count++;
			i++;
			break;
		}
	}
	if (i < len && cbyte == 0x7E)
	{
		if (state == READ_INFO)
		{
			if (count > 2)
			{
				outframe->infolen = count - 2;
				outframe->f_cs = outframe->info_buff[count - 2];
				outframe->f_cs <<= 8;
				outframe->f_cs += outframe->info_buff[count - 1];
			}
			else if (count == 0)
			{
				outframe->f_cs = 0;
				outframe->infolen = 0;
			}
			if (count == 0 || count > 2)
			{
				outframe->end_flag = 0x7E;
				state = END_NORMAL;
			}
		}
	}
	else if (i == len)
	{
		state = WAIT_FOR_ENDFLAG;
		return NO_ENDFLAG;
	}
	if ((i - 1) != outframe->f_format.frame_sublen)
	{
		return NOT_MATCH_LEN;
	}

	switch (state)
	{
	case READ_TYPE:
		return FTYPE_FIELD_ERROR;
	case READ_DSTADDR:
		return DSTADDR_FIELD_ERROR;
	case READ_SRCADDR:
		return SRCADDR_FIELD_ERROR;
	case READ_CONTROLBYTE:
		return CONTROLBYTE_FIELD_ERROR;
	case READ_HCS:
		return HCS_FIELD_ERROR;
	case READ_INFO:
		return INFO_FIELD_ERROR;
	case END_NORMAL:
		break;
	}
	//进行HCS校验-----------------------计算h_cs_cal
	u_short h_cs_val = h_cs_cal(pData, outframe->dst_addrlen + outframe->src_addrlen + 3);
	if (outframe->h_cs != h_cs_val)
	{
		*exception = *exception | (1 << EXCEPTION_HCS);
	}

	//进行FCS校验-----------------------计算h_cs_cal
	if (outframe->infolen == 0)
		return 0;

	u_short f_cs_val = f_cs_cal(pData, outframe->f_format.frame_sublen - 2);
	if (outframe->f_cs != f_cs_val)
	{
		*exception = *exception | (1 << EXCEPTION_FCS);
	}
	return 0;
}

int RuntimeFrameCheck(HdlcStationParam *stpar, hdlc *pframe)
{
	if (pframe->f_format.frame_type != 0xA0)//帧类型正确
	{
		return ERROR_FORMAT_TYPE;
	}
	else if (pframe->dst_addrlen != 1 && pframe->dst_addrlen != 2 && pframe->dst_addrlen != 4)
	{
		// m_liststateinfo.AddString(_T("目的地址长度不正确！"));
		return ERROR_DST_ADDR_LEN;
	}
	else if (pframe->src_addrlen != 1)// && pframe->src_addrlen!=2 &&pframe->src_addrlen!=4)
	{
		// m_liststateinfo.AddString(_T("源地址长度不正确！"));
		return ERROR_SRC_ADDR_LEN;
	}
	if (pframe->infolen > stpar->max_rcv_info_size)
	{
		//m_liststateinfo.AddString(_T("信息域超长，超出协商的最大信息长度！"));
		return ERROR_INF_OVERFLOW;
	}
	return 0;
}

int convHexStr(HdlcStationParam *stpar, u_char *pData, _TCHAR* S, int len)
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
	for (u_int i = 0; i < (nReceivedLen / 2); i++)
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
#ifdef __FLAGCHECK
	if (byte == 0x7D || byte == 0x7E)
	{
		if (S != NULL)
		{
			u_int rs = _stprintf(S + index, _T("%02x"), 0x7D);
			index += rs;
		}
		if (pRawData != NULL)
		{
			pRawData[(*bytesidx)++] = (u_char)0x7D;
		}
		byte = byte ^ 0x20;
	}
#endif
	if (S != NULL)
	{
		u_int rs = _stprintf(S + index, _T("%02x"), byte);
		index += rs;
	}
	if (pRawData != NULL)
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

	pRawData && (frame->h_cs = cs_cal(pRawData + 1, frame->dst_addrlen + frame->src_addrlen + 3));
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
	S ? S[index] = '\0' : 0;
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


int HdlcSetOpt(HdlcStationParam *stpar, hdlcpointer frame){
	return 0;
}

void HdlcParamInit(HdlcStationParam *stpar){
	//stpar->main_pot_addr = 0x10;
	//stpar->slav_pot_addr = 0x91;
	//stpar->main_pot_addrlen = 1;
	//stpar->slav_pot_addrlen = 2;
	stpar->nr = 0;
	stpar->ns = 0;
	stpar->started = 0;
	stpar->frame_p_f = 0;
	stpar->rcv_num = 0;
	stpar->send_num = 0;
	stpar->frmr_flag = 0;
	stpar->disc = 1;
	stpar->isUIWaiting = 0;
	stpar->canUISend = 0;
	stpar->isTransFinish = 1;
}

int HdlcTransParamInit(HdlcStationParam *stpar)
{
	stpar->max_rcv_info_size = 0x80;
	stpar->rcvwindowsize = 1;

	stpar->max_snd_info_size = 0x80;
	stpar->sendwindowsize = 3;
	return 0;
}

int byteparser(u_char *paramdata, u_int getlen, u_int len, u_int *pres)
{
	static u_int seek = 0;
	u_int num;
	int error = 0;
	u_int res = 0;
	for (int i = 0; i < getlen; i++)
	{
		if (seek + i + 1 > len)
		{
			error = -1;
			break;
		}
		num = paramdata[seek + i];
		num = num << ((getlen - i - 1) * 8);
		res = res + num;
	}
	if (error == -1)
	{
		res = 0;
	}
	else
	{
		seek += getlen;
	}
	*pres = res;
	return error;
}

hdlcparam *paramparser(u_char *paramstr, u_int len)
{
	u_char ch;
	u_char hexstr[2];
	u_int i = 0, j = 0;
	u_int hex;
	hdlcparam *paramlist = NULL;
	hdlcparam *param;
	int res = byteparser(paramstr, 2, len, &hex);
	if (res == -1)
	{
		return NULL;
	}
	if (hex != 0x8180)
	{
		return NULL;
	}
	res = byteparser(paramstr, 1, len, &hex);
	if (res == -1)
	{
		return NULL;
	}
	int paramstrlen = hex;

	u_int arg, value;
	while (1)
	{

		res = byteparser(paramstr, 1, len, &arg);
		if (res == -1)
		{
			break;
		}



		res = byteparser(paramstr, 1, len, &hex);
		if (res == -1)
		{
			break;
		}

		res = byteparser(paramstr, hex, len, &value);
		if (res == -1)
		{
			break;
		}
		param = new hdlcparam();
		param->arg = arg;
		param->value = value;
		if (paramlist == NULL)
		{
			paramlist = param;
			paramlist->next = NULL;
		}
		else
		{
			paramlist->next = param;
			param->next = NULL;
		}
	}
	return paramlist;
}
int HdlcSetParam(HdlcStationParam *stpar, u_char *paramstr, u_int len)
{
	hdlcparam *paramlist = paramparser(paramstr, len);
	u_char *settingdata = stpar->settingdata;
	hdlcparam *pParam;
	u_int argval;
	pParam = paramlist;
	while (pParam)
	{
		switch (pParam->arg)
		{
		case ARG_MAX_SND_SIZE:
			argval = pParam->value;
			stpar->max_snd_info_size = argval <= MAX_SNDINFO_SIZE ? argval : MAX_SNDINFO_SIZE;
			settingdata[ARG_SNDINFO_INDEX] = (argval & 0xFF00) >> 8;
			settingdata[ARG_SNDINFO_INDEX + 1] = (argval & 0xFF);
			break;
		case ARG_SND_WIN_SIZE:
			argval = pParam->value;
			stpar->sendwindowsize = argval <= MAX_SEND_WINDOW_SIZE ? argval : MAX_SEND_WINDOW_SIZE;
			settingdata[ARG_SNDWINSIZE_INDEX] = (argval & 0xFF000000) >> 24;
			settingdata[ARG_SNDWINSIZE_INDEX + 1] = (argval & 0xFF0000) >> 16;
			settingdata[ARG_SNDWINSIZE_INDEX + 2] = (argval & 0xFF00) >> 8;
			settingdata[ARG_SNDWINSIZE_INDEX + 1] = (argval & 0xFF);
			break;
		case ARG_MAX_RCV_SIZE:
			argval = pParam->value;
			stpar->max_rcv_info_size = argval <= MAX_RCVINFO_SIZE ? argval : MAX_RCVINFO_SIZE;;
			settingdata[ARG_RCVINFO_INDEX] = (argval & 0xFF00) >> 8;
			settingdata[ARG_RCVINFO_INDEX + 1] = (argval & 0xFF);
			break;
		case ARG_RCV_WIN_SIZE:
			argval = pParam->value;
			stpar->rcvwindowsize = argval <= MAX_RCV_WINDOW_SIZE ? argval : MAX_RCV_WINDOW_SIZE;;
			settingdata[ARG_RCVWINSIZE_INDEX] = (argval & 0xFF000000) >> 24;
			settingdata[ARG_RCVWINSIZE_INDEX + 1] = (argval & 0xFF0000) >> 16;
			settingdata[ARG_RCVWINSIZE_INDEX + 2] = (argval & 0xFF00) >> 8;
			settingdata[ARG_RCVWINSIZE_INDEX + 1] = (argval & 0xFF);
			break;
		}
		pParam = pParam->next;
	}
	pParam = paramlist;
	while (paramlist)
	{
		paramlist = pParam->next;
		delete pParam;
		pParam = paramlist;
	}
	return 0;
}

int KillFlag(u_char *pData, int len, u_char *pOut)
{
	int i = 0, j = 0;
	u_char cbyte;
	if (i == 0 && pData[i] == 0x7E)
	{
		pOut[i] = pData[i];
		i++;
	}
	else
	{
		return -1;
	}
	pData[len - 1] = ~pData[len - 1];

	while (i < len)
	{
		cbyte = pData[i++];
		if (cbyte == 0x7D || cbyte == 0x7E)
		{
			cbyte = cbyte ^ 0x20;
			pOut[j++] = 0x7D;
			pOut[j++] = cbyte;
		}
	}
	pData[len - 1] = ~pData[len - 1];
	pOut[j - 1] = ~pOut[j - 1];
}

u_char GetByte(u_char *pData, int *i)
{
	if (pData[*i] == 0x7D)
	{
		(*i)++;
		return pData[*i] ^ 0x20;
	}
	return pData[*i];
}

void RestoreByte(u_char *pData, int *i, u_char inbyte)
{
	if (inbyte == 0x7D || inbyte == 0x7E)
	{
		pData[*i] = 0x7D;
		(*i)++;
		pData[*i] = inbyte ^ 0x20;
		return;
	}
	pData[*i] = inbyte;
	return;
}

u_int gethighaddr(u_int addr, u_int addrlen)
{
	u_int preaddr;
	u_int ival;
	switch (addrlen)
	{
	case 1:
		return 0;
		break;
	case 2:
		addr &= 0x7F;
		ival = ival << 7;
		addr |= ival;
		break;
	case 4:
		addr &= 0x3FFF;
		ival = ival << 14;
		addr |= ival;
		break;
	default:
		return 0;
		break;
	}
	return addr;
}

u_int getlowaddr(u_int addr, u_int addrlen)
{
	u_int preaddr;
	u_int ival;
	switch (addrlen)
	{
	case 1:
		addr = ival;
		break;
	case 2:
		addr &= 0x3F80;
		addr |= ival;
		break;
	case 4:
		addr &= 0xFFFC000;
		addr |= ival;
		break;
	default:
		break;
	}
	return addr;
}

BOOL vailfyBoardCastAddr(HdlcStationParam *stpar, hdlc *frame)
{
	int dstlen = frame->dst_addrlen, srclen = frame->src_addrlen;
	int dstaddr = frame->dst_addr, srcaddr = frame->src_addr;
	int slavlen = stpar->slav_pot_addrlen;
	int mainlen = stpar->main_pot_addrlen;

	if (slavlen == 1)
	{
		if (srclen == 1)
		{
			if (srcaddr == 0x7F)
			{
				return TRUE;
			}
		}
		else if (srclen == 2)
		{

		}
		else if (srclen == 4)
		{

		}
	}
	else if (slavlen == 2)
	{
		if (srclen == 1)
		{

		}
		else if (srclen == 2)
		{
			if (gethighaddr(srcaddr, srclen) == 0x7F ||
				getlowaddr(srcaddr, srclen) == 0x7F)
			{
				return TRUE;
			}
		}
		else if (srclen == 4)
		{

		}
	}
	else if (slavlen == 4)
	{
		if (srclen == 1)
		{

		}
		else if (srclen == 2)
		{

		}
		else if (srclen == 4)
		{
			if (gethighaddr(srcaddr, srclen) == 0x3FFF ||
				getlowaddr(srcaddr, srclen) == 0x3FFF)
			{
				return TRUE;
			}
		}
	}
	return FALSE;
}

BOOL vailfyAddr(HdlcStationParam *stpar, hdlc *frame)
{
	int dstlen = frame->dst_addrlen, srclen = frame->src_addrlen;
	int dstaddr = frame->dst_addr, srcaddr = frame->src_addr;
	int slavlen = stpar->slav_pot_addrlen;
	int mainlen = stpar->main_pot_addrlen;

	if (srcaddr == 0x00) //NO_STATION
	{
		return FALSE;
	}

	if (srclen == 1)
	{
		if (srcaddr == 0x7F)//Discard all station in src addr
		{
			return FALSE;
		}
	}
	else if (srclen == 2)
	{
		if (gethighaddr(srcaddr, srclen) == 0x7F || gethighaddr(srcaddr, srclen) == 0x7F)
		{
			return FALSE;
		}
	}
	else if (srclen == 4)
	{
		if (gethighaddr(srcaddr, srclen) == 0x3FFF || gethighaddr(srcaddr, srclen) == 0x3FFF)
		{
			return FALSE;
		}
	}
	return TRUE;
}

void EncodeTransChannel(CString strdata, CString &outdata)//编码，消除数据中的0x7F
{
	CString HexNum;
	CString outStr;
	u_char data[255];
	_TCHAR out[255];
	int byte;
	int cnt = 0;
	int index = 0, rs = 0;
	data[cnt] = 0x7E;
	cnt++;
	for (int i = 0; i < (u_int)strdata.GetLength() / 2; i++)
	{
		HexNum = strdata.Mid(i * 2, 2);
		_stscanf(HexNum, _T("%x"), &byte);
		if (byte == 0x7E || byte == 0x7D)
		{
			data[cnt++] = 0x7D;
			data[cnt++] = byte ^ 0x20;
		}
		else
		{
			data[cnt++] = byte;
		}
	}
	data[cnt++] = 0x7E;

	for (int i = 0; i < cnt; i++)
	{
		rs = _stprintf(out + index, _T("%02x"), data[i]);
		index += rs;
	}
	out[index] = '\0';
	outdata = out;
}

void DecodeTransChannel(CString strdata, CString &outdata)//解码，恢复数据中的0x7F
{
	CString HexNum;
	CString outStr;
	u_char data[255];
	_TCHAR out[255];
	int byte;
	int cnt = 0;
	int index = 0, rs = 0;
	int datalen = 0;
	int flagreach = 0;
	for (int i = 0; i < (u_int)strdata.GetLength() / 2; i++)
	{
		HexNum = strdata.Mid(i * 2, 2);
		_stscanf(HexNum, _T("%x"), &byte);
		if (byte == 0x7e)
		{
			continue;
		}
		else if (byte == 0x7d)
		{
			flagreach = 1;
			continue;
		}
		else if (flagreach == 1)
		{
			flagreach = 0;
			data[cnt++] = byte ^ 0x20;
			datalen++;
		}
		else
		{
			data[cnt++] = byte;
			datalen++;
		}
	}

	for (int i = 0; i < cnt; i++)
	{
		rs = _stprintf(out + index, _T("%02x"), data[i]);
		index += rs;
	}
	out[index] = '\0';
	outdata = out;
}