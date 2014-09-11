#include "StdAfx.h"
#include "CGDI.h"

CGDI::CGDI(void)
{
	// count of GDI table cells
	m_tableCount = 16384;
	m_hProcess = NULL;
}

CGDI::~CGDI(void)
{
	if (NULL != m_hProcess)
	{
		CloseHandle(m_hProcess);
	}
}

void CGDI::SetPID(DWORD dwPID)
{
	m_PID = dwPID;
	GetProcessHandle();
}


void CGDI::GetProcessGDIInfo()
{
	if (GetSystemInfo())
	{
		if (m_bWow)
		{
			ReadGDIInfo<PROCESS_BASIC_INFORMATION_WOW64, _NtWow64ReadVirtualMemory64, GDICELL_WOW64>();
		}
		else
		{
			ReadGDIInfo<PROCESS_BASIC_INFORMATION, _NtReadVirtualMemory, GDICELL>();
		}
	}
}

BOOL CGDI::GetProcessHandle()
{
	// open the process
	m_hProcess = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, m_PID);
	m_err = 0;
	if (m_hProcess == NULL)
	{
		m_err = GetLastError();
		// printf OpenProcess failed
		return FALSE;
	}
	return TRUE;
}

BOOL CGDI::GetSystemInfo()
{
	// determine if 64 or 32-bit processor
	SYSTEM_INFO si;
	GetNativeSystemInfo(&si);

	// NOTE: as this is undocumented, it *may vary* depending on bitness (32/64) and on Windows version.
	// use WinDbg "dt ntdll!_PEB" command and search for GdiSharedHandleTable offset to find the truth out
	m_GdiSharedHandleTableOffset  = si.wProcessorArchitecture == PROCESSOR_ARCHITECTURE_AMD64 ? 0xF8 : 0x94;

	// determine if this process is running on WOW64
	if (0 == IsWow64Process(GetCurrentProcess(), &m_bWow))
	{
		m_err = GetLastError();
		// printf call IsWow64Process failed
		return FALSE;
	}
	return TRUE;
}

template<typename TPBI, typename TRVM, typename TGC>
BOOL CGDI::ReadGDIInfo()
{
	// read basic info to get PEB address, we only need the beginning of PEB
	DWORD pebSize = m_GdiSharedHandleTableOffset + 8;
	LPBYTE peb = (LPBYTE)malloc(pebSize);
	ZeroMemory(peb, pebSize);
	_NtQueryInformationProcess query;
	TRVM read;

	// we're running as a 32-bit process in a 32-bit OS, or as a 64-bit process in a 64-bit OS
	TPBI pbi;
	ZeroMemory(&pbi, sizeof(pbi));

	if (m_bWow)
	{
		// get process information
		query = (_NtQueryInformationProcess)GetProcAddress(GetModuleHandleA("ntdll.dll"), "NtWow64QueryInformationProcess64");
		read = (TRVM)GetProcAddress(GetModuleHandleA("ntdll.dll"), "NtWow64ReadVirtualMemory64");
	}
	else
	{
		// get process information
		query = (_NtQueryInformationProcess)GetProcAddress(GetModuleHandleA("ntdll.dll"), "NtQueryInformationProcess");
		read = (TRVM)GetProcAddress(GetModuleHandleA("ntdll.dll"), "NtReadVirtualMemory");
	}
	
	m_err = query(m_hProcess, 0, &pbi, sizeof(pbi), NULL);
	if (m_err == 0)
	{
		// read PEB
		m_err = read(m_hProcess, pbi.PebBaseAddress, peb, pebSize, NULL);
		if (m_err == 0)
		{
			// get GDI table ptr
			TGC* gdiTable = (TGC*)*(LPVOID*)(peb + m_GdiSharedHandleTableOffset); // address in remote process adress space

			if (NULL != gdiTable)
			{
				DWORD tableSize = sizeof(TGC) * m_tableCount; // size of GDI table
				TGC* table = (TGC*)malloc(tableSize); // local table copied over to our address space

				// read GDI table
				m_err = read(m_hProcess, gdiTable, table, tableSize, NULL);
				if (m_err == 0)
				{
					for(DWORD i = 0; i < m_tableCount; i++)
					{
						TGC cell = table[i];
						if (cell.wProcessId != m_PID)
							continue;

						HGDIOBJ gdiHandle = (HGDIOBJ)((cell.wUpper << 16) + i);
						WORD type = cell.wType & 0x7F;
						GetHandleType(gdiHandle, type);
					}
				}
				if (NULL != table)
				{
					free(table);
				}
			}
		}
	}

	if (NULL != peb)
	{
		free(peb);
	}
	return TRUE;
}

void CGDI::GetHandleType(HGDIOBJ handle, WORD type)
{
	GDI_INFO gi;
	gi.handle = handle;
	switch(type)
	{
	case 1:
		gi.type = OBJ_DC;
		break;

	case 4:
		gi.type = OBJ_REGION;
		break;

	case 5:
		gi.type = OBJ_BITMAP;
		break;

	case 8:
		gi.type = OBJ_PAL;
		break;

	case 10:
		gi.type = OBJ_FONT;
		break;

	case 16:
		gi.type = OBJ_BRUSH;
		break;

	case 33:
		gi.type = OBJ_ENHMETAFILE;
		break;

	case 48:
		gi.type = OBJ_PEN;
		break;

	case 80:
		gi.type = OBJ_EXTPEN;
		break;
		
	// 占位
	case 81:
		gi.type = OBJ_METADC;
		break;

	case 82:
		gi.type = OBJ_METAFILE;
		break;

	case 83:
		gi.type = OBJ_MEMDC;


	case 84:
		gi.type = OBJ_ENHMETADC;

	case 85:
		gi.type = OBJ_COLORSPACE;
		break;


	default:
		gi.type = 0;
		break;
	}

	m_GDI.push_back(gi);
}

vector<GDI_INFO> CGDI::GetGDI()
{
	return m_GDI;
}


DWORD CGDI::GetGDINumber()
{
	DWORD dwGDIOBJECTS = GetGuiResources(m_hProcess, GR_GDIOBJECTS);
	DWORD dwUSEROBJECTS = GetGuiResources(m_hProcess, GR_USEROBJECTS);
	return dwGDIOBJECTS;
}

