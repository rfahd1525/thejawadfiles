#include <ntddk.h>
#include <windef.h>
#include "../shared/shared.h"

// Driver globals
PDEVICE_OBJECT g_DeviceObject = NULL;
UNICODE_STRING g_DeviceName;
UNICODE_STRING g_SymbolicLink;

// Function prototypes
NTSTATUS DriverEntry(PDRIVER_OBJECT DriverObject, PUNICODE_STRING RegistryPath);
VOID DriverUnload(PDRIVER_OBJECT DriverObject);
NTSTATUS DeviceControl(PDEVICE_OBJECT DeviceObject, PIRP Irp);
NTSTATUS CreateClose(PDEVICE_OBJECT DeviceObject, PIRP Irp);

// Memory operation functions
NTSTATUS ReadProcessMemory(HANDLE ProcessId, PVOID Address, PVOID Buffer, SIZE_T Size);
NTSTATUS WriteProcessMemory(HANDLE ProcessId, PVOID Address, PVOID Buffer, SIZE_T Size);
NTSTATUS GetModuleBase(HANDLE ProcessId, WCHAR* ModuleName, PVOID* BaseAddress);
NTSTATUS ProtectProcess(HANDLE ProcessId, BOOLEAN Protect);

NTSTATUS DriverEntry(PDRIVER_OBJECT DriverObject, PUNICODE_STRING RegistryPath) {
    UNREFERENCED_PARAMETER(RegistryPath);

    NTSTATUS status;

    // Initialize device name and symbolic link
    RtlInitUnicodeString(&g_DeviceName, L"\\Device\\FiveMKernel");
    RtlInitUnicodeString(&g_SymbolicLink, L"\\DosDevices\\FiveMKernel");

    // Create device
    status = IoCreateDevice(
        DriverObject,
        0,
        &g_DeviceName,
        FILE_DEVICE_UNKNOWN,
        FILE_DEVICE_SECURE_OPEN,
        FALSE,
        &g_DeviceObject
    );

    if (!NT_SUCCESS(status)) {
        return status;
    }

    // Create symbolic link
    status = IoCreateSymbolicLink(&g_SymbolicLink, &g_DeviceName);
    if (!NT_SUCCESS(status)) {
        IoDeleteDevice(g_DeviceObject);
        return status;
    }

    // Set up driver functions
    DriverObject->DriverUnload = DriverUnload;
    DriverObject->MajorFunction[IRP_MJ_CREATE] = CreateClose;
    DriverObject->MajorFunction[IRP_MJ_CLOSE] = CreateClose;
    DriverObject->MajorFunction[IRP_MJ_DEVICE_CONTROL] = DeviceControl;

    // Set flags
    g_DeviceObject->Flags |= DO_BUFFERED_IO;
    g_DeviceObject->Flags &= ~DO_DEVICE_INITIALIZING;

    DbgPrint("[FiveMKernel] Driver loaded successfully\n");

    return STATUS_SUCCESS;
}

VOID DriverUnload(PDRIVER_OBJECT DriverObject) {
    UNREFERENCED_PARAMETER(DriverObject);

    IoDeleteSymbolicLink(&g_SymbolicLink);
    IoDeleteDevice(g_DeviceObject);

    DbgPrint("[FiveMKernel] Driver unloaded\n");
}

NTSTATUS CreateClose(PDEVICE_OBJECT DeviceObject, PIRP Irp) {
    UNREFERENCED_PARAMETER(DeviceObject);

    Irp->IoStatus.Status = STATUS_SUCCESS;
    Irp->IoStatus.Information = 0;
    IoCompleteRequest(Irp, IO_NO_INCREMENT);

    return STATUS_SUCCESS;
}

NTSTATUS DeviceControl(PDEVICE_OBJECT DeviceObject, PIRP Irp) {
    UNREFERENCED_PARAMETER(DeviceObject);

    PIO_STACK_LOCATION stack = IoGetCurrentIrpStackLocation(Irp);
    NTSTATUS status = STATUS_SUCCESS;
    ULONG bytesReturned = 0;

    PVOID buffer = Irp->AssociatedIrp.SystemBuffer;
    ULONG inputLength = stack->Parameters.DeviceIoControl.InputBufferLength;
    ULONG outputLength = stack->Parameters.DeviceIoControl.OutputBufferLength;
    ULONG controlCode = stack->Parameters.DeviceIoControl.IoControlCode;

    switch (controlCode) {
        case IOCTL_READ_MEMORY: {
            if (inputLength < sizeof(MEMORY_OPERATION)) {
                status = STATUS_BUFFER_TOO_SMALL;
                break;
            }

            PMEMORY_OPERATION memOp = (PMEMORY_OPERATION)buffer;
            status = ReadProcessMemory(
                (HANDLE)memOp->ProcessId,
                memOp->Address,
                memOp->Buffer,
                memOp->Size
            );
            bytesReturned = sizeof(MEMORY_OPERATION);
            break;
        }

        case IOCTL_WRITE_MEMORY: {
            if (inputLength < sizeof(MEMORY_OPERATION)) {
                status = STATUS_BUFFER_TOO_SMALL;
                break;
            }

            PMEMORY_OPERATION memOp = (PMEMORY_OPERATION)buffer;
            status = WriteProcessMemory(
                (HANDLE)memOp->ProcessId,
                memOp->Address,
                memOp->Buffer,
                memOp->Size
            );
            break;
        }

        case IOCTL_GET_MODULE_BASE: {
            if (inputLength < sizeof(MODULE_OPERATION)) {
                status = STATUS_BUFFER_TOO_SMALL;
                break;
            }

            PMODULE_OPERATION modOp = (PMODULE_OPERATION)buffer;
            status = GetModuleBase(
                (HANDLE)modOp->ProcessId,
                modOp->ModuleName,
                &modOp->BaseAddress
            );
            bytesReturned = sizeof(MODULE_OPERATION);
            break;
        }

        case IOCTL_PROTECT_PROCESS: {
            if (inputLength < sizeof(PROTECT_OPERATION)) {
                status = STATUS_BUFFER_TOO_SMALL;
                break;
            }

            PPROTECT_OPERATION protOp = (PPROTECT_OPERATION)buffer;
            status = ProtectProcess(
                (HANDLE)protOp->ProcessId,
                protOp->Protect
            );
            break;
        }

        default:
            status = STATUS_INVALID_DEVICE_REQUEST;
            break;
    }

    Irp->IoStatus.Status = status;
    Irp->IoStatus.Information = bytesReturned;
    IoCompleteRequest(Irp, IO_NO_INCREMENT);

    return status;
}

NTSTATUS ReadProcessMemory(HANDLE ProcessId, PVOID Address, PVOID Buffer, SIZE_T Size) {
    PEPROCESS process = NULL;
    NTSTATUS status = PsLookupProcessByProcessId(ProcessId, &process);

    if (!NT_SUCCESS(status)) {
        return status;
    }

    KAPC_STATE apcState;
    KeStackAttachProcess(process, &apcState);

    __try {
        // Probe and validate address
        ProbeForRead(Address, Size, 1);
        RtlCopyMemory(Buffer, Address, Size);
        status = STATUS_SUCCESS;
    }
    __except (EXCEPTION_EXECUTE_HANDLER) {
        status = GetExceptionCode();
    }

    KeUnstackDetachProcess(&apcState);
    ObDereferenceObject(process);

    return status;
}

NTSTATUS WriteProcessMemory(HANDLE ProcessId, PVOID Address, PVOID Buffer, SIZE_T Size) {
    PEPROCESS process = NULL;
    NTSTATUS status = PsLookupProcessByProcessId(ProcessId, &process);

    if (!NT_SUCCESS(status)) {
        return status;
    }

    KAPC_STATE apcState;
    KeStackAttachProcess(process, &apcState);

    __try {
        // Probe and validate address
        ProbeForWrite(Address, Size, 1);

        // Disable write protection if needed
        KIRQL irql = KeRaiseIrqlToDpcLevel();
        UINT64 cr0 = __readcr0();
        __writecr0(cr0 & ~0x10000);

        RtlCopyMemory(Address, Buffer, Size);

        // Re-enable write protection
        __writecr0(cr0);
        KeLowerIrql(irql);

        status = STATUS_SUCCESS;
    }
    __except (EXCEPTION_EXECUTE_HANDLER) {
        status = GetExceptionCode();
    }

    KeUnstackDetachProcess(&apcState);
    ObDereferenceObject(process);

    return status;
}

NTSTATUS GetModuleBase(HANDLE ProcessId, WCHAR* ModuleName, PVOID* BaseAddress) {
    PEPROCESS process = NULL;
    NTSTATUS status = PsLookupProcessByProcessId(ProcessId, &process);

    if (!NT_SUCCESS(status)) {
        return status;
    }

    // Get PEB address
    PPEB peb = PsGetProcessPeb(process);
    if (!peb) {
        ObDereferenceObject(process);
        return STATUS_UNSUCCESSFUL;
    }

    KAPC_STATE apcState;
    KeStackAttachProcess(process, &apcState);

    __try {
        // Access PEB_LDR_DATA
        PPEB_LDR_DATA ldr = peb->Ldr;
        if (!ldr) {
            status = STATUS_UNSUCCESSFUL;
            __leave;
        }

        // Iterate through loaded modules
        for (PLIST_ENTRY listEntry = ldr->InLoadOrderModuleList.Flink;
             listEntry != &ldr->InLoadOrderModuleList;
             listEntry = listEntry->Flink) {

            PLDR_DATA_TABLE_ENTRY entry = CONTAINING_RECORD(
                listEntry,
                LDR_DATA_TABLE_ENTRY,
                InLoadOrderLinks
            );

            if (entry->BaseDllName.Buffer) {
                if (_wcsicmp(entry->BaseDllName.Buffer, ModuleName) == 0) {
                    *BaseAddress = entry->DllBase;
                    status = STATUS_SUCCESS;
                    __leave;
                }
            }
        }

        status = STATUS_NOT_FOUND;
    }
    __except (EXCEPTION_EXECUTE_HANDLER) {
        status = GetExceptionCode();
    }

    KeUnstackDetachProcess(&apcState);
    ObDereferenceObject(process);

    return status;
}

NTSTATUS ProtectProcess(HANDLE ProcessId, BOOLEAN Protect) {
    PEPROCESS process = NULL;
    NTSTATUS status = PsLookupProcessByProcessId(ProcessId, &process);

    if (!NT_SUCCESS(status)) {
        return status;
    }

    // Strip debug privileges to prevent debugging
    if (Protect) {
        // Set protection level (requires Windows 8.1+)
        // This is a simplified version - full implementation would use
        // PS_PROTECTED_PROCESS structures
        UCHAR* protectionPtr = (UCHAR*)((ULONG_PTR)process + 0x6FA); // Offset for Windows 10
        *protectionPtr = Protect ? 0x51 : 0x00; // Protected process light
    }

    ObDereferenceObject(process);
    return STATUS_SUCCESS;
}
