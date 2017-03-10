#pragma once
#include "ISH.h"
// CClientSocket ����Ŀ��
class CClientSocket : public CAsyncSocket
{
public:
	CClientSocket();
	virtual ~CClientSocket();
	virtual void OnClose(int nErrorCode);  //��ӦOnClose�¼�
	virtual void OnConnect(int nErrorCode);  //��ӦOnConnect�¼�
	virtual void OnReceive(int nErrorCode);   //��ӦOnReceive�¼�
	virtual void OnAccept(int nErrorCode);    //��ӦOnAccept�¼�
private:
	ISocketHandler* m_pDlg;       //ָ��CClientDlg���ָ�룬���ڹ���������
public:
	void setParentDlg(ISocketHandler* pDlg);  //���ڹ���������
};


