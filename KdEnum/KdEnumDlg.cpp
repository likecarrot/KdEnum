﻿
// KdEnumDlg.cpp: 实现文件
//

#include "pch.h"
#include "framework.h"
#include "KdEnum.h"
#include "KdEnumDlg.h"
#include "afxdialogex.h"

#include	<Windows.h>
#include	<winsvc.h>
#include	<TlHelp32.h>
#include	"def.h"
#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// 用于应用程序“关于”菜单项的 CAboutDlg 对话框

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

	// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_ABOUTBOX };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

// 实现
protected:
	DECLARE_MESSAGE_MAP()
public:
};

CAboutDlg::CAboutDlg() : CDialogEx(IDD_ABOUTBOX)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()


// CKdEnumDlg 对话框



CKdEnumDlg::CKdEnumDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_KDENUM_DIALOG, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}


void CKdEnumDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST1, m_list);
}

BEGIN_MESSAGE_MAP(CKdEnumDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_WM_DESTROY()
	ON_NOTIFY(NM_RCLICK, IDC_LIST1, &CKdEnumDlg::OnNMRClickList1)
	ON_COMMAND(ID_M_32772, &CKdEnumDlg::OnMenuItemUpdata)
	ON_COMMAND(ID_32771, &CKdEnumDlg::OnMenuItemTerminate)
END_MESSAGE_MAP()


// CKdEnumDlg 消息处理程序

BOOL CKdEnumDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// 将“关于...”菜单项添加到系统菜单中。

	// IDM_ABOUTBOX 必须在系统命令范围内。
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != nullptr)
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

	// 设置此对话框的图标。  当应用程序主窗口不是对话框时，框架将自动
	//  执行此操作
	SetIcon(m_hIcon, TRUE);			// 设置大图标
	SetIcon(m_hIcon, FALSE);		// 设置小图标

	// TODO: 在此添加额外的初始化代码
	initList();
	initExportFunc();
	EnumProcess();
	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

void CKdEnumDlg::OnSysCommand(UINT nID, LPARAM lParam)
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

// 如果向对话框添加最小化按钮，则需要下面的代码
//  来绘制该图标。  对于使用文档/视图模型的 MFC 应用程序，
//  这将由框架自动完成。

void CKdEnumDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 用于绘制的设备上下文

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 使图标在工作区矩形中居中
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 绘制图标
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

//当用户拖动最小化窗口时系统调用此函数取得光标
//显示。
HCURSOR CKdEnumDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

void CKdEnumDlg::initList()
{
	m_list.SetExtendedStyle(LVS_EX_GRIDLINES | LVS_EX_FULLROWSELECT);
	m_list.InsertColumn(0, L"进程名", 0, 130);
	m_list.InsertColumn(1, L"进程id", 0, 50);
	m_list.InsertColumn(2, L"父进程id", 0, 60);
	m_list.InsertColumn(3, L"优先级", 0, 50);
	m_list.InsertColumn(4, L"线程数量", 0, 60);
	m_list.InsertColumn(5, L"文件路径", 0, 300);
}

VOID CKdEnumDlg::initExportFunc()
{
	HMODULE	hModule = LoadLibrary(L"ntdll.dll");
	if (hModule == INVALID_HANDLE_VALUE)
	{
		CKdEnumDlg::InitExportFun = FALSE;
	}
	CKdEnumDlg::ZwQueryInformationProcess = (PFN_ZwQueryInformationProcess)GetProcAddress(hModule, "ZwQueryInformationProcess");
	if (ZwQueryInformationProcess == 0)
	{
		CKdEnumDlg::InitExportFun = FALSE;

	}

	CKdEnumDlg::InitExportFun = TRUE;
}

BOOL CKdEnumDlg::EnumProcess()
{

	PROCESSENTRY32	PeEntry = { 0 };
	HANDLE	hSnap = NULL;
	PeEntry.dwSize = sizeof(PROCESSENTRY32);
	hSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
	if (hSnap == INVALID_HANDLE_VALUE)
	{
		return	FALSE;
	}
	BOOL ret = Process32First(hSnap, &PeEntry);
	WCHAR cntThreads[MAX_PATH], pcPriClassBase[MAX_PATH], \
		ProcessId[MAX_PATH], th32ParentProcessID[MAX_PATH], ProcessName[MAX_PATH];
	WCHAR	processpath[MAX_PATH];
	ULONG	index = 0;
	while (ret)
	{
		wsprintf(cntThreads, L"%d", PeEntry.cntThreads);
		wsprintf(pcPriClassBase, L"%d", PeEntry.pcPriClassBase);
		wsprintf(ProcessId, L"%d", PeEntry.th32ProcessID);
		wsprintf(th32ParentProcessID, L"%d", PeEntry.th32ParentProcessID);
		m_list.InsertItem(index, PeEntry.szExeFile);
		m_list.SetItemText(index, 1, ProcessId);
		m_list.SetItemText(index, 2, th32ParentProcessID);
		m_list.SetItemText(index, 3, pcPriClassBase);
		m_list.SetItemText(index, 4, cntThreads);
		GetModulePath(PeEntry.th32ProcessID, processpath);
		m_list.SetItemText(index, 5, processpath);
		index++;
		ret = Process32Next(hSnap, &PeEntry);
	}
	return	TRUE;
}

VOID CKdEnumDlg::InsertList()
{
}

ULONG CKdEnumDlg::LookupDebugPort(ULONG pid)
{

	return 0;
}

BOOL CKdEnumDlg::GetModulePath(ULONG pid, WCHAR path[])
{
	if (pid == 0)
	{
		wcscpy_s(path, MAX_PATH, L"[System Path]");
		return	TRUE;
	}
	MODULEENTRY32	mo = { 0 };
	mo.dwSize = sizeof(MODULEENTRY32);
	HANDLE hModule = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE | TH32CS_SNAPMODULE32, pid);
	if (hModule == INVALID_HANDLE_VALUE)
	{
		int roo = GetLastError();
		return	 FALSE;
	}
	Module32First(hModule, &mo);
	CloseHandle(hModule);
	wcscpy_s(path, MAX_PATH, mo.szExePath);
	return	TRUE;
}

VOID CKdEnumDlg::TerminateProcessShell(ULONG pid)
{
	CHAR	cmd[256];
	sprintf_s(cmd,"taskkill /pid %d /F",pid);
	system(cmd);
	return VOID();
}



void CKdEnumDlg::OnNMRClickList1(NMHDR* pNMHDR, LRESULT* pResult)
{
	NM_LISTVIEW* pNMListView = (NM_LISTVIEW*)pNMHDR;
	if (pNMListView->iItem != -1)
	{
		DWORD dwPos = GetMessagePos();
		CPoint point(LOWORD(dwPos), HIWORD(dwPos));
		CMenu menu;
		//添加线程操作
		VERIFY(menu.LoadMenu(IDR_MENU1));			//这里是我们在1中定义的MENU的文件名称
		CMenu* popup = menu.GetSubMenu(0);
		ASSERT(popup != NULL);
		popup->TrackPopupMenu(TPM_LEFTALIGN | TPM_RIGHTBUTTON, point.x, point.y, this);

		//下面的两行代码主要是为了后面的操作为准备的
		//获取列表视图控件中第一个被选择项的位置  
		POSITION m_pstion = m_list.GetFirstSelectedItemPosition();
		//该函数获取由pos指定的列表项的索引，然后将pos设置为下一个位置的POSITION值
		int m_nIndex = m_list.GetNextSelectedItem(m_pstion);

	}
}


void CKdEnumDlg::OnMenuItemUpdata()
{
	m_list.DeleteAllItems();
	EnumProcess();
}


void CKdEnumDlg::OnMenuItemTerminate()
{
	POSITION m_pstion = m_list.GetFirstSelectedItemPosition();
	//该函数获取由pos指定的列表项的索引，然后将pos设置为下一个位置的POSITION值
	int m_nIndex = m_list.GetNextSelectedItem(m_pstion);
	CString i = m_list.GetItemText(m_nIndex, 1);
	ULONG	pid = _wtoi(i);
	TerminateProcessShell(pid);
	m_list.DeleteAllItems();
	EnumProcess();
}