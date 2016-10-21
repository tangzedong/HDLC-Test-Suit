
// ServerDlg.h : ͷ�ļ�
//

#pragma once
#include "ServerSocket.h"
#include "afxwin.h"

// CServerDlg �Ի���
class CServerDlg : public CDialogEx
{
// ����
public:
	CServerDlg(CWnd* pParent = NULL);	// ��׼���캯��

// �Ի�������
	enum { IDD = IDD_SERVER_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV ֧��


// ʵ��
protected:
	HICON m_hIcon;

	// ���ɵ���Ϣӳ�亯��
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	CString m_strServerName;
	int m_nServerPort;
	CString m_strToServer;
	CListBox m_listReceived;
	CListBox m_listSent;
//    CListBox m_liststateinfo;
	afx_msg void OnClickedButtonListen();
	afx_msg void OnClickedButtonCloselisten();
	afx_msg void OnClickedButtonSend();
	

	void onClose(void);  //��Ӧ����OnClose()�¼��ĺ���
	void onAccept(void);
	void onReceive(void);
	CServerSocket m_sServerSocket;  //�����׽���
	CServerSocket m_sClientSocket;  //�����׽���
	CListBox m_liststateinfo;

//	void setHScroll();
};
