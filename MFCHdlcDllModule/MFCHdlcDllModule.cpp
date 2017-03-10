// MFCHdlcDllModule.cpp : 定义 DLL 的初始化例程。
//

#include "stdafx.h"
#include "MFCHdlcDllModule.h"
#include <afxstr.h>
#ifdef _DEBUG
#define new DEBUG_NEW
#endif

//
//TODO:  如果此 DLL 相对于 MFC DLL 是动态链接的，
//		则从此 DLL 导出的任何调入
//		MFC 的函数必须将 AFX_MANAGE_STATE 宏添加到
//		该函数的最前面。
//
//		例如: 
//
//		extern "C" BOOL PASCAL EXPORT ExportedFunction()
//		{
//			AFX_MANAGE_STATE(AfxGetStaticModuleState());
//			// 此处为普通函数体
//		}
//
//		此宏先于任何 MFC 调用
//		出现在每个函数中十分重要。  这意味着
//		它必须作为函数中的第一个语句
//		出现，甚至先于所有对象变量声明，
//		这是因为它们的构造函数可能生成 MFC
//		DLL 调用。
//
//		有关其他详细信息，
//		请参阅 MFC 技术说明 33 和 58。
//

// CMFCHdlcDllModuleApp

BEGIN_MESSAGE_MAP(CMFCHdlcDllModuleApp, CWinApp)
END_MESSAGE_MAP()


// CMFCHdlcDllModuleApp 构造

CMFCHdlcDllModuleApp::CMFCHdlcDllModuleApp()
{
	// TODO:  在此处添加构造代码，
	// 将所有重要的初始化放置在 InitInstance 中
}


// 唯一的一个 CMFCHdlcDllModuleApp 对象

CMFCHdlcDllModuleApp theApp;


// CMFCHdlcDllModuleApp 初始化

BOOL CMFCHdlcDllModuleApp::InitInstance()
{
	CWinApp::InitInstance();

	return TRUE;
}

//功能函数，用于查询接口
HWND hWndCaller = NULL;
extern "C" __declspec(dllexport) void QueryModule(__out LPTSTR szDescription, __in   int nMaxCount)
{
	TCHAR szModuleDesc[] = _T("hdlc测试模块");
	int n = _tcslen(szModuleDesc);
	n = n > nMaxCount - 1 ? nMaxCount - 1 : n;
	_tcsncpy(szDescription, szModuleDesc, n);
	szDescription[n] = '\0';
}

extern "C" __declspec(dllexport) void ConfigModule(void)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

}


extern "C" __declspec(dllexport) void InitModule(HWND hWnd)
{
	hWndCaller = hWnd;
}

extern "C" __declspec(dllexport) void DoTest()
{
	CString strMsg;
	for (int i = 1; i < 100; i++)
	{
		strMsg.Format(_T("%d"), i);
		Sleep(100);
	}
}