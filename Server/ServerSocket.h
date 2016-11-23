#pragma once
#include <afxsock.h>

// CServerSocket ����Ŀ��
class CAboutDlg;
class CServerSocket : public CAsyncSocket
{
public:
	CServerSocket();
	virtual ~CServerSocket();
	virtual void OnClose(int nErrorCode);  
	virtual void OnAccept(int nErrorCode);
	virtual void OnReceive(int nErrorCode);
private:
	CAboutDlg* m_pDlg;   
public:
	void setParentDlg(CAboutDlg* pDlg);  //���ڹ���������
};


