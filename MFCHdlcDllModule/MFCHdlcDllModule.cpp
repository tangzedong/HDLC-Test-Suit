// MFCHdlcDllModule.cpp : ���� DLL �ĳ�ʼ�����̡�
//

#include "stdafx.h"
#include "MFCHdlcDllModule.h"
#include <afxstr.h>
#ifdef _DEBUG
#define new DEBUG_NEW
#endif

//
//TODO:  ����� DLL ����� MFC DLL �Ƕ�̬���ӵģ�
//		��Ӵ� DLL �������κε���
//		MFC �ĺ������뽫 AFX_MANAGE_STATE ����ӵ�
//		�ú�������ǰ�档
//
//		����: 
//
//		extern "C" BOOL PASCAL EXPORT ExportedFunction()
//		{
//			AFX_MANAGE_STATE(AfxGetStaticModuleState());
//			// �˴�Ϊ��ͨ������
//		}
//
//		�˺������κ� MFC ����
//		������ÿ��������ʮ����Ҫ��  ����ζ��
//		��������Ϊ�����еĵ�һ�����
//		���֣������������ж������������
//		������Ϊ���ǵĹ��캯���������� MFC
//		DLL ���á�
//
//		�й�������ϸ��Ϣ��
//		����� MFC ����˵�� 33 �� 58��
//

// CMFCHdlcDllModuleApp

BEGIN_MESSAGE_MAP(CMFCHdlcDllModuleApp, CWinApp)
END_MESSAGE_MAP()


// CMFCHdlcDllModuleApp ����

CMFCHdlcDllModuleApp::CMFCHdlcDllModuleApp()
{
	// TODO:  �ڴ˴���ӹ�����룬
	// ��������Ҫ�ĳ�ʼ�������� InitInstance ��
}


// Ψһ��һ�� CMFCHdlcDllModuleApp ����

CMFCHdlcDllModuleApp theApp;


// CMFCHdlcDllModuleApp ��ʼ��

BOOL CMFCHdlcDllModuleApp::InitInstance()
{
	CWinApp::InitInstance();

	return TRUE;
}

//���ܺ��������ڲ�ѯ�ӿ�
HWND hWndCaller = NULL;
extern "C" __declspec(dllexport) void QueryModule(__out LPTSTR szDescription, __in   int nMaxCount)
{
	TCHAR szModuleDesc[] = _T("hdlc����ģ��");
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