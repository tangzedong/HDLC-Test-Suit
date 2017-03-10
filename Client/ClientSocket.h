#pragma once

// CClientSocket ����Ŀ��
class CClientDlg;      //����CClientDlg��
class CClientSocket : public CAsyncSocket
{
public:
	CClientSocket();
	virtual ~CClientSocket();
	virtual void OnClose(int nErrorCode);  //��ӦOnClose�¼�
	virtual void OnConnect(int nErrorCode);  //��ӦOnConnect�¼�
	virtual void OnReceive(int nErrorCode);   //��ӦOnReceive�¼�
private:
	CClientDlg* m_pDlg;       //ָ��CClientDlg���ָ�룬���ڹ���������
public:
	void setParentDlg(CClientDlg* pDlg);  //���ڹ���������
};


