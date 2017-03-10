
// ClientDlg.h : ͷ�ļ�
//

#pragma once
#include "ClientSocket.h"
#include "afxwin.h"
#include "HdlcFrameGenDlg.h"
#define MAX_LEN 2048

#define WM_AUTOSEND WM_USER+108
// CClientDlg �Ի���
class CClientDlg : public CDialogEx
{
// ����
public:
	CClientSocket m_sClientSocket;    //����CClientSocket��Ķ���
	CClientDlg(CWnd* pParent = NULL);	// ��׼���캯��

// �Ի�������
	enum { IDD = IDD_CLIENT_DIALOG };

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
	POINT old;
	CString m_strServerName;
	int m_nServerPort;
	//CString m_strToServer;
	CListBox m_listReceived;
	CListBox m_listSent;
	afx_msg void OnClickedButtonConnect();   //��Ӧ����
	afx_msg void OnClickedButtonDisconnect();
	afx_msg void OnClickedButtonSend();
	void onConnect(void);   //��ִ�е�CClientSocket��� OnConnect��������ʱ��ת���˴���ִ��
	void onReceive(void);
	void onClose(void);        
	//int readoneline (FILE* q,u_char oneLineData[MAX_LEN]);
	afx_msg void OnEnChangeEditFile();
	CString m_editfile;     //�ļ������ļ�·���༭��ĳ�Ա����
    CListBox m_listcontent;  //�ļ����ݣ��ļ����ݱ༭��ĳ�Ա����
	
	afx_msg void OnBnClickedButtonView();
	afx_msg void OnSelchangeListReceived();
	afx_msg void OnSelchangeListSent();
	//afx_msg void OnChangeEditContent();
	//afx_msg void OnButton3();
	//afx_msg void OnSelchangeListConnect();
	afx_msg void OnSelchangeListContent();
	CString m_listToServer;
//	CListBox m_listboxrec;
	CString m_strSendData;
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnBnClickedButtonmake();
	CHdlcFrameGenDlg m_makedlg;
protected:
	afx_msg LRESULT OnAutoSend(WPARAM wParam, LPARAM lParam);
};
