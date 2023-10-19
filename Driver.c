#include <ntddk.h>
#include <ntstrsafe.h>
#include <wdm.h>

// Define a custom structure for communication
typedef struct _SHARED_MEMORY {
    PVOID SourceAddress; // The source address to read or write
    PVOID TargetAddress; // The target address to read or write
    SIZE_T Size; // The size of the data to read or write
    BOOLEAN Write; // A flag indicating whether to read or write
    BOOLEAN Ready; // A flag indicating whether a request is ready
} SHARED_MEMORY, * PSHARED_MEMORY;

// Define a global pointer to the shared memory
PSHARED_MEMORY g_SharedMemory = NULL;

NTSTATUS DriverInitialize(PDRIVER_OBJECT DriverObject, PUNICODE_STRING RegistryPath)
{
    UNREFERENCED_PARAMETER(DriverObject);
    UNREFERENCED_PARAMETER(RegistryPath);

    DbgPrintEx(DPFLTR_IHVDRIVER_ID, DPFLTR_ERROR_LEVEL, "Hello from manual mapped kernel driver!\n");

    // Allocate shared memory
    g_SharedMemory = (PSHARED_MEMORY)ExAllocatePoolWithTag(NonPagedPool, sizeof(SHARED_MEMORY), 'memG');

    if (g_SharedMemory == NULL)
    {
        DbgPrintEx(DPFLTR_IHVDRIVER_ID, DPFLTR_ERROR_LEVEL, "Failed to allocate shared memory!\n");
        return STATUS_INSUFFICIENT_RESOURCES;
    }

    RtlZeroMemory(g_SharedMemory, sizeof(SHARED_MEMORY));

    // Return success status
    return STATUS_SUCCESS;
}

NTSTATUS DriverEntry(PDRIVER_OBJECT DriverObject, PUNICODE_STRING RegistryPath)
{
    NTSTATUS status;
    UNICODE_STRING drvName;

    /* This parameters are invalid due to nonstandard way of loading and should not be used. */
    UNREFERENCED_PARAMETER(DriverObject);
    UNREFERENCED_PARAMETER(RegistryPath);

    RtlInitUnicodeString(&drvName, L"Global\\MySharedMemory");

    status = DriverInitialize(DriverObject, RegistryPath);

    return status;
}