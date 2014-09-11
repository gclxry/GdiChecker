#pragma once
#include <vector>
#include "CGDI.h"

using namespace std;

struct PROCESS_INFO
{
  DWORD pid;
  CString name;
};


vector<PROCESS_INFO> GetProcessInfo();
CString I2S(int n);
int GetGDITypeCount(vector<GDI_INFO>& gi, int nType);
BOOL IsWow64ProcessEx(int nPid);
void CreateProcessWithCmd(CString sProcesName, CString sCmd);
void IniCurrentDirectory();
BOOL CreatePath(LPCTSTR pszPath);
CString GetTempPath();