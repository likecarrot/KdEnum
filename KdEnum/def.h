#pragma once
#include	"pch.h"
#include	<Windows.h>
#include	<iostream>
#include	<vector>
#include	<mutex>
#include <TlHelp32.h>


#define	WM_MSG_Enable_ToSettingsDlg	WM_USER+100
#define	WM_MSG_UPDATEConfig	WM_USER+101


BOOL GetModulePath(ULONG pid, WCHAR path[]);
struct MY_PROCESS_INFO
{
public:
	MY_PROCESS_INFO();
	MY_PROCESS_INFO(const MY_PROCESS_INFO& s);
	~MY_PROCESS_INFO();
	WCHAR	ProcessName[MAX_PATH];
	DWORD	ProcessId;
	DWORD	ThreadNums;
	DWORD	Priority;
	DWORD	ParentId;
	WCHAR	DebugPort[MAX_PATH];
	WCHAR	ProcessPath[MAX_PATH];
private:

};
struct ENABLE_FLAG
{
	int	enum_mode;
	int	terminate_mode;

	INT	DebugPortOffset;
	INT	ProcessNameOffset;
public:
};

class GLOBAL
{
public:
	GLOBAL();
	~GLOBAL();
	WCHAR	DriverPath[MAX_PATH] = L"0";
	WCHAR	DriverName[MAX_PATH] = L"0";
	WCHAR	AppdataPath[MAX_PATH];
	WCHAR	ConfigFilePath[MAX_PATH];
	WCHAR	ConfigPath[MAX_PATH];
	char	DefaultConfig[790] = {
		"\n{\n    \"mode\":0,\n    \"terminateMode\":0,\n    \"offset\":{\n        \"DebugPortOffset\":0,\n        \"ProcessNameOffset\":0\n    }\n}\n/*\nmode_flag_0:UserMode\nmode_flag_1:NtQueryInformationMode\nmode_flag_2:DriverMode\n\nterminateMode_flag_0:Shell\nterminateMode_flag_1:WinR3Api\nterminateMode_flag_2:WinR0Api\n*/"
	};
	char	UserConfig[790];

	std::vector<MY_PROCESS_INFO> ProcessData;
	std::mutex	DataLock;
	CWnd* MainWnd;
	void	EnumProcess_CreateSnapshot();
private:

};


enum Enum_TPYE
{
	UserMode,
	NtQueryInformationProcess,
	DriverMode
};
enum Terminat_TYPE
{
	Shell,
	WinR3API,
	WinR0API
};