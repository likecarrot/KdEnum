
// KdEnumDlg.h: 头文件
//

#pragma once
#include "def.h"


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
	std::map<char*, int> index;
protected:
	//初始化资源用
	VOID	InitResource();
	//用来读入配置文件,如果是第一次使用会创建配置文件
	VOID	StartUp();

	//使用Create32Snapshot来完成 || DriverMode || NtQueryProcessInformation
	BOOL	EnumProcess();
	VOID	Insertm_list();
	static UINT __cdecl ThreadEnumProcess_Createsnap(LPVOID Params);
	VOID	TerminateProcessShell(ULONG pid);
private:
	GLOBAL	g_Global;
	ENABLE_FLAG	g_EnableFlag;
public:
	afx_msg void OnNMRClickList1(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnMenuItemUpdata();
	afx_msg void OnMenuItemTerminate();
};



