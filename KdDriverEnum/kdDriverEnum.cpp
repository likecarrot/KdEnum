#include	"KdDriverEnum.h"


GLOBAL_STRUCT g_global = { 0 };

EXTERN_C	NTSTATUS	
DriverEntry(PDRIVER_OBJECT pDriver, PUNICODE_STRING pRegPath) {
	NTSTATUS	status = 0;
	g_global.pDriver = pDriver;
	DEVICE_OBJECT* pDevice = NULL;

	//创建设备
	UNICODE_STRING	device_name,device_sym;
	RtlInitUnicodeString(&device_name, DEVICE_NAME);
	status = IoCreateDevice(pDriver, NULL, &device_name, FILE_DEVICE_UNKNOWN, 0, TRUE, &g_global.pDevice);
	if (!NT_SUCCESS(status))
	{
		KdPrint((KDPRINT_PRE"创建设备失败.\n"));
		return	status;
	}
	else {
		KdPrint((KDPRINT_PRE"创建设备成功.\n"));
		g_global.pDevice->Flags |= DO_DIRECT_IO;
	}
	//创建符号链接
	RtlInitUnicodeString(&device_sym, SYM_NAME);
	status = IoCreateSymbolicLink(&device_sym, &device_name);
	if (!NT_SUCCESS(status))
	{
		KdPrint((KDPRINT_PRE"创建链接名失败.\n"));
	}
	else
	{
		KdPrint((KDPRINT_PRE"创建链接名成功.\n"));
	}

	pDriver->DriverUnload = Unload;
	for (size_t i = 0; i < IRP_MJ_MAXIMUM_FUNCTION; i++)
	{
		pDriver->MajorFunction[i] = DevicePassThough;
	}
	pDriver->MajorFunction[IRP_MJ_CREATE] =
		pDriver->MajorFunction[IRP_MJ_CLOSE] = DeviceCreateClose;

	pDriver->MajorFunction[IRP_MJ_READ] =
		pDriver->MajorFunction[IRP_MJ_WRITE] = DeviceReadWrite;

	pDriver->MajorFunction[IRP_MJ_DEVICE_CONTROL] = DeviceControl;

	UNREFERENCED_PARAMETER(pRegPath);



	return	status;
}

VOID	Unload(PDRIVER_OBJECT pDriver) {
	if (pDriver->DeviceObject != NULL)
	{
		IoDeleteDevice(pDriver->DeviceObject);
		KdPrint((KDPRINT_PRE"删除设备成功.\n"));
	}
	UNICODE_STRING	Sym;
	RtlInitUnicodeString(&Sym, SYM_NAME);
	IoDeleteSymbolicLink(&Sym);
	UNREFERENCED_PARAMETER(pDriver);
	KdPrint((KDPRINT_PRE"驱动卸载.\n"));
}



NTSTATUS	DeviceControl(PDEVICE_OBJECT	pDevice, PIRP irp) {
	KdPrint((KDPRINT_PRE"DeviceControl.\n"));
	NTSTATUS	status;
	PVOID	buf = NULL;
	ULONG	len = 0;
	OFFSET_STRUCT* pOffset;
	PIO_STACK_LOCATION	irpsp = IoGetCurrentIrpStackLocation(irp);
	switch (irpsp->Parameters.DeviceIoControl.IoControlCode)
	{
	case	IOCTL_SET_OFFSET:
		len = irpsp->Parameters.DeviceIoControl.InputBufferLength;
		if (irp->AssociatedIrp.SystemBuffer!=NULL&&len>=sizeof(OFFSET_STRUCT))
		{
			buf = irp->AssociatedIrp.SystemBuffer;
			pOffset = (OFFSET_STRUCT * )buf;
			g_global.offset.copyOffset(pOffset);
			KdPrint((KDPRINT_PRE"DeviceControl--- g_global.offset.test--%d.\n",g_global.offset.SizeOfEPROCESS));
			status = STATUS_SUCCESS;
		}
		break;
	case	IOCTL_TEST_COMM:
		len = irpsp->Parameters.DeviceIoControl.OutputBufferLength;
		buf = MmGetSystemAddressForMdlSafe(irp->MdlAddress, NormalPagePriority);
		if (buf && len)
		{
			memcpy(buf, "sssaaabbb", 9);
			len = 9;
			KdPrint((KDPRINT_PRE"收到用户层TEST请求.\n"));
			status = STATUS_SUCCESS;
		}
		break;
	case	IOCTL_GET_HEADERSIZE:
		len = irpsp->Parameters.DeviceIoControl.OutputBufferLength;
		buf = MmGetSystemAddressForMdlSafe(irp->MdlAddress, NormalPagePriority);
		if (len==sizeof(ULONG64)&&buf)
		{
			*((ULONG64*)buf) = GetHeaderSize();
			len= *((ULONG64*)buf);
			status = STATUS_SUCCESS;
		}
		else
		{
			status = STATUS_BUFFER_OVERFLOW;
			len = 0;
		}
		break;
	case IOCTL_GET_DATA:
		len = irpsp->Parameters.DeviceIoControl.OutputBufferLength;
		buf = MmGetSystemAddressForMdlSafe(irp->MdlAddress, NormalPagePriority);
		if (len>=sizeof(PROCESS_INFORMATION)*g_global.headerSize)
		{
			SetData(buf);
			len = g_global.headerSize;
			status = STATUS_SUCCESS;
		}
		else
		{
			status = STATUS_BUFFER_TOO_SMALL;
			len = 0;
		}
		break;
	default:
		status = STATUS_INVALID_DEVICE_REQUEST;
		len = 0;
		break;
	}

	if (irp->AssociatedIrp.SystemBuffer == NULL)
	{
		KdPrint((KDPRINT_PRE"DeviceControl--- irp->AssociatedIrp.SystemBuffer == NULL .\n"));
	}
	if (!irp->MdlAddress)
	{
		KdPrint((KDPRINT_PRE"DeviceControl--- irp->MdlAddress ==NULL .\n"));
	}
	if (!irpsp->Parameters.DeviceIoControl.Type3InputBuffer)
	{
		KdPrint((KDPRINT_PRE"DeviceControl--- irpsp->Parameters.DeviceIoControl.Type3InputBuffer ==NULL .\n"));
	}
	KdPrint((KDPRINT_PRE"DeviceControl---len-%d.\n", len));
	irp->IoStatus.Status = status;
	irp->IoStatus.Information = len;
	IoCompleteRequest(irp, IO_NO_INCREMENT);
	return	status;
}
NTSTATUS	DeviceCreateClose(PDEVICE_OBJECT pDevice, PIRP irp) {
	PIO_STACK_LOCATION	irpsp = IoGetCurrentIrpStackLocation(irp);
	if (irpsp->MajorFunction == IRP_MJ_CREATE) {
		KdPrint((KDPRINT_PRE"Device-Create.\n"));
	}
	if (irpsp->MajorFunction == IRP_MJ_CLOSE)
	{
		KdPrint((KDPRINT_PRE"Device-Close.\n"));
	}
	irp->IoStatus.Status = STATUS_SUCCESS;
	irp->IoStatus.Information = 0;
	IoCompleteRequest(irp, IO_NO_INCREMENT);
	return	STATUS_SUCCESS;
}

NTSTATUS	DeviceReadWrite(PDEVICE_OBJECT pDevice, PIRP irp) {
	KdPrint((KDPRINT_PRE"DeviceReadWrite.\n"));
	PIO_STACK_LOCATION	irpsp = IoGetCurrentIrpStackLocation(irp);
	PVOID buffer = NULL;
	NTSTATUS	status;
	ULONG		infoLen = 0;
	switch (irpsp->MajorFunction)
	{
	case	IRP_MJ_READ:
		infoLen =RefrashData();
		status = STATUS_SUCCESS;
		break;
	default:
		break;
	}
	irp->IoStatus.Status = status;
	irp->IoStatus.Information = infoLen;
	IoCompleteRequest(irp, IO_NO_INCREMENT);
	return	STATUS_SUCCESS;
}


NTSTATUS	DevicePassThough(PDEVICE_OBJECT pDevice, PIRP irp) {
	KdPrint((KDPRINT_PRE"DevicePassThough,一律返回成功.\n"));
	//PIO_STACK_LOCATION	irpsp = IoGetCurrentIrpStackLocation(irp);
	irp->IoStatus.Status = STATUS_SUCCESS;
	irp->IoStatus.Information = 0;
	IoCompleteRequest(irp, IO_NO_INCREMENT);
	return	STATUS_SUCCESS;
}

void StartUpInit()
{
	InitializeListHead(&g_global.header);
	ExInitializeFastMutex(&g_global.mutex);
	ExInitializePagedLookasideList(&g_global.pLookAsideList, NULL, NULL, 0, sizeof(PROCESS_INFORMATION_R0), '1234', 0);
}

BOOLEAN	EnumProcess_Dispatch() {
	if (g_global.INITED)
	{
		return EnumProcess_ActiveList();
	}
	return	FALSE;
}
//刷新传给R3的数据.把链表清空然后重新遍历
ULONG	RefrashData() {
	if (g_global.headerSize==0)
	{

	}
	else {
		ClearData();
	}
	EnumProcess();
	return	g_global.headerSize;
}
//开始遍历
ULONG	EnumProcess() {
	EnumProcess_ActiveList();
	return	g_global.headerSize;
}


BOOLEAN	EnumProcess_ActiveList() {
	//拿到system的EPROCESS
	PEPROCESS	eprocess = IoGetCurrentProcess();
	LIST_ENTRY* head, * list;
	list=head = (LIST_ENTRY*)((ULONG64)eprocess + g_global.offset.ActiveProcessLinksOffset);
	//开始遍历
	ExAcquireFastMutex(&g_global.mutex);
	while (list!=head->Flink)
	{
		PROCESS_INFORMATION_R0 * i=(PROCESS_INFORMATION_R0*)ExAllocateFromPagedLookasideList(&g_global.pLookAsideList);
		memcpy(i->eP.ImageName, (UCHAR*)((ULONG64)eprocess + g_global.offset.ProcessNameOffset), 15);
		i->eP.ActiveThreads = *(ULONG64*) ((ULONG64)eprocess + g_global.offset.ActiveThreadsOffset);
		i->eP.DebugPort = (PVOID)*(ULONG64*)((ULONG64)eprocess + g_global.offset.DebugPortOffset);
		i->eP.InheritedFromUniqueProcessId =  * (ULONG64*)((ULONG64)eprocess + g_global.offset.InheritedProcessId);
		i->eP.PriorityClass = (UCHAR) * (UCHAR*)((ULONG64*)eprocess + g_global.offset.PriorityClass);
		i->eP.UniqueProcessId = (ULONG) * (ULONG64*)((ULONG64)eprocess + g_global.offset.UniqueProcessIdOffset);
		InsertTailList(&g_global.header, &i->m_list);
		list = list->Blink;
		eprocess = (PEPROCESS)(list - g_global.offset.ActiveProcessLinksOffset);
		g_global.headerSize++;
	}
	ExReleaseFastMutex(&g_global.mutex);
	return	TRUE;
}

ULONG	GetHeaderSize() {
	return	g_global.headerSize;
}

VOID SetData(PVOID buff)
{
	ExAcquireFastMutex(&g_global.mutex);
	for (size_t i = 0; i < g_global.headerSize; i++)
	{
		auto	c = RemoveHeadList(&g_global.header);
		PROCESS_INFORMATION_R0* e = CONTAINING_RECORD(c, PROCESS_INFORMATION_R0, m_list);
		memcpy((PVOID)((ULONG64)buff + (i * sizeof(PROCESS_INFORMATION))), &e->eP, sizeof(PROCESS_INFORMATION));
		ExFreeToPagedLookasideList(&g_global.pLookAsideList, e);
	}
	ExReleaseFastMutex(&g_global.mutex);
	g_global.headerSize = 0;
}
VOID	ClearData() {
	ExAcquireFastMutex(&g_global.mutex);
	for (size_t i = 0; i < g_global.headerSize; i++)
	{
		auto	c = RemoveHeadList(&g_global.header);
		PROCESS_INFORMATION_R0* e = CONTAINING_RECORD(c, PROCESS_INFORMATION_R0, m_list);
		ExFreeToPagedLookasideList(&g_global.pLookAsideList, e);
	}
	ExReleaseFastMutex(&g_global.mutex);
	g_global.headerSize = 0;
}
void OFFSET_STRUCT::copyOffset(OFFSET_STRUCT* pOffset)
{
	this->ActiveProcessLinksOffset = pOffset->ActiveProcessLinksOffset;
	this->ActiveThreadsOffset = pOffset->ActiveThreadsOffset;
	this->CreateTimeOffset = pOffset->CreateTimeOffset;
	this->DebugPortOffset = pOffset->DebugPortOffset;
	this->ExitTimeOffset = pOffset->ExitTimeOffset;
	this->InheritedProcessId = pOffset->InheritedProcessId;
	this->PriorityClass = pOffset->PriorityClass;
	this->ProcessNameOffset = pOffset->ProcessNameOffset;
	this->SizeOfEPROCESS = pOffset->SizeOfEPROCESS;
	this->SystemClass = pOffset->SystemClass;
	this->UniqueProcessIdOffset = pOffset->UniqueProcessIdOffset;
}
