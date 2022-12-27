#pragma once
#include	"pch.h"
#include	<Windows.h>
#include	<iostream>
#include	<vector>
#include	<mutex>
#include <TlHelp32.h>

typedef	struct _Enable_Flag_
{
	BOOL	Enable_ProcessName=TRUE;
	BOOL	Enable_ProcessId = TRUE;
	BOOL	Enable_ParentId = TRUE;
	BOOL	Enable_Priority = TRUE;
	BOOL	Enable_ThreadNums = TRUE;
	BOOL	Enable_ProcessPath = TRUE;
	BOOL	Enable_Debugport = TRUE;

	BOOL	UserMode = TRUE;
	BOOL	DriverMode=FALSE;
	BOOL	NtQueryProcessInformation=FALSE;

	INT	DebugPortOffset;
	INT	ProcessNameOffset;
}ENABLE_FLAG;

class GLOBAL
{
public:
	GLOBAL();
	~GLOBAL();
	WCHAR	AppdataPath[MAX_PATH];
	WCHAR	ConfigFilePath[MAX_PATH];
	WCHAR	ConfigPath[MAX_PATH];
	char	DefaultConfig[790] = {
		"{\n    \"settings\":{\n        \"Enable_ProcessName\":1,\n        \"Enable_ProcessId\":1,\n        \"Enable_ParentId\":1,\n        \"Enable_Priority\":1,\n        \"Enable_ThreadNums\":1,\n        \"Enable_ProcessPath\":1,\n        \"Enable_Debugport\":1\n    },\n    \"mode\":{\n        \"UserMode\":1,\n        \"DriverMode\":0,\n        \"NtQueryProcessInformation\":0\n    },\n    \"offset\":{\n        \"DebugPortOffset\":0,\n        \"ProcessNameOffset\":0\n    }\n}"
	};
	char	UserConfig[790];

	std::vector<MY_PROCESS_INFO*> ProcessData;
	std::mutex	DataLock;

	void	EnumProcess_CreateSnapshot();
private:

};

GLOBAL::GLOBAL()
{
}

GLOBAL::~GLOBAL()
{
}
BOOL GetModulePath(ULONG pid, WCHAR path[]);
void GLOBAL::EnumProcess_CreateSnapshot()
{
	PROCESSENTRY32	PeEntry = { 0 };
	HANDLE	hSnap = NULL;
	PeEntry.dwSize = sizeof(PROCESSENTRY32);
	hSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
	if (hSnap == INVALID_HANDLE_VALUE)
	{
		return;
	}
	BOOL ret = Process32First(hSnap, &PeEntry);
	ULONG	index = 0;
	MY_PROCESS_INFO	*a=(MY_PROCESS_INFO*)malloc(sizeof(MY_PROCESS_INFO));
	GLOBAL::DataLock.lock();
	while (ret)
	{
		wsprintf((*a).ThreadNums, L"%d", PeEntry.cntThreads);
		wsprintf((*a).Priority, L"%d", PeEntry.pcPriClassBase);
		wsprintf((*a).ProcessId, L"%d", PeEntry.th32ProcessID);
		wsprintf((*a).ParentId, L"%d", PeEntry.th32ParentProcessID);
		wsprintf((*a).ProcessName, L"%ws", PeEntry.szExeFile);
		GetModulePath(PeEntry.th32ProcessID, (*a).ProcessPath);
		GLOBAL::ProcessData.push_back(&a);
		ret = Process32Next(hSnap, &PeEntry);
		a = (MY_PROCESS_INFO*)malloc(sizeof(MY_PROCESS_INFO));
	}
	GLOBAL::DataLock.unlock();
}


typedef	struct MY_PROCESS_INFO
{
	WCHAR	ProcessName[MAX_PATH];
	WCHAR	ProcessId[4];
	WCHAR	ThreadNums[4];
	WCHAR	Priority[4];
	WCHAR	ParentId[4];
	WCHAR	DebugPort[4];
	WCHAR	ProcessPath[MAX_PATH];
}MY_PROCESS_INFO,*PMY_PROCESS_INFO;




BOOL GetModulePath(ULONG pid, WCHAR path[])
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
