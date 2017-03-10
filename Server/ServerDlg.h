
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
#include "IPhysic.h"
#include "afxcmn.h"
#define WM_INFO WM_USER+8
#define WM_REPORTEVENT WM_USER+9
#define WM_WRITETOFILE WM_USER+10
#define WM_SENDLOG WM_USER+11

#define WM_APPLMSG WM_USER+3
#define WM_APPLGETDATA WM_USER+4

#define IDM_DATALINKREADY WM_USER+5
#define IDM_RESENDWND WM_USER+6
#define IDM_SENDDATA WM_USER+12

#define WM_APPLAYERREADY WM_USER+7

#define MAX_SEQ 15

typedef void(*QueryModuleFun)(__out LPTSTR szDescription, __in   int nMaxCount);

class CConsoleDlg;

class CMainDlg : public CDialogEx, public ISend
{
public:
	CMainDlg();

	// �Ի�������
	enum { IDD = IDD_ABOUTBOX };

protected:
	PROCESS_INFORMATION m_procinfo;
	HMODULE m_hMod = NULL;

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



	CString m_strServerName;
	int m_nServerPort;

	u_char str[2048];


	u_char m_bListening = 0;
	
	CConsoleDlg m_condlg;

	int m_bFrameIncomplete;
	int m_nPos;

	// ����֡��ʱ
	UINT m_delaySendData;
	// �ŵ����г�ʱ������ʱʧЧ��
	UINT m_nIdleTimeout;
	// �ط���ʱ
	int m_nResendTimeOut;

	UINT m_nInterTimeout;
	CSpinButtonCtrl m_SpinPort;

public:
	// //���ܴ�����
	int DoSend(CString szData);
	void onReceive();
	virtual BOOL OnInitDialog();
	void onAccept();
	void onClose();

	afx_msg void OnClose();
	afx_msg void OnClickedButtonCn();
	afx_msg void OnViewConsole();
	afx_msg void OnDropdownButtonNewser(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnNewServer();
	afx_msg void OnNewExit();
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg void OnChangeEdit1();
	afx_msg void OnChangeEdit4();
	afx_msg void OnChangeEdit5();
	afx_msg void OnChangeEdit6();
	afx_msg void OnDeltaposSpin1(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnNewClient();
	afx_msg void OnPluginLoadModule();
	afx_msg void OnPluginViewModuleInf();
	afx_msg void OnMacroEditMacro();
};

// CServerDlg �Ի���
class CServerDlg : public CDialogEx, public IGenFrame
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
	void DoClear();


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
	afx_msg void OnBnClickedButton1();
//hdlc related
	//hdlcpointer hdlc_p =  NULL;
public:
	hdlcpointer hdlc_p;
	u_char save_inf[MAX_LEN];
	//��ʼʱ��1״̬ΪNDM��2ΪWAIT_CONNECT , 3ΪNRM , 4ΪWAIT_DISCONNECT
	u_char str[MAX_LEN];
	u_char* str_pointer;
	FILE *wc=NULL;
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
	CMainDlg* pParent;
	CPropertyDlg m_propertydlg;
	CMFCPropertyGridCtrl m_propertysheet;
	CMFCTabCtrl m_tabout;
	CListBox m_listReceived;
	CListBox m_listLog;
	CListBox m_liststateinfo;
	// ��վid
	int m_id;

	hdlc m_UIFrame;
	char m_UIInfoBuf[255];
	u_int m_UIInfoLen;
	hdlc *pOutFrame; //���ݴ���֡����

	//�ر�ָ���Ƿ��´�
	BOOL close = FALSE;
public:
	void SetAddr(int addr, int addrlen = 1);
	void OnApplicationLook(UINT id = 0);
	//void SendFrame(u_int frameKind, u_int seq);
	int SendFrame(const char *frameKind, hdlc *outframe);
	void FromAppLayer(u_char *p, u_int *infolen, u_int nextSendFrame);
	//void ToPhysicLayer(hdlc *frame);
	int ToPhysicLayer(CString strToServer);
	void FromPhysicLayer(hdlc *frame);
	void ReSendFrame(int framePos);
	void Inc(u_int& seq) { (seq < m_totalFrameSend) ? seq++ : seq = 0; }
	BOOL Between(u_int a, u_int b, u_int c)
	{
		//return true if a<=b<c, false otherwise
		return ((a <= b && b < c) || (c < a && a <= b) || (b < c && c < a));
	}
	afx_msg void OnBnClickedButtonDatasnd();
protected:
	afx_msg LRESULT OnAppLayerReady(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnDatalinkReady(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnResendWnd(WPARAM wParam, LPARAM lParam);
public:
	void GenFrameBuf();
private:
	u_int m_totalFrameSend; //�ܹ���Ҫ�����֡��Ŀ
	u_int m_nextSendFrame;  //��һ��Ҫ�����֡������
	//u_int m_FrameInd;	//��ǰ֡�Ĵ��������� ��stpar->m ����
protected:
	afx_msg LRESULT OnSendData(WPARAM wParam, LPARAM lParam);
};
