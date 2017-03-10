// ClientSocket.cpp : 实现文件
//

#include "stdafx.h"
#include "Client.h"
#include "ClientSocket.h"
#include "ClientDlg.h"


// CClientSocket

CClientSocket::CClientSocket()
{
	m_pDlg=NULL;    //指向CClientDlg类的指针 m_pDlg的初始化
}

CClientSocket::~CClientSocket()
{
	m_pDlg=NULL;
}


// CClientSocket 成员函数


void CClientSocket::OnClose(int nErrorCode)  //m_sClientSocket.Close()函数执行后会触发OnClose()函数
{
	// TODO: 在此添加专用代码和/或调用基类

	CAsyncSocket::OnClose(nErrorCode);//先执行CAsyncSocket类下的OnClose()函数，判断是否断开连接
	if(nErrorCode==0)           //若成功断开连接
		m_pDlg->onClose();    //转去继续执行onClose()函数
}


void CClientSocket::OnConnect(int nErrorCode)  //m_sClientSocket.Connect()执行完后会触发OnConnect()函数
{
	// TODO: 在此添加专用代码和/或调用基类

	CAsyncSocket::OnConnect(nErrorCode);  //判断是否连接成功
	if(nErrorCode==0)    //若连接成功
		m_pDlg->onConnect();   //转去执行onConnect()函数
}


void CClientSocket::OnReceive(int nErrorCode)  //Receive()函数调用后会触发OnReceive()函数
{
	// TODO: 在此添加专用代码和/或调用基类

	CAsyncSocket::OnReceive(nErrorCode);
	if(nErrorCode==0)   //若已接收到数据
		m_pDlg->onReceive();  //转去执行onReceive()函数
}


void CClientSocket::setParentDlg(CClientDlg* pDlg)  //用于关联两个类
{
	m_pDlg=pDlg;   
}
