// SettingsDlg.cpp: 实现文件
//

#include "pch.h"
#include "KdEnum.h"
#include "SettingsDlg.h"
#include "afxdialogex.h"
#include "KdEnumDlg.h"


// SettingsDlg 对话框
IMPLEMENT_DYNAMIC(SettingsDlg, CDialogEx)

SettingsDlg::SettingsDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_DIALOG1, pParent)
	, Enum_Type(1)
	, Terminat_Type(0)
{
	
}

SettingsDlg::~SettingsDlg()
{
}

void SettingsDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Radio(pDX, IDC_RADIO3, Enum_Type);
	DDX_Radio(pDX, IDC_RADIO4, Terminat_Type);
}


BEGIN_MESSAGE_MAP(SettingsDlg, CDialogEx)
	ON_BN_CLICKED(IDC_RADIO3, &SettingsDlg::OnBnClickedRadio3)
	ON_BN_CLICKED(IDC_RADIO1, &SettingsDlg::OnBnClickedRadio1)
	ON_BN_CLICKED(IDC_RADIO2, &SettingsDlg::OnBnClickedRadio2)
	ON_BN_CLICKED(IDC_RADIO4, &SettingsDlg::OnBnClickedRadio4)
	ON_BN_CLICKED(IDC_RADIO5, &SettingsDlg::OnBnClickedRadio5)
	ON_BN_CLICKED(IDC_RADIO6, &SettingsDlg::OnBnClickedRadio6)
	ON_BN_CLICKED(IDOK, &SettingsDlg::OnBnClickedOk)
END_MESSAGE_MAP()


// SettingsDlg 消息处理程序


void SettingsDlg::OnBnClickedRadio3()
{
	Enum_Type = Enum_TPYE::NtQueryInformationProcess;
	g_Enable.enum_mode= Enum_TPYE::NtQueryInformationProcess;
}


void SettingsDlg::OnBnClickedRadio1()
{
	Enum_Type = Enum_TPYE::UserMode;
	g_Enable.enum_mode = Enum_TPYE::UserMode;
}


void SettingsDlg::OnBnClickedRadio2()
{
	Enum_Type = Enum_TPYE::DriverMode;
	g_Enable.enum_mode = Enum_TPYE::DriverMode;
}


void SettingsDlg::OnBnClickedRadio4()
{
	Terminat_Type = Terminat_TYPE::Shell;
	g_Enable.terminate_mode = Terminat_TYPE::Shell;
}


void SettingsDlg::OnBnClickedRadio5()
{
	Terminat_Type = Terminat_TYPE::WinR3API;
	g_Enable.terminate_mode = Terminat_TYPE::WinR3API;
}


void SettingsDlg::OnBnClickedRadio6()
{
	Terminat_Type = Terminat_TYPE::WinR0API;
	g_Enable.terminate_mode = Terminat_TYPE::WinR0API;
}




void SettingsDlg::OnBnClickedOk()
{
	::SendMessage(*AfxGetMainWnd(),WM_MSG_UPDATEConfig, (WPARAM)&g_Enable, 0);
	// TODO: 在此添加控件通知处理程序代码
	CDialogEx::OnOK();
}


BOOL SettingsDlg::OnWndMsg(UINT message, WPARAM wParam, LPARAM lParam, LRESULT* pResult)
{
	// TODO: 在此添加专用代码和/或调用基类
	switch (message)
	{
	case	WM_MSG_Enable_ToSettingsDlg:
		//傻比玩意 收不到消息 不知道怎么回事 舍弃这种方案 改为 子dlg给主dlg发送g_enbale
		//g_Enable = (ENABLE_FLAG*)wParam;
		//Enum_Type = g_Enable->enum_mode;
		//Terminat_Type = g_Enable->terminate_mode;

		break;
	default:
		break;
	}
	return CDialogEx::OnWndMsg(message, wParam, lParam, pResult);
}


BOOL SettingsDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();
	
	return TRUE;
}
