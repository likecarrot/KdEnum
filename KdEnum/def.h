#pragma once
#include	"pch.h"
#include	<Windows.h>



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

typedef	struct _GLOBAL_
{
	WCHAR	AppdataPath[MAX_PATH];
	WCHAR	ConfigFilePath[MAX_PATH];
	WCHAR	ConfigPath[MAX_PATH];
	char	DefaultConfig[790] = {
		"{\n    \"settings\":{\n        \"Enable_ProcessName\":1,\n        \"Enable_ProcessId\":1,\n        \"Enable_ParentId\":1,\n        \"Enable_Priority\":1,\n        \"Enable_ThreadNums\":1,\n        \"Enable_ProcessPath\":1,\n        \"Enable_Debugport\":1\n    },\n    \"mode\":{\n        \"UserMode\":1,\n        \"DriverMode\":0,\n        \"NtQueryProcessInformation\":0\n    },\n    \"offset\":{\n        \"DebugPortOffset\":0,\n        \"ProcessNameOffset\":0\n    }\n}"
	};
	char	UserConfig[790];
}GLOBAL;