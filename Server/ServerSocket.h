#pragma once

// CServerSocket 命令目标
class CServerDlg;
class CServerSocket : public CAsyncSocket
{
public:
	CServerSocket();
	virtual ~CServerSocket();
	virtual void OnClose(int nErrorCode);  
	virtual void OnAccept(int nErrorCode);
	virtual void OnReceive(int nErrorCode);
private:
	CServerDlg* m_pDlg;   
public:
	void setParentDlg(CServerDlg* pDlg);  //用于关联两个类
};


