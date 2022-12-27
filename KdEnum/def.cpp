#include	"def.h"


GLOBAL::GLOBAL()
{
}

GLOBAL::~GLOBAL()
{
}

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
	GLOBAL::DataLock.lock();
	MY_PROCESS_INFO	a;
	while (ret)
	{
		a.ThreadNums = PeEntry.cntThreads;
		a.Priority = PeEntry.pcPriClassBase;
		a.ProcessId = PeEntry.th32ProcessID;
		a.ParentId = PeEntry.th32ParentProcessID;
		wcscpy(a.ProcessName, PeEntry.szExeFile);
		wcscpy(a.DebugPort, L"NULL");
		//wsprintf(a.ThreadNums, L"%d", PeEntry.cntThreads);
		//wsprintf(a.Priority, L"%d", PeEntry.pcPriClassBase);
		//wsprintf(a.ProcessId, L"%d", PeEntry.th32ProcessID);
		//wsprintf(a.ParentId, L"%d", PeEntry.th32ParentProcessID);
		//wsprintf(a.ProcessName, L"%ws", PeEntry.szExeFile);
		GetModulePath(PeEntry.th32ProcessID, a.ProcessPath);
		GLOBAL::ProcessData.push_back(a);
		ret = Process32Next(hSnap, &PeEntry);
	}
	GLOBAL::DataLock.unlock();
	(*this).MainWnd->PostMessageW(WM_PAINT, 0, 0);

}



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



MY_PROCESS_INFO::MY_PROCESS_INFO()
{
}

MY_PROCESS_INFO::MY_PROCESS_INFO(const MY_PROCESS_INFO& s)
{
	wcscpy(this->ProcessName,s.ProcessName);
	this->ProcessId = s.ProcessId;
	this->ThreadNums = s.ThreadNums;
	this->Priority = s.Priority;
	this->ParentId = s.ParentId;
	wcscpy(this->DebugPort ,s.DebugPort);
	wcscpy(this->ProcessPath , s.ProcessPath);
}

MY_PROCESS_INFO::~MY_PROCESS_INFO()
{
}
