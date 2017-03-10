#pragma once
#include <afxsock.h>

// CServerSocket ����Ŀ��
class CMainDlg;
class CServerSocket : public CAsyncSocket
{
public:
	CServerSocket();
	virtual ~CServerSocket();
	virtual void OnClose(int nErrorCode);  
	virtual void OnAccept(int nErrorCode);
	virtual void OnReceive(int nErrorCode);
private:
	CMainDlg* m_pDlg;   
public:
	void setParentDlg(CMainDlg* pDlg);  //���ڹ���������
};


