#include <windows.h>
#include <stdio.h>

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

int main()
{
    printf("Starting user-mode application...\n");

    // Open a handle to the shared memory
    printf("Opening handle to shared memory...\n");
    HANDLE hMapFile = OpenFileMapping(FILE_MAP_ALL_ACCESS, FALSE, L"Global\\MySharedMemory");
    if (hMapFile == NULL)
    {
        printf("Could not open file mapping object (%d).\n", GetLastError());
        return 1;
    }

    // Map the shared memory into our address space
    printf("Mapping shared memory into address space...\n");
    g_SharedMemory = (PSHARED_MEMORY)MapViewOfFile(hMapFile, FILE_MAP_ALL_ACCESS, 0, 0, sizeof(SHARED_MEMORY));
    if (g_SharedMemory == NULL)
    {
        printf("Could not map view of file (%d).\n", GetLastError());
        CloseHandle(hMapFile);
        return 1;
    }

    printf("Shared memory successfully mapped!\n");
    // Map the shared memory into our address space
    g_SharedMemory = (PSHARED_MEMORY)MapViewOfFile(hMapFile, FILE_MAP_ALL_ACCESS, 0, 0, sizeof(SHARED_MEMORY));
    if (g_SharedMemory == NULL)
    {
        printf("Could not map view of file (%d).\n", GetLastError());
        CloseHandle(hMapFile);
        return 1;
    }

    BYTE newInstructions1[] = { 0xC6, 0x43, 0xC6, 0x01 };
    BYTE newInstructions2[] = { 0xC7, 0x43, 0xCC, 0x00, 0x00, 0x80, 0x3F };

    // Prepare a request
    g_SharedMemory->SourceAddress = newInstructions1;
    g_SharedMemory->TargetAddress = (PVOID)(0x14000000 + 0x5536C25);
    g_SharedMemory->Size = sizeof(newInstructions1);
    g_SharedMemory->Write = TRUE;

    // Signal the driver that a request is ready
    g_SharedMemory->Ready = TRUE;

    // Wait for the driver to process the request
    while (g_SharedMemory->Ready)
        Sleep(1);

    // Prepare another request
    g_SharedMemory->SourceAddress = newInstructions2;
    g_SharedMemory->TargetAddress = (PVOID)(0x14000000 + 0x5536C7B);
    g_SharedMemory->Size = sizeof(newInstructions2);
    g_SharedMemory->Write = TRUE;

    // Signal the driver that a request is ready
    g_SharedMemory->Ready = TRUE;

    // Wait for the driver to process the request
    while (g_SharedMemory->Ready)
        Sleep(1);

    // Unmap and close the shared memory when done
    UnmapViewOfFile(g_SharedMemory);
    CloseHandle(hMapFile);

    return 0;
}