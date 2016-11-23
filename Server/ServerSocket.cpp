// ServerSocket.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "Server.h"
#include "ServerDlg.h"
#include "ServerSocket.h"


// CServerSocket

CServerSocket::CServerSocket()
{
	m_pDlg=NULL;   //ָ��CServerDlg���ָ���Ա�����ĳ�ʼ��
}

CServerSocket::~CServerSocket()
{
	m_pDlg=NULL;
}


// CServerSocket ��Ա����

//��CAsyncSocket���OnClose()��OnAccept()��OnReceive()��������
void CServerSocket::OnClose(int nErrorCode) 
{
	// TODO: �ڴ����ר�ô����/����û���

	CAsyncSocket::OnClose(nErrorCode);
	if(nErrorCode==0)  //����ɹ��Ͽ�����
		m_pDlg->onClose();   //תȥִ��onClose()����
}

void CServerSocket::OnAccept(int nErrorCode)
{
	// TODO: �ڴ����ר�ô����/����û���

	CAsyncSocket::OnAccept(nErrorCode);
	if(nErrorCode==0)
		m_pDlg->onAccept();
}

void CServerSocket::OnReceive(int nErrorCode)  //�ͻ��˵���һ��send�����ᴥ��OnReceive
{
	// TODO: �ڴ����ר�ô����/����û���

	CAsyncSocket::OnReceive(nErrorCode);
	if(nErrorCode==0)   //����������ȷʵ���յ�����
		m_pDlg->onReceive();  //תȥִ��onReceive()��������ɽ������ݵĹ���
}


void CServerSocket::setParentDlg(CAboutDlg* pDlg)
{
	m_pDlg=pDlg;  //����������
}
