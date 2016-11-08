
// ServerDlg.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "Server.h"
#include "ServerDlg.h"
#include "afxdialogex.h"
#include <stdlib.h>
#include "framedef.h"
#include "hdlcFSM.h"
#include "eventhandler.h"
#ifdef _DEBUG
#define new DEBUG_NEW
#endif


/*-------------------------��������----------------------*/
//hdlcpointer hdlc_p =  NULL;
hdlcpointer hdlc_p;
u_char save_inf[MAX_LEN];
//tcb->rcv_num���Լ���¼�Ľ������
//tcb->send_num���Լ���¼�ķ������
u_char nr = 0;                //���յ��Ľ������
u_char ns = 0;                //���յ��ķ������
//��ʼʱ��1״̬ΪNDM��2ΪWAIT_CONNECT , 3ΪNRM , 4ΪWAIT_DISCONNECT
u_char str[MAX_LEN];
u_char* str_pointer;
FILE *wc;
u_int tmp;
u_char* str_p;
static u_int m = 0; //��¼��Ϣ��֡�洢�ı���
u_short h_cs_val;  //���������HCS����
u_short f_cs_val;   //���������FCS����
u_char write_str[MAX_LEN];
int nSentLen;
HdlcTcb _tcb;
HdlcTcb *tcb = &_tcb;
HdlcStationParam _stpar;

DWORD curtime;

// ����Ӧ�ó��򡰹��ڡ��˵���� CAboutDlg �Ի���

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
};

CAboutDlg::CAboutDlg() : CDialogEx(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
	ON_MESSAGE(WM_INFO, &CAboutDlg::OnInfo)
	ON_MESSAGE(WM_APPLMSG, &CAboutDlg::OnApplmsg)
END_MESSAGE_MAP()


// CServerDlg �Ի���




CServerDlg::CServerDlg(CWnd* pParent /*=NULL*/) //
	: CDialogEx(CServerDlg::IDD, pParent)
	, m_recdata(_T(""))
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	m_strServerName = _T("");
	m_nServerPort = 0;
	m_strToServer = _T("");
}

void CServerDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT_SERVERNAME, m_strServerName);
	DDX_Text(pDX, IDC_EDIT_SERVERPORT, m_nServerPort);
	DDV_MinMaxInt(pDX, m_nServerPort, 1024, 49151);
	DDX_Text(pDX, IDC_EDIT_TOSERVER, m_strToServer);
	DDX_Control(pDX, IDC_LIST_RECEIVED, m_listReceived);
	DDX_Control(pDX, IDC_LIST_SENT, m_listSent);
	//  DDX_Control(pDX, IDC_LIST1, IDC_LIST_STATEINFO);
	//  DDX_Control(pDX, IDC_LIST_STATEINFO, m_liststateinfo);
	DDX_Control(pDX, IDC_LIST_STATEINFO, m_liststateinfo);
	DDX_Text(pDX, IDC_EDIT1, m_recdata);
}

BEGIN_MESSAGE_MAP(CServerDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()

	ON_BN_CLICKED(IDC_BUTTON_LISTEN, &CServerDlg::OnClickedButtonListen)
	ON_BN_CLICKED(IDC_BUTTON_CLOSELISTEN, &CServerDlg::OnClickedButtonCloselisten)
	ON_BN_CLICKED(IDC_BUTTON_SEND, &CServerDlg::OnClickedButtonSend)
	ON_MESSAGE(WM_INFO, &CServerDlg::OnInfo)
//	ON_REGISTERED_MESSAGE(WM_TEST, &CServerDlg::OnTest)
ON_WM_TIMER()
ON_MESSAGE(WM_REPORTEVENT, &CServerDlg::OnReportEvent)
ON_MESSAGE(WM_APPLMSG, &CServerDlg::OnApplmsg)
ON_MESSAGE(WM_APPLGETDATA, &CServerDlg::OnApplgetdata)
END_MESSAGE_MAP()


// CServerDlg ��Ϣ�������

BOOL CServerDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// ��������...���˵�����ӵ�ϵͳ�˵��С�

	// IDM_ABOUTBOX ������ϵͳ���Χ�ڡ�
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// ���ô˶Ի����ͼ�ꡣ��Ӧ�ó��������ڲ��ǶԻ���ʱ����ܽ��Զ�
	//  ִ�д˲���
	SetIcon(m_hIcon, TRUE);			// ���ô�ͼ��
	SetIcon(m_hIcon, FALSE);		// ����Сͼ��

	// TODO: �ڴ���Ӷ���ĳ�ʼ������
	hdlc_p = (hdlcpointer)malloc(sizeof(*hdlc_p));//�ṹ��ָ��ĳ�ʼ��
	memset(hdlc_p, 0, sizeof(*hdlc_p));//��̬����ռ䣬ȫ������
	//FILE *wc ;
	//static u_int m = 0; //��¼��Ϣ��֡�洢�ı���,��̬����������ڶ��α�����ʼֵ
	wc = fopen("write.txt", "w");
	if (wc == NULL)
	{
		printf("failed to open file\n");
		return -1;
	}
	//	nr = 0; ns = 0;
	//	rcv_num = 0; send_num = 0;
	//	state_val = STATE_NDM;//��ʼ״̬ΪNDM
	m_strServerName = "localhost";
	m_nServerPort = 1024;                   //�������Ķ˿ں�����ǰ���úõ�
	UpdateData(FALSE);                   //��������ֵ(��localhost��1024)�����ؼ�
	m_sServerSocket.setParentDlg(this);
	m_sClientSocket.setParentDlg(this);   //���������� ��ʱthis=m_pDlg

	SendDlgItemMessage(IDC_LIST_SENT, LB_SETHORIZONTALEXTENT, (WPARAM)1000, 0);
	SendDlgItemMessage(IDC_LIST_RECEIVED, LB_SETHORIZONTALEXTENT, (WPARAM)1000, 0);
	SendDlgItemMessage(IDC_LIST_STATEINFO, LB_SETHORIZONTALEXTENT, (WPARAM)1000, 0);


	FSMinit();

	gstpar = &_stpar;
	HdlcParamInit();
	
	return TRUE;  // ���ǽ��������õ��ؼ������򷵻� TRUE
}

void CServerDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialogEx::OnSysCommand(nID, lParam);
	}
}

// �����Ի��������С����ť������Ҫ����Ĵ���
//  �����Ƹ�ͼ�ꡣ����ʹ���ĵ�/��ͼģ�͵� MFC Ӧ�ó���
//  �⽫�ɿ���Զ���ɡ�

void CServerDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // ���ڻ��Ƶ��豸������

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// ʹͼ���ڹ����������о���
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// ����ͼ��
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

//���û��϶���С������ʱϵͳ���ô˺���ȡ�ù��
//��ʾ��
HCURSOR CServerDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}


void CServerDlg::OnClickedButtonListen()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	int opt = 1;
	UpdateData(TRUE);   //ˢ�¿ؼ���ֵ����Ӧ�ı���(�ⲿ����ֵ�����ڲ�����)
	GetDlgItem(IDC_BUTTON_LISTEN)->EnableWindow(FALSE);
	GetDlgItem(IDC_EDIT_SERVERNAME)->EnableWindow(FALSE);
	GetDlgItem(IDC_EDIT_SERVERPORT)->EnableWindow(FALSE);
	m_sServerSocket.Create(m_nServerPort);//�����׽��֣������������������Լ��˿ںŰ�
	m_sServerSocket.SetSockOpt(SO_REUSEADDR, &opt, sizeof(opt));
	m_sServerSocket.Listen(); //��ʼ����

	//��������б����Ϣ
	while (m_listSent.GetCount() != 0)
		m_listSent.DeleteString(0);
	while (m_listReceived.GetCount() != 0)
		m_listReceived.DeleteString(0);
	//���°�ť���ı����״̬
	GetDlgItem(IDC_EDIT_TOSERVER)->EnableWindow(TRUE);
	GetDlgItem(IDC_BUTTON_CLOSELISTEN)->EnableWindow(TRUE);
	GetDlgItem(IDC_BUTTON_SEND)->EnableWindow(TRUE);
}


void CServerDlg::OnClickedButtonCloselisten()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	onClose();
}


void CServerDlg::OnClickedButtonSend()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	int nMsgLen;
	//int nSentLen;
	UpdateData(TRUE);
	if (!m_strToServer.IsEmpty())
	{
		nMsgLen = m_strToServer.GetLength()*sizeof(m_strToServer);
		nSentLen = m_sClientSocket.Send(m_strToServer, nMsgLen);
		if (nSentLen != SOCKET_ERROR){
			//���ͳɹ�
			m_listSent.AddString(m_strToServer);
			//	setHScroll();
			UpdateData(FALSE);
		}


		else {
			AfxMessageBox(LPCTSTR("��������ͻ��˷�����Ϣ���ִ���"), MB_OK | MB_ICONSTOP);
		}
		m_strToServer.Empty();
		UpdateData(FALSE);
	}
}


void CServerDlg::onClose(void)
{
	m_listReceived.AddString(CString("�������յ���onClose��Ϣ"));
	m_sClientSocket.Close();
	m_sServerSocket.Close();   //Close()����ִ�к�ᴥ��OnClose()����
	GetDlgItem(IDC_EDIT_TOSERVER)->EnableWindow(FALSE);
	GetDlgItem(IDC_BUTTON_CLOSELISTEN)->EnableWindow(FALSE);
	GetDlgItem(IDC_BUTTON_SEND)->EnableWindow(FALSE);
	GetDlgItem(IDC_EDIT_SERVERNAME)->EnableWindow(TRUE);
	GetDlgItem(IDC_EDIT_SERVERPORT)->EnableWindow(TRUE);
	GetDlgItem(IDC_BUTTON_LISTEN)->EnableWindow(TRUE);
}


void CServerDlg::onAccept(void)  //���OnAccept����ִ�гɹ���ת��ִ��onAccept()
{
	m_listReceived.AddString(CString("�������յ���OnAccept��Ϣ"));
	m_sServerSocket.Accept(m_sClientSocket);  //�������׽�����ͻ��˽�������
	GetDlgItem(IDC_EDIT_TOSERVER)->EnableWindow(TRUE);
	GetDlgItem(IDC_BUTTON_SEND)->EnableWindow(TRUE);
	GetDlgItem(IDC_BUTTON_CLOSELISTEN)->EnableWindow(TRUE);
}


void CServerDlg::onReceive(void)//���OnReceive()����ִ�гɹ���ת��ִ��onReceive()
{

	u_char buff[2048];
	int nBufferSize = 2048;
	int nReceivedLen;
	CString strReceived;

	!gstpar->disc && KillTimer(1);//�رճ�ʱ������
	gstpar->send_flag = 0;	//�Ƿ�����Ӧ֡

	nReceivedLen = m_sClientSocket.Receive(buff, nBufferSize);  //Receive()��������ִ�н��չ���
	//���ؽ��յ����ֽ���     (������ӱ��ر��ˣ�����0�����򷵻�SOCKET_ERROR)
	if (nReceivedLen == SOCKET_ERROR)  //���ճɹ�
	{
		AfxMessageBox(_T("����ʧ�ܣ�"), MB_OK | MB_ICONSTOP);
		return;
	}
	buff[nReceivedLen] = _T('\0');
	CString szTemp(buff);
	szTemp.Replace(_T(" "), _T(""));
	strReceived = szTemp;
	m_listReceived.AddString(strReceived);   //�ڽ����б����ӡ�����յ�����
	UpdateData(FALSE);
	//���յ������ݴ洢��buff�У�Ҫ����str��������
	memset(str, 0, MAX_LEN);//ÿ�����һ������

	convStrHex(szTemp.GetBuffer(), str);

	//-----------------------------------------------------------���ṹ�帳ֵ----------------------------------------------------------------------

	FSMenter(FSMTypePrehandleFrame);
	u_int res = convHexFrame(str, hdlc_p);
	FSMreturn();
	
	//-----------------------------------------------------------������--------------------------------------------------------------------------
	switch (res)
	{
	case ERROR_STARTFLAG:
		AfxMessageBox(_T("��ʼ��־λ����"), MB_OK | MB_ICONSTOP);
		return;
	case ERROR_DST_ADDR_LEN:
		AfxMessageBox(_T("Ŀ�ĵ�ַ���Ȳ���ȷ��"), MB_OK | MB_ICONSTOP);
		return;
	case ERROR_SRC_ADDR_LEN:
		AfxMessageBox(_T("Դ��ַ���Ȳ���ȷ��"), MB_OK | MB_ICONSTOP);
		return;
	case ERROR_INF_OVERFLOW:
		AfxMessageBox(_T("Դ��ַ���Ȳ���ȷ��"), MB_OK | MB_ICONSTOP);
		return;
	}
	if (res == ERROR_INF_OVERFLOW)//��Ϣ�򳬳�
	{
		u_char data[255] = {0x10, 0x00, 0x20};
		m_strToServer = " 7E A0 0D 21 02 23 97 04 C4 10 00 20 E0 6B 7E";
		nSentLen = m_sClientSocket.Send(m_strToServer, 100);
		if (nSentLen != SOCKET_ERROR)
		{
			//���ͳɹ�
			m_listSent.AddString(m_strToServer);
			UpdateData(FALSE);
		}
		//  m_liststateinfo.AddString(_T("��Ϣ�򳬳�������Э�̵������Ϣ���ȣ�"));
		AfxMessageBox(_T("��Ϣ�򳬳�������Э�̵������Ϣ���ȣ�����"), MB_OK | MB_ICONSTOP);
		return;
	}

	//printf("\n��ʼ����hdlc֡��\n");
	m_liststateinfo.AddString(_T("��ʼ����HDLC֡��"));
	//start inter_frame_timer
	if (hdlc_p->f_format.frame_type != 0xA0)//֡������ȷ
	{
		AfxMessageBox(_T("HDLC��ʽ���ʹ���!"));
		return;
	}
	u_short frame_length = hdlc_p->f_format.frame_sublen;//���֡���������ֵ��Ϊ�����ֽ�
	// u_char head_length = 2;//��¼ͷ���еĳ���
	//u_int d_addr_length = addr_length();
	//head_length = head_length +d_addr_length+1;//���ļ�һ�ǽ�Դ��ַ�ĳ���Ҳ�ӽ�ȥ��


	//�жϿ�����
	switch (GetTypes(*hdlc_p))
	{
	case TYPESNRM:
		m_liststateinfo.AddString(_T("֡����ΪSNRM��"));
		break;
	case TYPEDISC:
		m_liststateinfo.AddString(_T("֡����ΪDISC��"));
		break;
	case TYPEI:
		m_liststateinfo.AddString(_T("֡����ΪI��"));
		break;
	case TYPERR:
		m_liststateinfo.AddString(_T("֡����ΪRR��"));
		break;
	case TYPERNR:
		m_liststateinfo.AddString(_T("֡����ΪRNR��"));
		break;
	case TYPEUI:
		m_liststateinfo.AddString(_T("֡����ΪRNR��"));
		break;
	default:
		m_liststateinfo.AddString(_T("frmr:���������ʹ����Դ��ַ���ȳ���1�ֽڣ�"));
		gstpar->frmr_flag = 1;
		return;
	}

	
	//����HCSУ��-----------------------����h_cs_cal
	h_cs_val = h_cs_cal(str, hdlc_p->dst_addrlen);
	if (hdlc_p->h_cs != h_cs_val)
	{
		m_liststateinfo.AddString(_T("error:HCSУ���������"));
		return;
	}

	//֡������־���
	if (hdlc_p->end_flag != 0x7E)
	{
		m_liststateinfo.AddString(_T("error:֡���ճ�ʱ,û�н�β��־������"));//�ȴ���ʱ������û�м�⵽֡������־
		return;
	}

	m_liststateinfo.AddString(_T("һ֡���ս�����"));
	if (gstpar->started == 0)
	{
		gstpar->started = 1;//frame_ind = 1;//��־���Խ���״̬ѭ��
		gstpar->m = 0;
		gstpar->save_inf = save_inf;
		gstpar->hWnd = this;

		//fsmstack->curstate = STATE_NDM;
	}
	gstpar->nr = hdlc_p->nr;
	gstpar->ns = hdlc_p->ns;
	hdlc outframe;//���֡

	u_int i_length = hdlc_p->infolen;
	gstpar->frame_p_f = hdlc_p->pollfin;

	
	do{
		gstpar->isTransFinish = 1;
		GETHANDLER(fsmstack->curstate)(gstpar, hdlc_p, &outframe);
	} while (!gstpar->isTransFinish);


	if (gstpar->isUIWaiting == 1 && gstpar->canUISend == 1)
	{
		_TCHAR  sendStr[1024];
		u_char sendData[255];
		convFrameHex(&gUIFrame, write_str);
		convFrameStr(&gUIFrame, sendStr);
		CString m_strToServer = sendStr;
		writetxt((FILE*)wc, write_str, outframe.f_format.frame_sublen);
		nSentLen = m_sClientSocket.Send(m_strToServer, 1024);
		if (nSentLen != SOCKET_ERROR)
		{
			//���ͳɹ�
			m_listSent.AddString(_T("����UI֡"));
			m_listSent.AddString(m_strToServer);
		}
		gstpar->canUISend = 0;
		gstpar->isUIWaiting = 0;
	}

	if (gstpar->send_flag == 1)
	{
		_TCHAR  sendStr[1024];
		u_char sendData[255];
		convFrameHex(&outframe, write_str);
		convFrameStr(&outframe, sendStr);
		CString m_strToServer = sendStr;
		writetxt((FILE*)wc, write_str, outframe.f_format.frame_sublen);
		nSentLen = m_sClientSocket.Send(m_strToServer, 1024);
		if (nSentLen != SOCKET_ERROR)
		{
			//���ͳɹ�
			m_listSent.AddString(m_strToServer);
		}
	}


	UpdateData(FALSE);

	!gstpar->disc && SetTimer(1, 10000, NULL);

	return;
}

/*void CServerDlg::setHScroll()
{
CDC* dc = GetDC();
SIZE s;
int index;
CString str;
long temp;
for(index= 0; index< m_listSent.GetCount(); index++)
{
m_listSent.GetText(index,str);
s = dc->GetTextExtent(str,str.GetLength()+1);   // ��ȡ�ַ��������ش�С
// ����µ��ַ�����ȴ�����ǰ��ˮƽ��������ȣ����������ù��������
// IDC_LISTBOXΪm_List����ԴID
temp = (long)SendDlgItemMessage(IDC_LIST_SENT, LB_GETHORIZONTALEXTENT, 0, 0); //temp�õ��������Ŀ��
if (s.cx > temp)
{
SendDlgItemMessage(IDC_LIST_SENT, LB_SETHORIZONTALEXTENT, (WPARAM)s.cx,0);
}
}
ReleaseDC(dc);
}*/

afx_msg LRESULT CAboutDlg::OnInfo(WPARAM wParam, LPARAM lParam)
{
	return 0;
}


afx_msg LRESULT CServerDlg::OnInfo(WPARAM wParam, LPARAM lParam)
{
	_TCHAR *info = (_TCHAR*)wParam;
	m_liststateinfo.AddString(info);
	return 0;
}


//afx_msg LRESULT CServerDlg::OnTest(WPARAM wParam, LPARAM lParam)
//{
//	return 0;
//}


void CServerDlg::OnTimer(UINT_PTR nIDEvent)
{
	// TODO:  �ڴ������Ϣ�����������/�����Ĭ��ֵ
	u_int timerid = (u_int)nIDEvent;
	if (timerid == 1)
	{
		m_liststateinfo.AddString(_T("warning:Idle Timeout Disconnect!"));
		m_liststateinfo.SetCurSel(m_liststateinfo.GetCount() - 1);
	}
	_TCHAR outstr[255];
	hdlcpointer outframe = (hdlcpointer)malloc(sizeof(hdlc));
	makeDISC(gstpar, hdlc_p, outframe);
	memset(hdlc_p, 0, sizeof(*hdlc_p));//��̬����ռ䣬ȫ������
	FSMinit();
	gstpar->nr = 0;
	gstpar->ns = 0;
	gstpar->started = 0;
	gstpar->frame_p_f = 0;
	gstpar->rcv_num = 0;
	gstpar->send_num = 0;
	gstpar->frmr_flag = 0;
	gstpar->disc = 1;
	UpdateData(FALSE);
	KillTimer(timerid);
	CDialogEx::OnTimer(nIDEvent);
}


afx_msg LRESULT CServerDlg::OnReportEvent(WPARAM wParam, LPARAM lParam)
{
	u_char *data = (u_char*)wParam;
	u_int size = (u_int)lParam;
	for (u_int i = 0; i < size; i++)
	{
		gUIInfoBuf[i] = data[i];
	}
	gstpar->isUIWaiting = 1;
	return 0;
}


afx_msg LRESULT CAboutDlg::OnApplmsg(WPARAM wParam, LPARAM lParam)
{

	return 0;
}


afx_msg LRESULT CServerDlg::OnApplmsg(WPARAM wParam, LPARAM lParam)
{
	u_char *data = (u_char*)wParam;
	m_recdata = data;
	UpdateData(false);
	return 0;
}


afx_msg LRESULT CServerDlg::OnApplgetdata(WPARAM wParam, LPARAM lParam)
{
	u_char *pdata = (u_char *)wParam;
	u_int *plen = (u_int*)lParam;
	u_char data[] = "CAIDLKJFLDKJFLJDLKJFLJDLKFJLSDJLFD";
	*plen = sizeof(data) / sizeof(u_char);
	for (int i = 0; i < *plen; i++)
	{
		pdata[i] = data[i];
	}
	return 0;
}
