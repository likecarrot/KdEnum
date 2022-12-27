
// KdEnumDlg.cpp: 实现文件
//

#include "pch.h"
#include "framework.h"
#include "KdEnum.h"
#include "KdEnumDlg.h"
#include "afxdialogex.h"
#include	"SettingsDlg.h"

#include	<Windows.h>
#include	<winsvc.h>
#include	<TlHelp32.h>
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
	ON_COMMAND(ID_Settings, &CKdEnumDlg::OnSettings)
	ON_COMMAND(ID_32778, &CKdEnumDlg::On32778)
	ON_COMMAND(ID_32777, &CKdEnumDlg::On32777)
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
	g_Global.MainWnd = this;
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
		Insertm_list();
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
	m_list.InsertColumn(0, L"进程名", 0, 130);


	m_list.InsertColumn(1, L"进程id", 0, 50);

	m_list.InsertColumn(2, L"父进程id", 0, 60);

	m_list.InsertColumn(3, L"优先级", 0, 50);

	m_list.InsertColumn(4, L"线程数量", 0, 60);

	m_list.InsertColumn(5, L"DebugPort", 0, 80);

	m_list.InsertColumn(6, L"文件路径", 0, 300);

	m_Menu.LoadMenuW(IDR_MENU2);
	SetMenu(&m_Menu);

}

VOID CKdEnumDlg::StartUp()
{
	if (SHGetFolderPath(NULL, CSIDL_APPDATA, NULL, 0, g_Global.AppdataPath) == S_OK) {
		HANDLE	hFile = NULL;
		wsprintf(g_Global.ConfigFilePath, L"%ws\\kdEnum\\config.json", g_Global.AppdataPath);
		wsprintf(g_Global.ConfigPath, L"%ws\\kdEnum", g_Global.AppdataPath);
		hFile = CreateFile(g_Global.ConfigFilePath, GENERIC_READ | GENERIC_WRITE,
			FILE_SHARE_DELETE | FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
		if (hFile == INVALID_HANDLE_VALUE)
		{//第一次打开程序
			CreateDirectory(g_Global.ConfigPath, NULL);
			hFile = CreateFile(g_Global.ConfigFilePath, GENERIC_READ | GENERIC_WRITE,
				FILE_SHARE_DELETE | FILE_SHARE_READ, NULL, CREATE_NEW, FILE_ATTRIBUTE_NORMAL, NULL);
			DWORD	ret = 0;
			WriteFile(hFile, g_Global.DefaultConfig, strlen(g_Global.DefaultConfig), &ret, 0);
		}
		CloseHandle(hFile);

		//加载config.json
		Json::Reader	reader;
		Json::Value	root;
		using namespace std;
		std::ifstream	file;
		file.open(g_Global.ConfigFilePath, ios::in | ios::out);
		if (file.is_open())
		{//文件流已经打开
			if (reader.parse(file, root, false)) {
				g_EnableFlag.enum_mode = root["mode"].asInt();
				g_EnableFlag.terminate_mode = root["terminateMode"].asInt();
				g_EnableFlag.DebugPortOffset = root["offset"]["DebugPortOffset"].asInt();
				g_EnableFlag.ProcessNameOffset = root["offset"]["ProcessNameOffset"].asInt();
			}
			file.close();
		}
	}

	return;
}

BOOL CKdEnumDlg::EnumProcess()
{
	if (g_EnableFlag.enum_mode == Enum_TPYE::UserMode)
	{
		AfxBeginThread(ThreadEnumProcess_Createsnap, &g_Global, THREAD_PRIORITY_NORMAL, 0, 0, 0);
		return	TRUE;
	}
	if (g_EnableFlag.enum_mode == Enum_TPYE::NtQueryInformationProcess)
	{
		return	TRUE;
	}
	if (g_EnableFlag.enum_mode == Enum_TPYE::DriverMode)
	{
		return	TRUE;
	}
}

VOID CKdEnumDlg::Insertm_list()
{
	g_Global.DataLock.lock();
	if (g_Global.ProcessData.empty()) {
		g_Global.DataLock.unlock();
		return;
	}
	int i = 0;
	std::vector<MY_PROCESS_INFO>::iterator it = g_Global.ProcessData.begin();
	std::vector<MY_PROCESS_INFO>::iterator bt = g_Global.ProcessData.end();
	WCHAR	Processid[MAX_PATH], ParentId[MAX_PATH], Priority[MAX_PATH], ThreadNums[MAX_PATH], DebugPort[MAX_PATH];
	int index = 0;
	while (it != bt) {
		wsprintf(Processid, L"%d", it->ProcessId);
		wsprintf(ParentId, L"%d", it->ParentId);
		wsprintf(Priority, L"%d", it->Priority);
		wsprintf(ThreadNums, L"%d", it->ThreadNums);

		m_list.InsertItem(index, it->ProcessName);
		m_list.SetItemText(index, 1, Processid);
		m_list.SetItemText(index, 2, ParentId);
		m_list.SetItemText(index, 3, Priority);
		m_list.SetItemText(index, 4, ThreadNums);
		m_list.SetItemText(index, 5, it->DebugPort);
		m_list.SetItemText(index, 6, it->ProcessPath);
		/*if (g_EnableFlag.Enable_ProcessId)
		{

		}
		if (g_EnableFlag.Enable_ParentId)
		{
		}
		if (g_EnableFlag.Enable_Priority)
		{
		}
		if (g_EnableFlag.Enable_ThreadNums)
		{
		}
		if (g_EnableFlag.Enable_Debugport)
		{
		}
		if (g_EnableFlag.Enable_ProcessPath)
		{
		}*/
		it++;
		index++;
	}
	g_Global.DataLock.unlock();
	g_Global.ProcessData.clear();
}

BOOL CKdEnumDlg::InstallDrv()
{
#ifdef DEBUG
	return	true;
#endif
	SC_HANDLE	scgManager = OpenSCManager(NULL, NULL, SC_MANAGER_ALL_ACCESS);
	if (scgManager == NULL)
	{
		CloseServiceHandle(scgManager);
		return	FALSE;
	}
	// 创建服务对象，添加至服务控制管理器数据库
	SC_HANDLE schService = CreateService(
		scgManager,               // 服务控件管理器数据库的句柄
		g_Global.DriverName,        // 要安装的服务的名称
		g_Global.DriverName,                // 用户界面程序用来标识服务的显示名称
		SERVICE_ALL_ACCESS,         // 对服务的访问权限：所有全权限
		SERVICE_KERNEL_DRIVER,      // 服务类型：驱动服务
		SERVICE_DEMAND_START,       // 服务启动选项：进程调用 StartService 时启动
		SERVICE_ERROR_IGNORE,       // 如果无法启动：忽略错误继续运行
		g_Global.DriverPath,        // 驱动文件绝对路径，如果包含空格需要多加双引号
		NULL,                       // 服务所属的负载订购组：服务不属于某个组
		NULL,                       // 接收订购组唯一标记值：不接收
		NULL,                       // 服务加载顺序数组：服务没有依赖项
		NULL,                       // 运行服务的账户名：使用 LocalSystem 账户
		NULL                        // LocalSystem 账户密码
	);
	if (schService == NULL) {
		CloseServiceHandle(schService);
		CloseServiceHandle(scgManager);
		return FALSE;
	}


	CloseServiceHandle(schService);
	CloseServiceHandle(scgManager);
	return TRUE;
	return 0;
}

BOOL CKdEnumDlg::StartDrv()
{
#ifdef DEBUG
	return	true;
#endif
	// 打开服务控制管理器数据库
	SC_HANDLE schSCManager = OpenSCManager(
		NULL,                   // 目标计算机的名称,NULL：连接本地计算机上的服务控制管理器
		NULL,                   // 服务控制管理器数据库的名称，NULL：打开 SERVICES_ACTIVE_DATABASE 数据库
		SC_MANAGER_ALL_ACCESS   // 所有权限
	);
	if (schSCManager == NULL) {
		CloseServiceHandle(schSCManager);
		return FALSE;
	}

	// 打开服务
	SC_HANDLE hs = OpenService(
		schSCManager,           // 服务控件管理器数据库的句柄
		g_Global.DriverName,            // 要打开的服务名
		SERVICE_ALL_ACCESS      // 服务访问权限：所有权限
	);
	if (hs == NULL) {
		CloseServiceHandle(hs);
		CloseServiceHandle(schSCManager);
		return FALSE;
	}
	if (StartService(hs, 0, 0) == 0) {
		CloseServiceHandle(hs);
		CloseServiceHandle(schSCManager);
		return FALSE;
	}


	CloseServiceHandle(hs);
	CloseServiceHandle(schSCManager);
	return TRUE;

}

BOOL CKdEnumDlg::StopDrv()
{
	// 打开服务控制管理器数据库
	SC_HANDLE schSCManager = OpenSCManager(
		NULL,                   // 目标计算机的名称,NULL：连接本地计算机上的服务控制管理器
		NULL,                   // 服务控制管理器数据库的名称，NULL：打开 SERVICES_ACTIVE_DATABASE 数据库
		SC_MANAGER_ALL_ACCESS   // 所有权限
	);
	if (schSCManager == NULL) {
		CloseServiceHandle(schSCManager);
		return FALSE;
	}

	// 打开服务
	SC_HANDLE hs = OpenService(
		schSCManager,           // 服务控件管理器数据库的句柄
		g_Global.DriverName,            // 要打开的服务名
		SERVICE_ALL_ACCESS      // 服务访问权限：所有权限
	);
	if (hs == NULL) {
		CloseServiceHandle(hs);
		CloseServiceHandle(schSCManager);
		return FALSE;
	}

	// 如果服务正在运行
	SERVICE_STATUS status;
	if (QueryServiceStatus(hs, &status) == 0) {
		CloseServiceHandle(hs);
		CloseServiceHandle(schSCManager);
		return FALSE;
	}
	if (status.dwCurrentState != SERVICE_STOPPED &&
		status.dwCurrentState != SERVICE_STOP_PENDING
		) {
		// 发送关闭服务请求
		if (ControlService(
			hs,                         // 服务句柄
			SERVICE_CONTROL_STOP,       // 控制码：通知服务应该停止
			&status                     // 接收最新的服务状态信息
		) == 0) {
			CloseServiceHandle(hs);
			CloseServiceHandle(schSCManager);
			return FALSE;
		}

		// 判断超时
		INT timeOut = 0;
		while (status.dwCurrentState != SERVICE_STOPPED) {
			timeOut++;
			QueryServiceStatus(hs, &status);
			Sleep(50);
		}
		if (timeOut > 80) {
			CloseServiceHandle(hs);
			CloseServiceHandle(schSCManager);
			return FALSE;
		}
	}

	CloseServiceHandle(hs);
	CloseServiceHandle(schSCManager);
	return TRUE;
}

BOOL CKdEnumDlg::UninstallDrv()
{
	// 打开服务控制管理器数据库
	SC_HANDLE schSCManager = OpenSCManager(
		NULL,                   // 目标计算机的名称,NULL：连接本地计算机上的服务控制管理器
		NULL,                   // 服务控制管理器数据库的名称，NULL：打开 SERVICES_ACTIVE_DATABASE 数据库
		SC_MANAGER_ALL_ACCESS   // 所有权限
	);
	if (schSCManager == NULL) {
		CloseServiceHandle(schSCManager);
		return FALSE;
	}

	// 打开服务
	SC_HANDLE hs = OpenService(
		schSCManager,           // 服务控件管理器数据库的句柄
		g_Global.DriverName,            // 要打开的服务名
		SERVICE_ALL_ACCESS      // 服务访问权限：所有权限
	);
	if (hs == NULL) {
		CloseServiceHandle(hs);
		CloseServiceHandle(schSCManager);
		return FALSE;
	}

	// 删除服务
	if (DeleteService(hs) == 0) {
		CloseServiceHandle(hs);
		CloseServiceHandle(schSCManager);
		return FALSE;
	}

	CloseServiceHandle(hs);
	CloseServiceHandle(schSCManager);
	return TRUE;
}

UINT CKdEnumDlg::ThreadEnumProcess_Createsnap(LPVOID Params)
{
	GLOBAL* g = (GLOBAL*)Params;
	g->EnumProcess_CreateSnapshot();
	return 0;
}



BOOL CKdEnumDlg::TerminateProcess(ULONG pid)
{
	if (g_EnableFlag.terminate_mode == Terminat_TYPE::Shell)
	{
		TerminateProcessShell(pid);
		return	TRUE;
	}
	if (g_EnableFlag.terminate_mode == Terminat_TYPE::WinR3API)
	{
		return	TRUE;
	}
	if (g_EnableFlag.terminate_mode == Terminat_TYPE::WinR0API)
	{
		return	TRUE;
	}
	return FALSE;
}

VOID CKdEnumDlg::TerminateProcessShell(ULONG pid)
{
	CHAR	cmd[256];
	sprintf_s(cmd, "taskkill /pid %d /F", pid);
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
	TerminateProcess(pid);
	m_list.DeleteAllItems();
	EnumProcess();
}


BOOL CKdEnumDlg::OnWndMsg(UINT message, WPARAM wParam, LPARAM lParam, LRESULT* pResult)
{
	Json::Reader	reader;
	Json::Value	root;
	using namespace std;
	std::ofstream	file;
	static string	config = g_Global.DefaultConfig;
	::ENABLE_FLAG* p;
	// TODO: 在此添加专用代码和/或调用基类
	switch (message)
	{
	case	WM_MSG_UPDATEConfig:
		p = (ENABLE_FLAG*)wParam;
		if (p->terminate_mode != g_EnableFlag.terminate_mode || p->enum_mode != g_EnableFlag.enum_mode)
		{
			file.open(g_Global.ConfigFilePath, ios::in | ios::out | ios::_Nocreate);
			if (reader.parse(config, root)) {
				root["mode"] = p->enum_mode;
				root["terminateMode"] = p->terminate_mode;
			}
			if (file.is_open())
			{//文件流已经打开
				file << root.toStyledString() << endl;
				file.close();
				AfxMessageBox(L"请重新启动程序", MB_OK, 0);
			}
		}
		break;
	default:
		break;
	}
	return CDialogEx::OnWndMsg(message, wParam, lParam, pResult);
}


void CKdEnumDlg::OnSettings()
{
	// TODO: 在此添加命令处理程序代码// TODO: 在此添加命令处理程序代码
	SettingsDlg	dlg;
	//dlg.SendMessage(WM_MSG_Enable_ToSettingsDlg, (WPARAM)&g_EnableFlag, 0);
	dlg.DoModal();
}


void CKdEnumDlg::On32777()
{
	// TODO: 在此添加命令处理程序代码
	//加载驱动
	if (InstallDrv())
		if (StartDrv()) {

		}
		else
		{
			AfxMessageBox(L"驱动启动失败", MB_OK, 0);
		}
	else {
		AfxMessageBox(L"驱动加载失败", MB_OK, 0);
	}
}

void CKdEnumDlg::On32778()
{
	// TODO: 在此添加命令处理程序代码
	//测试连接
}

