#pragma once
#include <vector>
using namespace std;


// defines a GDI CELL
typedef struct
{
	LPVOID pKernelAddress;
	USHORT wProcessId;
	USHORT wCount;
	USHORT wUpper;
	USHORT wType;
	LPVOID pUserAddress;
} GDICELL;

// defines a GDI CELL for WOW64
typedef struct
{
	PVOID64 pKernelAddress;
	USHORT wProcessId;
	USHORT wCount;
	USHORT wUpper;
	USHORT wType;
	PVOID64 pUserAddress;
} GDICELL_WOW64;

// NtQueryInformationProcess for pure 32 and 64-bit processes
typedef NTSTATUS (NTAPI *_NtQueryInformationProcess)(
	IN HANDLE ProcessHandle,
	ULONG ProcessInformationClass,
	OUT PVOID ProcessInformation,
	IN ULONG ProcessInformationLength,
	OUT PULONG ReturnLength OPTIONAL
	);

typedef NTSTATUS (NTAPI *_NtReadVirtualMemory)(
	IN HANDLE ProcessHandle,
	IN PVOID BaseAddress,
	OUT PVOID Buffer,
	IN SIZE_T Size,
	OUT PSIZE_T NumberOfBytesRead);

// NtQueryInformationProcess for 32-bit process on WOW64
typedef NTSTATUS (NTAPI *_NtWow64ReadVirtualMemory64)(
	IN HANDLE ProcessHandle,
	IN PVOID64 BaseAddress,
	OUT PVOID Buffer,
	IN ULONG64 Size,
	OUT PULONG64 NumberOfBytesRead);

// PROCESS_BASIC_INFORMATION for pure 32 and 64-bit processes
typedef struct _PROCESS_BASIC_INFORMATION {
	PVOID Reserved1;
	PVOID PebBaseAddress;
	PVOID Reserved2[2];
	ULONG_PTR UniqueProcessId;
	PVOID Reserved3;
} PROCESS_BASIC_INFORMATION;

// PROCESS_BASIC_INFORMATION for 32-bit process on WOW64
// The definition is quite funky, as we just lazily doubled sizes to match offsets...
typedef struct _PROCESS_BASIC_INFORMATION_WOW64 {
	PVOID Reserved1[2];
	PVOID64 PebBaseAddress;
	PVOID Reserved2[4];
	ULONG_PTR UniqueProcessId[2];
	PVOID Reserved3[2];
} PROCESS_BASIC_INFORMATION_WOW64;


struct GDI_INFO
{
	DWORD type;
	HANDLE handle;
};


class CGDI
{
public:
	CGDI(void);
	~CGDI(void);
	void SetPID(DWORD dwPID);
	void GetProcessGDIInfo();
	BOOL GetProcessHandle();
	BOOL GetSystemInfo();
	void GetHandleType(HGDIOBJ handle, WORD type);
	vector<GDI_INFO> GetGDI();
	DWORD GetGDINumber();

	template<typename TPBI, typename TRVM, typename TGC>
	BOOL ReadGDIInfo();

private:
	DWORD m_PID;
	HANDLE m_hProcess;
	DWORD m_err;
	DWORD m_GdiSharedHandleTableOffset;
	DWORD m_tableCount;
	vector<GDI_INFO> m_GDI;
	BOOL m_bWow;
};
