#pragma once

// CClientSocket 命令目标
class CClientDlg;      //声明CClientDlg类
class CClientSocket : public CAsyncSocket
{
public:
	CClientSocket();
	virtual ~CClientSocket();
	virtual void OnClose(int nErrorCode);  //响应OnClose事件
	virtual void OnConnect(int nErrorCode);  //响应OnConnect事件
	virtual void OnReceive(int nErrorCode);   //响应OnReceive事件
private:
	CClientDlg* m_pDlg;       //指向CClientDlg类的指针，用于关联两个类
public:
	void setParentDlg(CClientDlg* pDlg);  //用于关联两个类
};


