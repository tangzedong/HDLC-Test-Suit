
// ServerDlg.h : ͷ�ļ�
//

#pragma once
#include "ServerSocket.h"
#include "afxwin.h"
#include "hdlc.h"
#include "hdlcFSM.h"
#include "hdlceventhandler.h"
#include "ConsoleDlg.h"
#include "PropertyDlg.h"
#define WM_INFO WM_USER+8
#define WM_REPORTEVENT WM_USER+9
#define WM_WRITETOFILE WM_USER+10
#define WM_SENDLOG WM_USER+11

#define WM_APPLMSG WM_USER+3
#define WM_APPLGETDATA WM_USER+4

class CConsoleDlg;

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

	// �Ի�������
	enum { IDD = IDD_ABOUTBOX };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	// ʵ��
protected:
	DECLARE_MESSAGE_MAP()
	afx_msg LRESULT OnInfo(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnApplmsg(WPARAM wParam, LPARAM lParam);
	//	afx_msg LRESULT OnWriteToFiel(WPARAM wParam, LPARAM lParam);
	//	afx_msg LRESULT OnSendLog(WPARAM wParam, LPARAM lParam);
public:
	afx_msg void OnBnClickedButtonNewser();

public:
	CServerSocket m_sServerSocket;  //�����׽���
	CServerSocket m_sClientSocket;  //�����׽���
	CServerSocket m_sLoopSocket;  //�����׽���

	CServerDlg *serverlist[25];
	u_int m = 0;
	afx_msg void OnClose();
	// //���ܴ�����
	void onReceive();
	virtual BOOL OnInitDialog();
	CString m_strServerName;
	int m_nServerPort;
	void onAccept();
	void onClose();
	u_char str[2048];
	afx_msg void OnClickedButtonCn();
	int DoSend(CString nSentLen);
	u_char m_bListening = 0;
	afx_msg void OnViewConsole();
	CConsoleDlg m_condlg;
	afx_msg void OnDropdownButtonNewser(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnNewServer();
	afx_msg void OnNewExit();
};

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
//	int m_nServerPort;
	CString m_strToServer;

	u_char *m_settingdata;
//    CListBox m_liststateinfo;
	afx_msg void OnClickedButtonListen();
	afx_msg void OnClickedButtonCloselisten();
	afx_msg void OnClickedButtonSend();
	afx_msg LRESULT CServerDlg::OnPropertyChanged(WPARAM wParam, LPARAM lParam);

	void onClose(void);  //��Ӧ����OnClose()�¼��ĺ���
	void onAccept(void);
	void onReceive(hdlc*);



//	void setHScroll();
protected:
	afx_msg LRESULT OnInfo(WPARAM wParam, LPARAM lParam);
//	afx_msg LRESULT OnTest(WPARAM wParam, LPARAM lParam);
public:
	afx_msg void OnTimer(UINT_PTR nIDEvent);
protected:
	afx_msg LRESULT OnReportEvent(WPARAM wParam, LPARAM lParam);
public:
	CString m_recdata;
protected:
	afx_msg LRESULT OnApplmsg(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnApplgetdata(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnWriteToFile(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnSendLog(WPARAM wParam, LPARAM lParam);
public:
	afx_msg void OnClose();

//hdlc related
	//hdlcpointer hdlc_p =  NULL;
public:
	hdlcpointer hdlc_p;
	u_char save_inf[MAX_LEN];
	//��ʼʱ��1״̬ΪNDM��2ΪWAIT_CONNECT , 3ΪNRM , 4ΪWAIT_DISCONNECT
	u_char str[MAX_LEN];
	u_char* str_pointer;
	FILE *wc;
	u_int tmp;
	u_char* str_p;
	u_int m; //��¼��Ϣ��֡�洢�ı���
	u_char write_str[MAX_LEN];
	int nSentLen;
	HdlcTcb _tcb;
	HdlcTcb *m_fsmstack = &_tcb;
	
	HdlcStationParam _stpar;
	HdlcStationParam *stpar = &_stpar;

	DWORD curtime;
	afx_msg void OnBnClickedButton1();
	CAboutDlg* pParent;
	CPropertyDlg m_propertydlg;
	void SetAddr(int addr, int addrlen=1);
	void OnApplicationLook(UINT id = 0);
	CMFCPropertyGridCtrl m_propertysheet;
	CMFCTabCtrl m_tabout;

	CListBox m_listReceived;
	CListBox m_listSent;
	CListBox m_liststateinfo;
	// ��վid
	int m_id;
};
