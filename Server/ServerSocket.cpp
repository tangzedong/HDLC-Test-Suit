// ServerSocket.cpp : 实现文件
//

#include "stdafx.h"
#include "Server.h"
#include "ServerDlg.h"
#include "ServerSocket.h"


// CServerSocket

CServerSocket::CServerSocket()
{
	m_pDlg=NULL;   //指向CServerDlg类的指针成员变量的初始化
}

CServerSocket::~CServerSocket()
{
	m_pDlg=NULL;
}


// CServerSocket 成员函数

//对CAsyncSocket类的OnClose()、OnAccept()、OnReceive()函数重载
void CServerSocket::OnClose(int nErrorCode) 
{
	// TODO: 在此添加专用代码和/或调用基类

	CAsyncSocket::OnClose(nErrorCode);
	if(nErrorCode==0)  //如果成功断开连接
		m_pDlg->onClose();   //转去执行onClose()函数
}

void CServerSocket::OnAccept(int nErrorCode)
{
	// TODO: 在此添加专用代码和/或调用基类

	CAsyncSocket::OnAccept(nErrorCode);
	if(nErrorCode==0)
		m_pDlg->onAccept();
}

void CServerSocket::OnReceive(int nErrorCode)  //客户端调用一次send函数会触发OnReceive
{
	// TODO: 在此添加专用代码和/或调用基类

	CAsyncSocket::OnReceive(nErrorCode);
	if(nErrorCode==0)   //若缓冲区里确实有收到数据
		m_pDlg->onReceive();  //转去执行onReceive()，真正完成接受数据的功能
}


void CServerSocket::setParentDlg(CAboutDlg* pDlg)
{
	m_pDlg=pDlg;  //关联两个类
}
