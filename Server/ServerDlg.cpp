
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
HdlcStationParam *gstpar = &_stpar;
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
END_MESSAGE_MAP()


// CServerDlg �Ի���




CServerDlg::CServerDlg(CWnd* pParent /*=NULL*/) //
	: CDialogEx(CServerDlg::IDD, pParent)
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

	HdlcTcb *pTcb = new HdlcTcb();
	pTcb->listhandler = (StateHandler)PrimaryStateHandler;
	FSMinit(NULL);
	fsmstack->curstate = STATE_NDM;//��ʼ��״̬��
	gstpar->nr = 0;
	gstpar->ns = 0;
	gstpar->started = 0;
	gstpar->frame_p_f = 0;
	gstpar->rcv_num = 0;
	gstpar->send_num = 0;
	gstpar->frmr_flag = 0;

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

	UpdateData(TRUE);   //ˢ�¿ؼ���ֵ����Ӧ�ı���(�ⲿ����ֵ�����ڲ�����)
	GetDlgItem(IDC_BUTTON_LISTEN)->EnableWindow(FALSE);
	GetDlgItem(IDC_EDIT_SERVERNAME)->EnableWindow(FALSE);
	GetDlgItem(IDC_EDIT_SERVERPORT)->EnableWindow(FALSE);
	m_sServerSocket.Create(m_nServerPort);//�����׽��֣������������������Լ��˿ںŰ�
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
	HdlcTcb *pTcb = new HdlcTcb();
	pTcb->fsmtype = FSMTypePrimary;
	pTcb->listhandler = (int(*)(HdlcTcb*, hdlc*, hdlc*))PrimaryStateHandler;
	pTcb->error = FSMenter(pTcb);
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
	default:
		AfxMessageBox(_T("���������ʹ����Դ��ַ���ȳ���1�ֽڣ�"), MB_OK | MB_ICONSTOP);
		gstpar->frmr_flag = 1;
		return;
	}

	
	//����HCSУ��-----------------------����h_cs_cal
	h_cs_val = h_cs_cal(str, hdlc_p->dst_addrlen);
	if (hdlc_p->h_cs != h_cs_val)
	{
		AfxMessageBox(_T("HCSУ���������"), MB_OK | MB_ICONSTOP);
		return;
	}

	//֡������־���
	if (hdlc_p->end_flag != 0x7E)
	{
		AfxMessageBox(_T("֡���ճ�ʱ,û�н�β��־������"), MB_OK | MB_ICONSTOP);//�ȴ���ʱ������û�м�⵽֡������־
		return;
	}

	m_liststateinfo.AddString(_T("һ֡���ս�����"));
	if (gstpar->started == 0)
	{
		gstpar->started = 1;//frame_ind = 1;//��־���Խ���״̬ѭ��
		gstpar->curstate = STATE_NDM;
		gstpar->m = 0;
		gstpar->save_inf = save_inf;
		gstpar->hWnd = this;
	}
	gstpar->nr = hdlc_p->nr;
	gstpar->ns = hdlc_p->ns;
	hdlc outframe;//���֡
	gstpar->send_flag = 0;//�Ƿ�����Ӧ֡
	u_int i_length = hdlc_p->infolen;
	gstpar->frame_p_f = hdlc_p->pollfin;
	//if (i_length == 0)
	//{
	//	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	//	////////////////////////////////////////////no information field��������/////////////////////////////////////////////////////
	//	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	//	//if(send_num !=  nr)//????
	//	//frmr_flag = 1;  
	//	//-----------ȡ��֡�䳬ʱ����------readoneline������һ�еĶ��룬���ؿ�ʼ����û��������֡

	//	//==========�ж�״̬=============������Ӧ===========����ת��========û����Ϣ��===
	//	/*	FILE *wc ;
	//	wc=fopen("write.txt","w");
	//	if(wc == NULL)
	//	{
	//	printf("failed to open file\n");
	//	return -1;
	//	}*/
	//	if (GetTypes(*hdlc_p) == TYPESNRM && tcb->started == 1 && tcb->frame_p_f == 1)    //֡����ΪSNRM��
	//	{
	//		if (tcb->curstate == STATE_NDM)
	//		{
	//			tcb->curstate = STATE_WAIT_CONNECT;
	//			tcb->curstate = STATE_WAIT_CONNECT;
	//		}
	//		switch (tcb->curstate)
	//		{
	//		case STATE_NDM: //��������������
	//		case STATE_WAIT_CONNECT://Ĭ���ϲ㷵��result�ǵ���OK									
	//			tcb->rcv_num = 0;
	//			tcb->send_num = 0;
	//			//make UA ֡������Ӧ  
	//			//doUAResponse();
	//			//make UA response;
	//			//DOHANDLER(WAITCON, SNRM)(&tcb, hdlc_p, &outframe);
	//			makeUA(tcb, hdlc_p, &outframe, settingdata, UAdatalen); 
	//			tcb->curstate = STATE_NRM;              //ת�Ƶ�NRM״̬
	//			m_liststateinfo.AddString(_T("�Ѿ�����NRM״̬��"));
	//			send_flag = 1;
	//			break;
	//		case STATE_NRM:
	//			//doNRM_SNRM();
	//			//make UA
	//			//DOHANDLER(NRM, SNRM)(&tcb, hdlc_p, &outframe);
	//			makeUA(tcb, hdlc_p, &outframe, settingdata, UAdatalen);
	//			m_liststateinfo.AddString(_T("�Ѿ���������״̬��"));
	//			send_flag = 1;
	//			break;
	//		case STATE_WAIT_DISCONNECT:m_liststateinfo.AddString(_T("����Ӧ��")); break;
	//		default:m_liststateinfo.AddString(_T("�����ڸ�״̬��"));
	//		}
	//	}
	//	else if (GetTypes(*hdlc_p) == TYPEDISC && tcb->started == 1 && tcb->frame_p_f == 1)     //֡����ΪDISC��
	//	{
	//		if (tcb->curstate == STATE_NRM)
	//		{
	//			tcb->curstate = STATE_WAIT_DISCONNECT;
	//		}
	//		switch (tcb->curstate)
	//		{
	//		case STATE_NDM:
	//			//doNDM_DISC();
	//			//make DM frame
	//			makeDM(tcb, hdlc_p, &outframe);
	//			m_liststateinfo.AddString(_T("�Ѿ�����NDM״̬��"));
	//			send_flag = 1;
	//			break;                                                              //��DM,��ת��
	//		case STATE_WAIT_CONNECT:
	//			m_liststateinfo.AddString(_T("����Ӧ��"));
	//			break;
	//		case STATE_NRM://��������������
	//		case STATE_WAIT_DISCONNECT://�ϲ�Ĭ��result ΪOK 
	//			//make UA
	//			//DOHANDLER(WAITDIS, DISC)(&tcb, hdlc_p, &outframe);
	//			//ͨ��send()����������Ӧ
	//			makeUA(tcb, hdlc_p, &outframe, settingdata, UAdatalen);
	//			send_flag = 1;
	//			tcb->curstate = STATE_NDM;
	//			break;                                                          //��UA��״̬ת�Ƶ�NDM
	//		default:
	//			m_liststateinfo.AddString(_T("�����ڸ�״̬��"));
	//		}
	//	}
	//	else if (GetTypes(*hdlc_p) == TYPERR && tcb->started == 1 && tcb->frame_p_f == 1)//֡����ΪRR��
	//	{
	//		switch (tcb->curstate)
	//		{
	//		case STATE_NDM:m_liststateinfo.AddString(_T("����Ӧ��")); break;
	//		case STATE_WAIT_CONNECT:m_liststateinfo.AddString(_T("����Ӧ��")); break;
	//		case STATE_NRM:
	//			//DOHANDLER(NRM, RR)(&tcb, hdlc_p, &outframe);
	//			//ͨ��send()����������Ӧ
	//			if (nr == tcb->send_num && tcb->frame_p_f == 1)
	//			{
	//				//make RR
	//				if (nr == 0)
	//				{
	//					makeRR(tcb, hdlc_p, &outframe, 0, tcb->frame_p_f);
	//				}
	//				else
	//				{
	//					tcb->send_num++;
	//					makeRR(tcb, hdlc_p, &outframe, tcb->rcv_num, tcb->frame_p_f);
	//				}
	//			}
	//			else
	//			{
	//				//make FRMR
	//				u_char errorinf[3];
	//				errorinf[0] = 0x10; errorinf[1] = 0x00; errorinf[2] = 0x20;
	//				len = 3;
	//				makeFRMR(tcb, hdlc_p, &outframe, errorinf, len);
	//			}
	//			send_flag = 1;
	//			m_liststateinfo.AddString(_T("����NRM״̬��"));
	//			break;
	//		case STATE_WAIT_DISCONNECT:	m_liststateinfo.AddString(_T("����Ӧ��")); break;
	//		default:m_liststateinfo.AddString(_T("�����ڸ�״̬��"));
	//		}
	//	}
	//	else if (GetTypes(*hdlc_p) == TYPERNR && tcb->started == 1 && tcb->frame_p_f == 1) //֡����ΪRNR��
	//	{
	//		switch (tcb->curstate)
	//		{
	//		case STATE_NDM:m_liststateinfo.AddString(_T("����Ӧ��")); break;
	//		case STATE_WAIT_CONNECT:m_liststateinfo.AddString(_T("����Ӧ��")); break;
	//		case STATE_NRM:
	//			m_liststateinfo.AddString(_T("δ׼���ý������ݣ���æ��"));
	//			break;
	//		case STATE_WAIT_DISCONNECT:m_liststateinfo.AddString(_T("����Ӧ��")); break;
	//		default:m_liststateinfo.AddString(_T("�����ڸ�״̬��"));
	//		}
	//	}
	//	else
	//	{
	//		m_liststateinfo.AddString(_T("����������͵�֡��"));
	//	}
	//}
	//else if (i_length > 0) //����Ϣ��Ĵ���
	//{
	//	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	//	////////////////////////////////////////////with information field���ݴ���///////////////////////////////////////////////////
	//	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	//	//��Ϊ��Ϣ�Ѿ����ձ����ˣ����Կ���ֱ�ӽ���FCSУ��
	//	f_cs_val = f_cs_cal(str, (hdlc_p->dst_addrlen + i_length));
	//	if (hdlc_p->f_cs != f_cs_val)
	//	{
	//		AfxMessageBox(_T("FCSУ��ʧ�ܣ�"), MB_OK | MB_ICONSTOP);
	//		return;
	//	}
	//	//-----------ȡ��֡�䳬ʱ����------readoneline������һ�еĶ��룬���ؿ�ʼ����û��������֡

	//	if (curtime - GetTickCount() > 250)
	//	{
	//		curtime = GetTickCount();
	//	}
	//	//-----------ȡ��֡�䳬ʱ����------readoneline������һ�еĶ��룬���ؿ�ʼ����û��������֡
	//	//==========�ж�״̬=============������Ӧ===========����ת��========����Ϣ��===
	//	if (GetTypes(*hdlc_p) == TYPESNRM && tcb->started == 1 && tcb->frame_p_f == 1)    //֡����ΪSNRM��
	//	{
	//		if (tcb->curstate == STATE_NDM)
	//			tcb->curstate = STATE_WAIT_CONNECT;

	//		int sendlen;
	//		CStringA tempstr;
	//		switch (tcb->curstate)
	//		{
	//		case STATE_NDM: //����NDM״̬,������
	//		case STATE_WAIT_CONNECT:  //Ĭ���ϲ㷴������ʱresult = OK
	//			tcb->rcv_num = 0;
	//			tcb->send_num = 0;
	//			//make UA ֡������Ӧ   ���в���Э��  
	//			//DOHANDLER(STATE_WAIT_CONNECT, SNRM)(&tcb, hdlc_p, write_str);
	//			makeUA(tcb, hdlc_p, &outframe, settingdata, UAdatalen);
	//			send_flag = 1;
	//			tcb->curstate = STATE_NRM;                              //ת�Ƶ�NRM״̬
	//			break;
	//		case STATE_NRM:m_liststateinfo.AddString(_T("�Ѿ��������ӣ�")); break;
	//		case STATE_WAIT_DISCONNECT:m_liststateinfo.AddString(_T("����Ӧ��")); break;
	//		default:m_liststateinfo.AddString(_T("�����ڸ�״̬��"));
	//		}
	//	}
	//	else if (GetTypes(*hdlc_p) == TYPEDISC && tcb->started == 1 && tcb->frame_p_f == 1)     //֡����ΪDISC��
	//	{
	//		//�����Ͽ����ֶβ�������Ϣ�ֶ�ʱ�����յ�������Ϣ�ֶε�֡��make FRMR
	//		u_char data[3] = {0x10, 0x00, 0x20};
	//		makeFRMR(tcb, hdlc_p, &outframe, data, 3);
	//	}
	//	else if (GetTypes(*hdlc_p) == TYPEI && tcb->started == 1 && tcb->frame_p_f == 1)     //֡����ΪI��
	//	{
	//		switch (tcb->curstate)
	//		{
	//		case STATE_NDM:m_liststateinfo.AddString(_T("����NDM״̬�������մ�����֡��")); break;
	//		case STATE_WAIT_CONNECT:break;
	//		case STATE_NRM:
	//			if (nr != tcb->send_num || ns != tcb->rcv_num) //��Ŵ�����
	//			{
	//				m_liststateinfo.AddString(_T("��Ŵ��󣬾ܾ����ո�֡��"));
	//				//make FRMR   ���
	//				u_char errorcode[3] = { 0x10, 0x00, 0x20 };
	//				makeFRMR(tcb, hdlc_p, &outframe, errorcode, 3);
	//				send_flag = 1;
	//				break;
	//			}

	//			if (hdlc_p->f_format.frame_seg == 0 && tcb->frame_p_f == 1 && m != 0)//�ֶε����һ֡����m����
	//			{
	//				//for(u_int n = 0;n < (m + 1);n++)//�ϱ��������Ҫ���ص���Ϣ��
	//				// {
	//				write_str[0] = 0x7E; write_str[1] = 0xA0; write_str[2] = 0x2E; write_str[3] = 0x21;
	//				write_str[4] = 0x02; write_str[5] = 0x23;
	//				u_char s_rcv = (ns + 1) << 5;
	//				u_char s_send = nr;
	//				write_str[6] = s_rcv | s_send | 16; write_str[7] = 0xE0;//HCSҪ��������ģ�����
	//				write_str[8] = 0xAE; write_str[9] = 0xE6; write_str[10] = 0xE7; write_str[11] = 0x00;
	//				write_str[12] = 0x61; write_str[13] = 0x1F; write_str[14] = 0xA1; write_str[15] = 0x09;
	//				write_str[16] = 0x06; write_str[17] = 0x07; write_str[18] = 0x60; write_str[19] = 0x85;
	//				write_str[20] = 0x74; write_str[21] = 0x05; write_str[22] = 0x08; write_str[23] = 0x01;
	//				write_str[24] = 0x01; write_str[25] = 0xA2; write_str[26] = 0x03; write_str[27] = 0x02;
	//				write_str[28] = 0x01; write_str[29] = 0x01; write_str[30] = 0xA3; write_str[31] = 0x05;
	//				write_str[32] = 0xA1; write_str[33] = 0x03; write_str[34] = 0x02; write_str[35] = 0x01;
	//				write_str[36] = 0x0D; write_str[37] = 0xBE; write_str[38] = 0x06; write_str[39] = 0x04;
	//				write_str[40] = 0x04; write_str[41] = 0x0E; write_str[42] = 0x01; write_str[43] = 0x06;
	//				write_str[44] = 0x01; write_str[45] = 0x6C;
	//				write_str[46] = 0x71;//FCS�Ǽ�������ģ�����
	//				write_str[47] = 0x7E;
	//				writetxt((FILE*)wc, write_str, 48);
	//				//ͨ��send()����������Ӧ
	//				m_strToServer.Format(_T("7E A0 2E 21 02 23 %02x E0 AE E6 E7 00 61 1F \
	//															A1 09 06 07 60 85 74 05 08 01 01 A2 03 02 01 01 A3 05 A1 03 02 01 0D \
	//																				BE 06 04 04 0E 01 06 01 6C 71 7E"), write_str[6]);
	//				send_flag = 1;
	//			}
	//			else if (hdlc_p->f_format.frame_seg == 0 && tcb->frame_p_f == 1 && m == 0)//��֤��������һ֡  FRAME_type ΪI_COMPLETE������֡
	//			{
	//				write_str[0] = 0x7E; write_str[1] = 0xA0; write_str[2] = 0x08; write_str[3] = 0x21;
	//				write_str[4] = 0x02; write_str[5] = 0x23;
	//				switch (tcb->rcv_num)
	//				{
	//				case 0:write_str[6] = 0x11;	write_str[7] = 0x6E; write_str[8] = 0x03; break;
	//				case 1:write_str[6] = 0x31; write_str[7] = 0x6C; write_str[8] = 0x22; break;
	//				case 2:write_str[6] = 0x51; write_str[7] = 0x6A; write_str[8] = 0x41; break;
	//				case 3:write_str[6] = 0x71; write_str[7] = 0x68; write_str[8] = 0x60; break;
	//				case 4:write_str[6] = 0x91; write_str[7] = 0x66; write_str[8] = 0x87; break;
	//				case 5:write_str[6] = 0xB1; write_str[7] = 0x64; write_str[8] = 0xA6; break;
	//				case 6:write_str[6] = 0xD1; write_str[7] = 0x62; write_str[8] = 0xC5; break;
	//				case 7:write_str[6] = 0xF1; write_str[7] = 0x60; write_str[8] = 0xE4; break;
	//				}
	//				write_str[9] = 0x7E;
	//				//д��txt�ļ�
	//				writetxt((FILE*)wc, write_str, 10);
	//				//ͨ��send()����������Ӧ
	//				m_strToServer.Format(_T("7E A0 08 21 02 23 %02x %02x %02x 7E"), write_str[6], write_str[7], write_str[8]);
	//				send_flag = 1;
	//			}
	//			else if (hdlc_p->f_format.frame_seg == 1 && tcb->frame_p_f == 0)//ÿһ����֡��һ����
	//			{
	//				for (u_int n = 0; n < i_length; n++)
	//				{
	//					save_inf[m] = hdlc_p->info_buff[n];
	//					m++;
	//				}
	//				tcb->rcv_num++;
	//				if (tcb->rcv_num % 8 == 0)
	//					tcb->rcv_num = 0;
	//			}
	//			else if (hdlc_p->f_format.frame_seg == 1 && tcb->frame_p_f == 1)//ÿһ����֡�����һ����
	//			{
	//				for (u_int n = 0; n < i_length; n++)
	//				{
	//					save_inf[m] = hdlc_p->info_buff[n];
	//					m++;
	//				}
	//				tcb->rcv_num++;
	//				if (tcb->rcv_num % 8 == 0)
	//					tcb->rcv_num = 0;
	//				//make RR   ��Ҫ���յ����ҲҪ��
	//				write_str[0] = 0x7E; write_str[1] = 0xA0; write_str[2] = 0x08; write_str[3] = 0x21;
	//				write_str[4] = 0x02; write_str[5] = 0x23;
	//				switch (tcb->rcv_num)
	//				{
	//				case 0:write_str[6] = 0x11;	write_str[7] = 0x6E; write_str[8] = 0x03; break;
	//				case 1:write_str[6] = 0x31; write_str[7] = 0x6C; write_str[8] = 0x22; break;
	//				case 2:write_str[6] = 0x51; write_str[7] = 0x6A; write_str[8] = 0x41; break;
	//				case 3:write_str[6] = 0x71; write_str[7] = 0x68; write_str[8] = 0x60; break;
	//				case 4:write_str[6] = 0x91; write_str[7] = 0x66; write_str[8] = 0x87; break;
	//				case 5:write_str[6] = 0xB1; write_str[7] = 0x64; write_str[8] = 0xA6; break;
	//				case 6:write_str[6] = 0xD1; write_str[7] = 0x62; write_str[8] = 0xC5; break;
	//				case 7:write_str[6] = 0xF1; write_str[7] = 0x60; write_str[8] = 0xE4; break;
	//				}
	//				write_str[9] = 0x7E;
	//				//������Ӧ
	//				m_strToServer.Format(_T("7E A0 08 21 02 23 %02x %02x %02x 7E"), write_str[6], write_str[7], write_str[8]);
	//				send_flag = 1;
	//			}
	//			//д��txt�ļ�
	//			UpdateData(FALSE);
	//			break;
	//		case STATE_WAIT_DISCONNECT:m_liststateinfo.AddString(_T("�����մ����͵�֡��")); break;
	//		default:m_liststateinfo.AddString(_T("û�д�״̬��"));
	//		}
	//	}
	//	else if (GetTypes(*hdlc_p) == TYPERR && tcb->started == 1 && tcb->frame_p_f == 1)//֡����ΪRR��
	//	{
	//		//�����Ͽ����ֶβ�������Ϣ�ֶ�ʱ�����յ�������Ϣ�ֶε�֡��make FRMR
	//		u_char data[3] = { 0x10, 0x00, 0x20 };
	//		makeFRMR(tcb, hdlc_p, &outframe, data, 3);
	//		send_flag = 1;
	//	}
	//	else if (GetTypes(*hdlc_p) == TYPERNR && tcb->started == 1 && tcb->frame_p_f == 1) //֡����ΪRNR��
	//	{
	//		//�����Ͽ����ֶβ�������Ϣ�ֶ�ʱ�����յ�������Ϣ�ֶε�֡��make FRMR
	//		u_char data[3] = { 0x10, 0x00, 0x20 };
	//		makeFRMR(tcb, hdlc_p, &outframe, data, 3);
	//		send_flag = 1;
	//	}
	//	else m_liststateinfo.AddString(_T("����������͵�֡��"));
	//}


	GETHANDLER(tcb->curstate)(tcb, hdlc_p, &outframe);
	if (tcb->send_flag == 1)
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
