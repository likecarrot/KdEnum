#include <iostream>
#include    <Windows.h>

typedef	struct OFFSET_STRUCT
{
	ULONG	SystemClass = 0;	//用来指示基本指针的位数 32/64
	ULONG	SizeOfEPROCESS = 0;		//EPROCESS的大小

	ULONG	ActiveProcessLinksOffset = 0;	//进程链表偏移
	ULONG	UniqueProcessIdOffset = 0;	//进程id
	ULONG	ProcessNameOffset = 0;		//进程名
	ULONG	DebugPortOffset = 0;		//debugport
	ULONG	CreateTimeOffset = 0;		//进程创建时间
	ULONG	ExitTimeOffset = 0;		//进程退出时间
	ULONG	ActiveThreadsOffset = 0;	//线程数量
	ULONG	InheritedProcessId = 0;		//父进程id
	ULONG	PriorityClass = 0;		//进程优先级

}OFFSET_STRUCT, * POFFSET_STRUCT;

#define		IOCTL_SET_OFFSET	CTL_CODE(FILE_DEVICE_UNKNOWN,0x900,METHOD_IN_DIRECT,FILE_ANY_ACCESS)
#define		IOCTL_TEST_COMM		CTL_CODE(FILE_DEVICE_UNKNOWN,0x901,METHOD_IN_DIRECT,FILE_ANY_ACCESS)
#define		IOCTL_GET_HEADERSIZE	CTL_CODE(FILE_DEVICE_UNKNOWN,0x902,METHOD_IN_DIRECT,FILE_ANY_ACCESS)
#define DEVICE_SYMNAME  L"\\\\.\\KdEnum"
int main()
{
    HANDLE  hDevice = NULL;
    hDevice = CreateFile(DEVICE_SYMNAME, GENERIC_ALL, 0, NULL, OPEN_EXISTING, 0, NULL);
    if (hDevice==INVALID_HANDLE_VALUE)
    {
        printf("打开设备失败.\n");
    }

	OFFSET_STRUCT offset = { 0 };
	offset.ActiveProcessLinksOffset = 1;
	offset.SizeOfEPROCESS = 1100;

	DWORD	retlen = 0;
	bool ret=DeviceIoControl(hDevice, IOCTL_SET_OFFSET, &offset, sizeof(offset), NULL, 0, &retlen, 0);
	if (ret)
	{
		printf("发送数据1成功.\n");
	}
	else {
		printf("发送数据1失败.\n");
	}
	char	test[10] = { 0 };
	ret = DeviceIoControl(hDevice, IOCTL_TEST_COMM, NULL,0, test, 10, &retlen, 0);
	if (ret)
	{
		printf("发送数据2成功.\n");
		printf("recv-----%s --%d.\n", test, retlen);
	}
	else {
		printf("发送数据2失败.\n");
		printf("recv-----%s --%d.\n", test,retlen);
	}
	ret=ReadFile(hDevice, NULL, 0, 0, 0);
	if (ret)
	{
		printf("发送数据2成功.\n");
	}
	else {
		printf("发送数据2失败.\n");
	}
	CloseHandle(hDevice);
	system("pause");
	return	0;
}

