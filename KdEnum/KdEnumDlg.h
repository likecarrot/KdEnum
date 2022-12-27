
// KdEnumDlg.h: 头文件
//

#pragma once


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

protected:
	//初始化资源用
	VOID	InitResource();
	//用来读入配置文件,如果是第一次使用会创建配置文件
	VOID	StartUp();


	BOOL	EnumProcess();
	//使用Create32Snapshot来完成 || DriverMode || NtQueryProcessInformation
	static UINT __stdcall ThreadEnumProcess_Createsnap(LPVOID Params);


	BOOL	TerminateProcess(ULONG pid);
	VOID	TerminateProcessShell(ULONG pid);

	VOID	Insertm_list();

	BOOL	InstallDrv();
	BOOL	StartDrv();
	BOOL	StopDrv();
	BOOL	UninstallDrv();
private:
	GLOBAL	g_Global;
	ENABLE_FLAG	g_EnableFlag;
public:
	afx_msg void OnNMRClickList1(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnMenuItemUpdata();
	afx_msg void OnMenuItemTerminate();
protected:
public:
	virtual BOOL OnWndMsg(UINT message, WPARAM wParam, LPARAM lParam, LRESULT* pResult);
	afx_msg void OnSettings();
	afx_msg void On32778();
	afx_msg void On32777();
};



