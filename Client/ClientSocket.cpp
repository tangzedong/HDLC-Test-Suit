// ClientSocket.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "Client.h"
#include "ClientSocket.h"
#include "ClientDlg.h"


// CClientSocket

CClientSocket::CClientSocket()
{
	m_pDlg=NULL;    //ָ��CClientDlg���ָ�� m_pDlg�ĳ�ʼ��
}

CClientSocket::~CClientSocket()
{
	m_pDlg=NULL;
}


// CClientSocket ��Ա����


void CClientSocket::OnClose(int nErrorCode)  //m_sClientSocket.Close()����ִ�к�ᴥ��OnClose()����
{
	// TODO: �ڴ����ר�ô����/����û���

	CAsyncSocket::OnClose(nErrorCode);//��ִ��CAsyncSocket���µ�OnClose()�������ж��Ƿ�Ͽ�����
	if(nErrorCode==0)           //���ɹ��Ͽ�����
		m_pDlg->onClose();    //תȥ����ִ��onClose()����
}


void CClientSocket::OnConnect(int nErrorCode)  //m_sClientSocket.Connect()ִ�����ᴥ��OnConnect()����
{
	// TODO: �ڴ����ר�ô����/����û���

	CAsyncSocket::OnConnect(nErrorCode);  //�ж��Ƿ����ӳɹ�
	if(nErrorCode==0)    //�����ӳɹ�
		m_pDlg->onConnect();   //תȥִ��onConnect()����
}


void CClientSocket::OnReceive(int nErrorCode)  //Receive()�������ú�ᴥ��OnReceive()����
{
	// TODO: �ڴ����ר�ô����/����û���

	CAsyncSocket::OnReceive(nErrorCode);
	if(nErrorCode==0)   //���ѽ��յ�����
		m_pDlg->onReceive();  //תȥִ��onReceive()����
}


void CClientSocket::setParentDlg(CClientDlg* pDlg)  //���ڹ���������
{
	m_pDlg=pDlg;   
}
