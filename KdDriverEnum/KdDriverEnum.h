#pragma once
#include	<ntddk.h>

#define	DEBUG

#define	DEVICE_NAME	L"\\Device\\KdEnum"
#define	SYM_NAME	L"\\??\\KdEnum"

#define		IOCTL_SET_OFFSET	CTL_CODE(FILE_DEVICE_UNKNOWN,0x900,METHOD_IN_DIRECT,FILE_ANY_ACCESS)
#define		IOCTL_TEST_COMM		CTL_CODE(FILE_DEVICE_UNKNOWN,0x901,METHOD_IN_DIRECT,FILE_ANY_ACCESS)
#define		IOCTL_GET_HEADERSIZE	CTL_CODE(FILE_DEVICE_UNKNOWN,0x902,METHOD_IN_DIRECT,FILE_ANY_ACCESS)
#define		IOCTL_GET_DATA	CTL_CODE(FILE_DEVICE_UNKNOWN,0x903,METHOD_IN_DIRECT,FILE_ANY_ACCESS)
#define		IOCTL_REFRASHDATA	CTL_CODE(FILE_DEVICE_UNKNOWN,0x904,METHOD_IN_DIRECT,FILE_ANY_ACCESS)


#ifdef DEBUG
#define		KDPRINT_PRE	"KdEnum_TEST:"
#endif // DEBUG

NTSTATUS	DeviceControl(PDEVICE_OBJECT	pDevice, PIRP irp);
NTSTATUS	DeviceCreateClose(PDEVICE_OBJECT pDevice, PIRP irp);
NTSTATUS	DeviceReadWrite(PDEVICE_OBJECT pDevice, PIRP irp);
NTSTATUS	DevicePassThough(PDEVICE_OBJECT pDevice, PIRP irp);

VOID	Unload(PDRIVER_OBJECT pDriver);


#define	WIN64	1
#define	WIN32	0
typedef	struct OFFSET_STRUCT
{
	ULONG	SystemClass = WIN64;	//用来指示基本指针的位数 32/64
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

public	:
	void	copyOffset(OFFSET_STRUCT* pOffset);
}OFFSET_STRUCT,*POFFSET_STRUCT;

typedef	struct GLOBAL_STRUCT
{
	LIST_ENTRY	header;	//用来存放我们给R3的数据的链表
	FAST_MUTEX	mutex;	//操作链表的互斥体
	ULONG	headerSize = 0;
	PDRIVER_OBJECT	pDriver;
	PDEVICE_OBJECT	pDevice;
	OFFSET_STRUCT offset;
	PAGED_LOOKASIDE_LIST pLookAsideList = { 0 };		//旁视列表
	ULONG	INITED = 0;		//为0则意味着没有初始化,不能进行遍历
}GLOBAL_STRUCT;

class PROCESS_INFORMATION
{
public:
	PROCESS_INFORMATION();
	PROCESS_INFORMATION(PROCESS_INFORMATION& s);

	~PROCESS_INFORMATION();
	ULONG	UniqueProcessId;
	ULONG	InheritedFromUniqueProcessId;	//0x3e8
	UCHAR	PriorityClass;		//0x45f	
	UINT32	ActiveThreads;		//0x498
	PVOID	DebugPort;		//0x420
	UCHAR	ImageName[15];
private:

};

PROCESS_INFORMATION::PROCESS_INFORMATION()
{
	UniqueProcessId = 0;
	InheritedFromUniqueProcessId = 0;
	PriorityClass = 0;
	ActiveThreads = 0;
	DebugPort = 0;
	memset(this->ImageName,0,15);
}

PROCESS_INFORMATION::~PROCESS_INFORMATION()
{
}
PROCESS_INFORMATION::PROCESS_INFORMATION(PROCESS_INFORMATION& s) {
	this->UniqueProcessId = s.UniqueProcessId;
	this->InheritedFromUniqueProcessId = s.InheritedFromUniqueProcessId;
	this->PriorityClass = s.PriorityClass;
	this->ActiveThreads = s.ActiveThreads;
	this->DebugPort = s.DebugPort;
	memcpy(s.ImageName, this->ImageName, 15);
}

//这是我们R0用到的结构,传给R3的时候,会把链表指针拿掉
struct PROCESS_INFORMATION_R0
{
	PROCESS_INFORMATION eP;
	LIST_ENTRY	m_list;
};

void	StartUpInit();

BOOLEAN	EnumProcess_Dispatch();
BOOLEAN	EnumProcess_ActiveList();

//辅助函数
VOID	CopyOffsetToGLOBAL(OFFSET_STRUCT* pOffset);
//刷新传给R3的数据.把链表清空然后重新遍历
ULONG	RefrashData();
//开始遍历
ULONG	EnumProcess();
//清楚链表
VOID	ClearData();
//R3获取链表的长度,R3准备多少个缓冲区的大小
ULONG	GetHeaderSize();

//复制数据函数,R3传进来的缓冲区,填充
VOID	SetData(PVOID buff);