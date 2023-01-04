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
	ULONG	SystemClass = WIN64;	//����ָʾ����ָ���λ�� 32/64
	ULONG	SizeOfEPROCESS = 0;		//EPROCESS�Ĵ�С

	ULONG	ActiveProcessLinksOffset = 0;	//��������ƫ��
	ULONG	UniqueProcessIdOffset = 0;	//����id
	ULONG	ProcessNameOffset = 0;		//������
	ULONG	DebugPortOffset = 0;		//debugport
	ULONG	CreateTimeOffset = 0;		//���̴���ʱ��
	ULONG	ExitTimeOffset = 0;		//�����˳�ʱ��
	ULONG	ActiveThreadsOffset = 0;	//�߳�����
	ULONG	InheritedProcessId = 0;		//������id
	ULONG	PriorityClass = 0;		//�������ȼ�

public	:
	void	copyOffset(OFFSET_STRUCT* pOffset);
}OFFSET_STRUCT,*POFFSET_STRUCT;

typedef	struct GLOBAL_STRUCT
{
	LIST_ENTRY	header;	//����������Ǹ�R3�����ݵ�����
	FAST_MUTEX	mutex;	//��������Ļ�����
	ULONG	headerSize = 0;
	PDRIVER_OBJECT	pDriver;
	PDEVICE_OBJECT	pDevice;
	OFFSET_STRUCT offset;
	PAGED_LOOKASIDE_LIST pLookAsideList = { 0 };		//�����б�
	ULONG	INITED = 0;		//Ϊ0����ζ��û�г�ʼ��,���ܽ��б���
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

//��������R0�õ��Ľṹ,����R3��ʱ��,�������ָ���õ�
struct PROCESS_INFORMATION_R0
{
	PROCESS_INFORMATION eP;
	LIST_ENTRY	m_list;
};

void	StartUpInit();

BOOLEAN	EnumProcess_Dispatch();
BOOLEAN	EnumProcess_ActiveList();

//��������
VOID	CopyOffsetToGLOBAL(OFFSET_STRUCT* pOffset);
//ˢ�´���R3������.���������Ȼ�����±���
ULONG	RefrashData();
//��ʼ����
ULONG	EnumProcess();
//�������
VOID	ClearData();
//R3��ȡ����ĳ���,R3׼�����ٸ��������Ĵ�С
ULONG	GetHeaderSize();

//�������ݺ���,R3�������Ļ�����,���
VOID	SetData(PVOID buff);