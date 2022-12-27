
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
#include	"include/json/json.h"
#include	<string>
#include	<iostream>
#include	<fstream>
#include	<map>
#include	<vector>
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
	StartUp();
	InitResource();
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

void CKdEnumDlg::InitResource()
{
	//初始化表格
	m_list.SetExtendedStyle(LVS_EX_GRIDLINES | LVS_EX_FULLROWSELECT);
	if (g_EnableFlag.Enable_ProcessName)
	{
		m_list.InsertColumn(0, L"进程名", 0, 130);
	}
	if (g_EnableFlag.Enable_ProcessId)
	{
		m_list.InsertColumn(1, L"进程id", 0, 50);
	}
	if (g_EnableFlag.Enable_ParentId)
	{
		m_list.InsertColumn(2, L"父进程id", 0, 60);
	}
	if (g_EnableFlag.Enable_Priority)
	{
		m_list.InsertColumn(3, L"优先级", 0, 50);
	}
	if (g_EnableFlag.Enable_ThreadNums)
	{
		m_list.InsertColumn(4, L"线程数量", 0, 60);
	}
	if (g_EnableFlag.Enable_Debugport)
	{
		m_list.InsertColumn(5, L"DebugPort", 0, 80);
	}
	if (g_EnableFlag.Enable_ProcessPath)
	{
		m_list.InsertColumn(6, L"文件路径", 0, 300);
	}
	
	m_Menu.LoadMenuW(IDR_MENU2);
	SetMenu(&m_Menu);
}

VOID CKdEnumDlg::StartUp()
{
	if (SHGetFolderPath(NULL, CSIDL_APPDATA, NULL, 0, g_Global.AppdataPath) == S_OK) {
		HANDLE	hFile = NULL;
		wsprintf(g_Global.ConfigFilePath, L"%ws\\kdEnum\\config.json", g_Global.AppdataPath);
		wsprintf(g_Global.ConfigPath, L"%ws\\kdEnum", g_Global.AppdataPath);
		hFile=CreateFile(g_Global.ConfigFilePath, GENERIC_READ | GENERIC_WRITE,
			FILE_SHARE_DELETE | FILE_SHARE_READ, NULL, OPEN_EXISTING,  FILE_ATTRIBUTE_NORMAL,NULL);
		if (hFile==INVALID_HANDLE_VALUE)
		{//第一次打开程序
			CreateDirectory(g_Global.ConfigPath, NULL);
			hFile = CreateFile(g_Global.ConfigFilePath, GENERIC_READ | GENERIC_WRITE,
				FILE_SHARE_DELETE | FILE_SHARE_READ, NULL, CREATE_NEW, FILE_ATTRIBUTE_NORMAL, NULL);
			DWORD	ret = 0;
			WriteFile(hFile, g_Global.DefaultConfig, strlen(g_Global.DefaultConfig), &ret, 0);
			if (ret>strlen(g_Global.DefaultConfig))
			{
				int a = 0;
				a = strlen(g_Global.DefaultConfig);
				return;
			}
		}
		//加载config.json
		Json::Reader	reader;
		Json::Value	root;
		using namespace std;
		std::ifstream	file;
		file.open(g_Global.ConfigFilePath, ios::in);
		if (file.is_open())
		{//文件流已经打开
			if (reader.parse(file, root, false)) {
				g_EnableFlag.Enable_Debugport = root["settings"]["Enable_Debugport"].asBool();
				g_EnableFlag.Enable_ParentId = root["settings"]["Enable_ParentId"].asBool();
				g_EnableFlag.Enable_Priority = root["settings"]["Enable_Priority"].asBool();
				g_EnableFlag.Enable_ProcessId = root["settings"]["Enable_ProcessId"].asBool();
				g_EnableFlag.Enable_ProcessName = root["settings"]["Enable_ProcessName"].asBool();
				g_EnableFlag.Enable_ProcessPath = root["settings"]["Enable_ProcessPath"].asBool();
				g_EnableFlag.Enable_ThreadNums = root["settings"]["Enable_ThreadNums"].asBool();

				g_EnableFlag.DriverMode = root["mode"]["DriverMode"].asBool();
				g_EnableFlag.UserMode = root["mode"]["UserMode"].asBool();
				g_EnableFlag.NtQueryProcessInformation = root["mode"]["NtQueryProcessInformation"].asBool();

				g_EnableFlag.DebugPortOffset = root["offset"]["DebugPortOffset"].asInt();
				g_EnableFlag.ProcessNameOffset = root["offset"]["ProcessNameOffset"].asInt();

			}
		}
	}
	return ;
}

BOOL CKdEnumDlg::EnumProcess()
{
	if (g_EnableFlag.UserMode)
	{
		AfxBeginThread(ThreadEnumProcess_Createsnap, &g_Global, THREAD_PRIORITY_NORMAL, 0, 0, 0);
		return	TRUE;
	}
	if (g_EnableFlag.DriverMode)
	{

		return	TRUE;
	}
	if (g_EnableFlag.NtQueryProcessInformation)
	{
		return	TRUE;
	}
}

VOID CKdEnumDlg::Insertm_list()
{
	while (!g_Global.DataLock.try_lock())
	{
		if (g_Global.ProcessData.empty())
			return;
		int i = 0;
	}
}

UINT CKdEnumDlg::ThreadEnumProcess_Createsnap(LPVOID Params)
{
	GLOBAL* g = (GLOBAL*)Params;
	g->EnumProcess_CreateSnapshot();
	return;
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
