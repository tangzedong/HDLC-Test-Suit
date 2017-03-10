#pragma once
#include <afxsock.h>

// CServerSocket 命令目标
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
	void setParentDlg(CMainDlg* pDlg);  //用于关联两个类
};


