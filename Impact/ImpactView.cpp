// ��� MFC ʾ��Դ������ʾ���ʹ�� MFC Microsoft Office Fluent �û����� 
// (��Fluent UI��)����ʾ�������ο���
// ���Բ��䡶Microsoft ������ο����� 
// MFC C++ ������渽����ص����ĵ���  
// ���ơ�ʹ�û�ַ� Fluent UI ����������ǵ����ṩ�ġ�  
// ��Ҫ�˽��й� Fluent UI ��ɼƻ�����ϸ��Ϣ�������  
// http://go.microsoft.com/fwlink/?LinkId=238214��
//
// ��Ȩ����(C) Microsoft Corporation
// ��������Ȩ����

// ImpactView.cpp : CImpactView ���ʵ��
//

#include "stdafx.h"
// SHARED_HANDLERS ������ʵ��Ԥ��������ͼ������ɸѡ�������
// ATL ��Ŀ�н��ж��壬�����������Ŀ�����ĵ����롣
#ifndef SHARED_HANDLERS
#include "Impact.h"
#endif

#include "ImpactDoc.h"
#include "ImpactView.h"
#include "IAppendLog.h"
#include <msclr/marshal_cppstd.h>
#ifdef _DEBUG
#define new DEBUG_NEW
#endif


using namespace msclr::interop;
using namespace System;
using namespace System::Windows;
using namespace System::Windows::Interop;
using namespace System::Windows::Media;
// CImpactView

IMPLEMENT_DYNCREATE(CImpactView, CView)

BEGIN_MESSAGE_MAP(CImpactView, CView)
	// ��׼��ӡ����
	ON_COMMAND(ID_FILE_PRINT, &CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_DIRECT, &CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_PREVIEW, &CImpactView::OnFilePrintPreview)
	ON_WM_CONTEXTMENU()
	ON_WM_RBUTTONUP()
	ON_WM_CREATE()
	ON_WM_MOVE()
	ON_WM_TIMER()
	ON_MESSAGE(WM_INFO, &CImpactView::OnInfo)
	ON_COMMAND(ID_BUTTON_CLS, &CImpactView::OnButtonCls)
END_MESSAGE_MAP()

//�¼�����

// CImpactView ����/����

CImpactView::CImpactView()
{
	// TODO:  �ڴ˴���ӹ������

}

CImpactView::~CImpactView()
{
}

BOOL CImpactView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO:  �ڴ˴�ͨ���޸�
	//  CREATESTRUCT cs ���޸Ĵ��������ʽ

	return CView::PreCreateWindow(cs);
}

// CImpactView ����

void CImpactView::OnDraw(CDC* /*pDC*/)
{
	CImpactDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	if (!pDoc)
		return;

	// TODO:  �ڴ˴�Ϊ����������ӻ��ƴ���
}


// CImpactView ��ӡ


void CImpactView::OnFilePrintPreview()
{
#ifndef SHARED_HANDLERS
	AFXPrintPreview(this);
#endif
}

BOOL CImpactView::OnPreparePrinting(CPrintInfo* pInfo)
{
	// Ĭ��׼��
	return DoPreparePrinting(pInfo);
}

void CImpactView::OnBeginPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO:  ��Ӷ���Ĵ�ӡǰ���еĳ�ʼ������
}

void CImpactView::OnEndPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO:  ��Ӵ�ӡ����е��������
}

void CImpactView::OnRButtonUp(UINT /* nFlags */, CPoint point)
{
	ClientToScreen(&point);
	OnContextMenu(this, point);
}

void CImpactView::OnContextMenu(CWnd* /* pWnd */, CPoint point)
{
#ifndef SHARED_HANDLERS
	theApp.GetContextMenuManager()->ShowPopupMenu(IDR_POPUP_EDIT, point.x, point.y, this, TRUE);
#endif
}


// CImpactView ���

#ifdef _DEBUG
void CImpactView::AssertValid() const
{
	CView::AssertValid();
}

void CImpactView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}

CImpactDoc* CImpactView::GetDocument() const // �ǵ��԰汾��������
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CImpactDoc)));
	return (CImpactDoc*)m_pDocument;
}
#endif //_DEBUG

ICSharpCode::AvalonEdit::TextEditor^ CImpactView::GetHwnd(int x, int y, int width, int height, HWND *hWnd) {
	HWND parent = this->m_hWnd;
	HwndSource^ source = gcnew HwndSource(
		0, // class style  
		WS_VISIBLE | WS_CHILD, // style  
		0, // exstyle  
		x, y, width, height,
		"hi", // NAME  
		IntPtr(parent)        // parent window   
		);

	ICSharpCode::AvalonEdit::TextEditor ^page = gcnew ICSharpCode::AvalonEdit::TextEditor();
	ICSharpCode::AvalonEdit::Highlighting::HighlightingManager ^ins = ICSharpCode::AvalonEdit::Highlighting::HighlightingManager::Instance;
	page->SyntaxHighlighting = ins->GetDefinition("HTML");
	page->ShowLineNumbers = true;
	source->RootVisual = page;
	*hWnd = (HWND)source->Handle.ToPointer();
	return page;
}

// CImpactView ��Ϣ�������


int CImpactView::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CView::OnCreate(lpCreateStruct) == -1)
		return -1;

	// TODO:  �ڴ������ר�õĴ�������
	RECT rectangle, rect2;
	GetClientRect(&rectangle);
	//ClientToScreen(&rectangle);
	int width = rectangle.right - rectangle.left;
	int height = rectangle.bottom - rectangle.top;
	POINT point;
	point.x = rectangle.left;
	point.y = rectangle.top;
	//int result = ::MapWindowPoints(NULL, this->m_hWnd, &point, 1);
	m_ctlCodeEdit = GetHwnd(point.x, point.y, width, height, &m_hCODE);
	//ModifyStyleEx(WS_EX_CLIENTEDGE, 0, TRUE);
	//ModifyStyleEx(WS_CAPTION, WS_DLGFRAME, SWP_FRAMECHANGED);
	m_ctlCodeEdit->AppendText(_T("[123]\n[334]\n"));
	m_ctlCodeEdit->TextChanged += gcnew System::EventHandler(HandlerT::handler);
	SetTimer(1, 1000, NULL);
	return 0;
}


void CImpactView::OnMove(int x, int y)
{
	CView::OnMove(x, y);

	// TODO:  �ڴ˴������Ϣ����������
	RECT rectangle, rect2;
	GetClientRect(&rectangle);
	//ClientToScreen(&rectangle);
	int width = rectangle.right - rectangle.left;
	int height = rectangle.bottom - rectangle.top;
	POINT point;
	point.x = rectangle.left;
	point.y = rectangle.top;
	//int result = ::MapWindowPoints(NULL, this->m_hWnd, &point, 1);
	::MoveWindow(m_hCODE, point.x, point.y, width, height, TRUE);
}

//void CImpactView::moveedit()
//{
//	RECT rectangle, rect2;
//	GetClientRect(&rectangle);
//	ClientToScreen(&rectangle);
//	int width = rectangle.right - rectangle.left;
//	int height = rectangle.bottom - rectangle.top;
//	POINT point;
//	point.x = rectangle.left;
//	point.y = rectangle.top;
//	//int result = ::MapWindowPoints(NULL, this->m_hWnd, &point, 1);
//	::MoveWindow(m_hCODE, point.x, point.y, 200, 200, TRUE);
//}

void CImpactView::OnTimer(UINT_PTR nIDEvent)
{
	// TODO:  �ڴ������Ϣ�����������/�����Ĭ��ֵ
	//char buf[32];
	//_strtime_s(buf, 32);
	//CString strMsg;
	//strMsg.Format(_T("[%s] %5s\n"), CString(buf), _T("df"));
	//String^ str = marshal_as<String^>(strMsg.GetBuffer());
	//page->AppendText(str);
	CView::OnTimer(nIDEvent);
}

void CImpactView::AppendLog(LPTSTR strLog)
{
	String^ str = marshal_as<String^>(strLog);
	m_ctlCodeEdit->AppendText(str);
	AfxMessageBox(_T("dff"), MB_OKCANCEL);
}

afx_msg LRESULT CImpactView::OnInfo(WPARAM wParam, LPARAM lParam)
{
	CString *strLog = (CString*)wParam;
	String^ str = marshal_as<String^>(strLog->GetBuffer());
	m_ctlCodeEdit->AppendText(str);
	delete strLog;
	return 0;
}


void CImpactView::OnButtonCls()
{
	// TODO:  �ڴ���������������
	m_ctlCodeEdit->Clear();
}

void HandlerT::handler(System::Object ^sender, System::EventArgs ^e){
	ICSharpCode::AvalonEdit::TextEditor ^s = (ICSharpCode::AvalonEdit::TextEditor^)sender;
	s->ScrollToEnd();
}