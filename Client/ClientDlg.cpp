
// ClientDlg.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "Client.h"
#include "ClientDlg.h"
#include "afxdialogex.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

UINT fileLen=0; //�ļ����ȣ���ʼ��Ϊ0
FILE* fq;
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
public:
//	afx_msg void OnSize(UINT nType, int cx, int cy);
};

CAboutDlg::CAboutDlg() : CDialogEx(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
//	ON_WM_SIZE()
END_MESSAGE_MAP()


// CClientDlg �Ի���




CClientDlg::CClientDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CClientDlg::IDD, pParent)
	, m_strSendData(_T(""))
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	m_strServerName = _T("");
	m_nServerPort = 0;
	m_editfile = _T("");
	//m_listcontent = _T("");
	m_listToServer = _T("");
}

void CClientDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT_SERVERNAME, m_strServerName);
	DDX_Text(pDX, IDC_EDIT_SERVERPORT, m_nServerPort);
	DDV_MinMaxInt(pDX, m_nServerPort, 1024, 49151);
	//	DDX_Text(pDX, IDC_EDIT_TOSERVER, m_strToServer);
	DDX_Control(pDX, IDC_LIST_RECEIVED, m_listReceived);
	DDX_Control(pDX, IDC_LIST_SENT, m_listSent);
	DDX_Control(pDX, IDC_LIST_CONTENT, m_listcontent);
	DDX_Text(pDX, IDC_EDIT_FILE, m_editfile);
	//  DDX_Text(pDX, IDC_LIST_CONTENT, m_listcontent);
	//  DDX_Control(pDX, IDC_LIST_CONTENT, m_listcontent);
	DDX_LBString(pDX, IDC_LIST_CONTENT, m_listToServer);
	DDX_Text(pDX, IDC_EDIT1, m_strSendData);
}

BEGIN_MESSAGE_MAP(CClientDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	//ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()

	ON_BN_CLICKED(IDC_BUTTON_CONNECT, &CClientDlg::OnClickedButtonConnect)
	ON_BN_CLICKED(IDC_BUTTON_DISCONNECT, &CClientDlg::OnClickedButtonDisconnect)
	ON_BN_CLICKED(IDC_BUTTON_SEND, &CClientDlg::OnClickedButtonSend)
	ON_BN_CLICKED(IDC_BUTTON_VIEW, &CClientDlg::OnBnClickedButtonView)//
	ON_EN_CHANGE(IDC_EDIT_FILE, &CClientDlg::OnEnChangeEditFile)
	ON_LBN_SELCHANGE(IDC_LIST_RECEIVED, &CClientDlg::OnSelchangeListReceived)
	ON_LBN_SELCHANGE(IDC_LIST_SENT, &CClientDlg::OnSelchangeListSent)
	//ON_EN_CHANGE(IDC_EDIT_CONTENT, &CClientDlg::OnChangeEditContent)
//	ON_COMMAND(IDC_BUTTON3, &CClientDlg::OnButton3)
    ON_LBN_SELCHANGE(IDC_LIST_CONTENT, &CClientDlg::OnSelchangeListContent)
	ON_WM_SIZE()
	ON_BN_CLICKED(IDC_BUTTONMAKE, &CClientDlg::OnBnClickedButtonmake)
	ON_MESSAGE(WM_AUTOSEND, &CClientDlg::OnAutoSend)
END_MESSAGE_MAP()

 
// CClientDlg ��Ϣ�������

BOOL CClientDlg::OnInitDialog()
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
	m_strServerName="localhost";    //�ͻ���Ҫ���ӵķ��������������趨����λ������
	m_nServerPort=1024;             //�������˿�
	UpdateData(FALSE);              //��������ֵ(��localhost��1024)����Ӧ�Ŀؼ���ʾ
	m_sClientSocket.setParentDlg(this);  //��ָ��CClientDlg���ָ��this����CClientSocket���У���ʱthis=m_pDlg
	GetDlgItem(IDC_LIST_CONTENT)->EnableWindow(TRUE);   //���÷�����Ϣ�ı���
	GetDlgItem(IDC_BUTTON_DISCONNECT)->EnableWindow(FALSE);   //���öϿ���ť
	GetDlgItem(IDC_BUTTON_SEND)->EnableWindow(TRUE);       //���÷��Ͱ�ť
	SendDlgItemMessage(IDC_LIST_SENT,LB_SETHORIZONTALEXTENT,(WPARAM)1000,0);
	SendDlgItemMessage(IDC_LIST_RECEIVED,LB_SETHORIZONTALEXTENT,(WPARAM)1000,0);
	SendDlgItemMessage(IDC_LIST_CONTENT,LB_SETHORIZONTALEXTENT,(WPARAM)1000,0);

	m_makedlg.Create(IDD_HDLCFRAMEGEN_DIALOG, this);
	m_makedlg.ShowWindow(SW_HIDE);
	return TRUE;  // ���ǽ��������õ��ؼ������򷵻� TRUE
}

void CClientDlg::OnSysCommand(UINT nID, LPARAM lParam)
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

void CClientDlg::OnPaint()
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
HCURSOR CClientDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}
//���������ť
void CClientDlg::OnBnClickedButtonView()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	BROWSEINFO broInfo={0};  //����һ��BROWSEINFO����
	TCHAR szDisName[MAX_PATH]={0};  //����һ���ַ������������ļ���

	broInfo.hwndOwner=this->m_hWnd; //�ƶ������ڣ��ڶԻ�����ʾ�ڼ䣬�����ڽ�������
	broInfo.pidlRoot=NULL;   //������Ϊ��
	broInfo.pszDisplayName=szDisName;   //��������Ŀ¼�ַ������ڴ��ַ
	broInfo.lpszTitle=_T("ѡ��Ҫ��ȡ���ļ�");  //�Ի������
	//����״̬�ı�����ʾĿ¼���ļ���ֻ�����ļ�ϵͳĿ¼������ϵͳһ���ڵ㡢
	broInfo.ulFlags=BIF_STATUSTEXT|BIF_BROWSEINCLUDEFILES|BIF_RETURNONLYFSDIRS|BIF_RETURNFSANCESTORS ;  
	broInfo.lpfn=NULL;    //Ӧ�ó����������Ի���ص������ĵ�ַ  
	broInfo.lParam=NULL;   //�Ի��򴫵ݸ��ص�������һ������ָ��
	broInfo.iImage=IDR_MAINFRAME;   //��ѡ��Ŀ¼��ص�ͼ�񡣸�ͼ�񽫱�ָ��Ϊϵͳͼ���б��е�����ֵ

	LPITEMIDLIST pIDList=SHBrowseForFolder(&broInfo);  //����һ���ṹָ��洢·��
	if(pIDList!=NULL)
	{
		//����û�������ȷ����
		memset(szDisName,0,sizeof(szDisName));
		SHGetPathFromIDList(pIDList,szDisName);   //����Ŀ��ʶ�б�ת�����ַ���
		m_editfile=szDisName;   //��TCHAR���͵��ַ���ת��ΪCString���͵��ַ���
		
		//��ȡ�ļ����ݲ���ʾ�ڱ༭����
		CStdioFile file;//��ȡһ��
		file.Open (m_editfile,CFile::modeRead | CFile::typeBinary);
		CString strRead;
		char p_str[255];
		//file.ReadString(strRead);
		while(file.ReadString(strRead))
		{
			strcpy(p_str, (char*)strRead.GetBuffer());
			strRead = p_str;
			m_listcontent.AddString (strRead);
		}
		UpdateData(FALSE);       //��·���ַ������ļ����ݷֱ���ʾ�ڱ༭����
	//	delete pBuf;
		file.Close();
	}
}

//�������Ӱ�ť
void CClientDlg::OnClickedButtonConnect()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	UpdateData(TRUE);     //ˢ�¿ؼ���ֵ����Ӧ�ı���(�����������Ͷ˿ں�)
	GetDlgItem(IDC_BUTTON_CONNECT)->EnableWindow(FALSE);   //�������Ӱ�ť
	GetDlgItem(IDC_EDIT_SERVERNAME)->EnableWindow(FALSE);  //���÷��������ı���
	GetDlgItem(IDC_EDIT_SERVERPORT)->EnableWindow(FALSE);   //���ö˿ں��ı���
	m_sClientSocket.Create();  //����Socket��WSAStartup���̺�bind���̱��򻯳�ΪCreate����
	m_sClientSocket.Connect(m_strServerName,m_nServerPort);//���ӳɹ���ʧ�ܺ�ᴥ�� OnConnect()����
}

void CClientDlg::OnClickedButtonDisconnect()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	onClose();
}


void CClientDlg::OnClickedButtonSend()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	int nMsgLen;
	int nSentLen;
	UpdateData(TRUE);
	if(!m_listToServer.IsEmpty()||!m_strSendData.IsEmpty())
	{
		//nMsgLen=m_listToServer.GetLength()*sizeof(m_listToServer);
		m_listToServer = m_strSendData;
		int len = WideCharToMultiByte(CP_ACP, 0, m_listToServer, -1, NULL, 0, NULL, NULL);
		char *ptxtTemp = new char[len + 1];
		WideCharToMultiByte(CP_ACP, 0, m_listToServer, -1, ptxtTemp, len, NULL, NULL);
		nMsgLen = len;
		nSentLen = m_sClientSocket.Send(ptxtTemp, nMsgLen);
		delete ptxtTemp;
		ptxtTemp = NULL;
		if(nSentLen!=SOCKET_ERROR){
		     //���ͳɹ�
			m_listSent.AddString(m_listToServer);
		    UpdateData(FALSE);    //��������ֵ(��m_strToServer)����Ӧ�Ŀؼ���ʾ
		} 
		else{
		       AfxMessageBox(LPCTSTR("�ͻ����������������Ϣ���ִ���"),MB_OK|MB_ICONSTOP);
		}
		m_listToServer.Empty();
	    UpdateData(FALSE);
	}
}

//�ͻ��������ӵ���������
void CClientDlg::onConnect(void)
{
	GetDlgItem(IDC_LIST_CONTENT)->EnableWindow(TRUE);
	GetDlgItem(IDC_BUTTON_DISCONNECT)->EnableWindow(TRUE);
	GetDlgItem(IDC_BUTTON_SEND)->EnableWindow(TRUE);
}



void CClientDlg::onReceive(void)
{
	TCHAR buff[2048];
	int nBufferSize=2048;
	int nReceiveLen;
	CString strReceive;
	memset(buff,0,2048);
	
	nReceiveLen=m_sClientSocket.Receive(buff,nBufferSize);//Receive()�������ú�ᴥ��OnReceive()����
	if(nReceiveLen!=SOCKET_ERROR)
	{
	    //���ܳɹ�
	    CString szTemp(buff);   //��buff��charת��ΪCString����
	    strReceive=szTemp;
		m_listReceived.AddString(strReceive);  //AddString����Ҫ��ΪLPCTSTR(CString)����
	    UpdateData(FALSE);   //��������ֵ(��strReceive)����Ӧ�Ŀؼ���ʾ
	}
	else {
	        AfxMessageBox(LPCTSTR("�ͻ����������������Ϣ���ִ���"),MB_OK|MB_ICONSTOP);
	}

}


void CClientDlg::onClose(void)
{
	m_sClientSocket.Close();      //Close()��������ִ�гɹ���ʧ�ܶ��ᴥ��OnClose()����
	//GetDlgItem(IDC_EDIT_TOSERVER)->EnableWindow(FALSE);
	GetDlgItem(IDC_BUTTON_DISCONNECT)->EnableWindow(FALSE);
	GetDlgItem(IDC_BUTTON_SEND)->EnableWindow(FALSE);
	
	//��������б����Ϣ
	while(m_listSent.GetCount()!=0)
		m_listSent.DeleteString(0);
	while(m_listReceived.GetCount()!=0)
		m_listReceived.DeleteString(0);
	//���°�ť���ı���״̬
	GetDlgItem(IDC_EDIT_SERVERNAME)->EnableWindow(TRUE);
	GetDlgItem(IDC_EDIT_SERVERPORT)->EnableWindow(TRUE);
	GetDlgItem(IDC_BUTTON_CONNECT)->EnableWindow(TRUE);
}


void CClientDlg::OnEnChangeEditFile()
{
	// TODO:  ����ÿؼ��� RICHEDIT �ؼ���������
	// ���ʹ�֪ͨ��������д CDialogEx::OnInitDialog()
	// ���������� CRichEditCtrl().SetEventMask()��
	// ͬʱ�� ENM_CHANGE ��־�������㵽�����С�

	// TODO:  �ڴ���ӿؼ�֪ͨ����������
}


void CClientDlg::OnSelchangeListReceived()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
}

void CClientDlg::OnSelchangeListSent()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
}


void CClientDlg::OnSelchangeListContent()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	int i=m_listcontent.GetCurSel();
	m_listcontent.GetText(i,m_listToServer);//�����ѡ��һ�е��ַ���
	//		int nSentLen=m_sClientSocket.Send(m_listToServer,150);
}


//void CAboutDlg::OnSize(UINT nType, int cx, int cy)
//{
//	CDialogEx::OnSize(nType, cx, cy);
//
//	// TODO:  �ڴ˴������Ϣ����������
//}


void CClientDlg::OnSize(UINT nType, int cx, int cy)
{
	CDialogEx::OnSize(nType, cx, cy);

	// TODO:  �ڴ˴������Ϣ����������
}


void CClientDlg::OnBnClickedButtonmake()
{
	// TODO:  �ڴ���ӿؼ�֪ͨ����������
	m_makedlg.ShowWindow(m_makedlg.IsWindowVisible() ? SW_HIDE : SW_SHOW);
}


afx_msg LRESULT CClientDlg::OnAutoSend(WPARAM wParam, LPARAM lParam)
{
	int nLen, nState;
	char szTemp[255];
	nState = m_sClientSocket.Send(szTemp, nLen);
	return 0;
}
