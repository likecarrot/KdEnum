
// KdEnumDlg.h: 头文件
//

#pragma once
#include "def.h"
#include	<map>

// CKdEnumDlg 对话框
class CKdEnumDlg : public CDialogEx
{
// 构造
public:
	CKdEnumDlg(CWnd* pParent = nullptr);	// 标准构造函数
// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_KDENUM_DIALOG };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 支持


// 实现
protected:
	HICON m_hIcon;

	// 生成的消息映射函数
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	// 用于显示
	CMenu m_Menu;
	CListCtrl m_list;
	//用来储存ProcessId / ProcessName 所对应的位置
	std::map<char*, int> MenuIndex;
protected:
	//初始化资源用
	VOID	InitResource();
	//用来读入配置文件,如果是第一次使用会创建配置文件
	VOID	StartUp();
	VOID	ReloadMenuIndex();
	//使用Create32Snapshot来完成 || DriverMode || NtQueryProcessInformation
	BOOL	EnumProcess();
	BOOL	GetModulePath(ULONG pid, WCHAR path[]);
	VOID	TerminateProcessShell(ULONG pid);
private:
	GLOBAL	g_Global;
	ENABLE_FLAG	g_EnableFlag;
public:
	afx_msg void OnNMRClickList1(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnMenuItemUpdata();
	afx_msg void OnMenuItemTerminate();
};

struct Process_Info
{
	DWORD	ProcessId;
	DWORD	cntThreads;		//进程启动的执行线程数。
	DWORD	th32ParentProcessID;//创建此过程的进程标识符 (其父进程) 。
	DWORD	pcPriClassBase;	//此过程创建的任何线程的基本优先级。
	WCHAR	szExeFile[MAX_PATH];
};

