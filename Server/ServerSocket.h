#pragma once
#include <afxsock.h>

// CServerSocket 命令目标
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
	void setParentDlg(CAboutDlg* pDlg);  //用于关联两个类
};


