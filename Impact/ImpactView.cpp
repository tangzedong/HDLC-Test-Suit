// 这段 MFC 示例源代码演示如何使用 MFC Microsoft Office Fluent 用户界面 
// (“Fluent UI”)。该示例仅供参考，
// 用以补充《Microsoft 基础类参考》和 
// MFC C++ 库软件随附的相关电子文档。  
// 复制、使用或分发 Fluent UI 的许可条款是单独提供的。  
// 若要了解有关 Fluent UI 许可计划的详细信息，请访问  
// http://go.microsoft.com/fwlink/?LinkId=238214。
//
// 版权所有(C) Microsoft Corporation
// 保留所有权利。

// ImpactView.cpp : CImpactView 类的实现
//

#include "stdafx.h"
// SHARED_HANDLERS 可以在实现预览、缩略图和搜索筛选器句柄的
// ATL 项目中进行定义，并允许与该项目共享文档代码。
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
	// 标准打印命令
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

//事件处理

// CImpactView 构造/析构

CImpactView::CImpactView()
{
	// TODO:  在此处添加构造代码

}

CImpactView::~CImpactView()
{
}

BOOL CImpactView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO:  在此处通过修改
	//  CREATESTRUCT cs 来修改窗口类或样式

	return CView::PreCreateWindow(cs);
}

// CImpactView 绘制

void CImpactView::OnDraw(CDC* /*pDC*/)
{
	CImpactDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	if (!pDoc)
		return;

	// TODO:  在此处为本机数据添加绘制代码
}


// CImpactView 打印


void CImpactView::OnFilePrintPreview()
{
#ifndef SHARED_HANDLERS
	AFXPrintPreview(this);
#endif
}

BOOL CImpactView::OnPreparePrinting(CPrintInfo* pInfo)
{
	// 默认准备
	return DoPreparePrinting(pInfo);
}

void CImpactView::OnBeginPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO:  添加额外的打印前进行的初始化过程
}

void CImpactView::OnEndPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO:  添加打印后进行的清理过程
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


// CImpactView 诊断

#ifdef _DEBUG
void CImpactView::AssertValid() const
{
	CView::AssertValid();
}

void CImpactView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}

CImpactDoc* CImpactView::GetDocument() const // 非调试版本是内联的
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

// CImpactView 消息处理程序


int CImpactView::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CView::OnCreate(lpCreateStruct) == -1)
		return -1;

	// TODO:  在此添加您专用的创建代码
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

	// TODO:  在此处添加消息处理程序代码
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
	// TODO:  在此添加消息处理程序代码和/或调用默认值
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
	// TODO:  在此添加命令处理程序代码
	m_ctlCodeEdit->Clear();
}

void HandlerT::handler(System::Object ^sender, System::EventArgs ^e){
	ICSharpCode::AvalonEdit::TextEditor ^s = (ICSharpCode::AvalonEdit::TextEditor^)sender;
	s->ScrollToEnd();
}