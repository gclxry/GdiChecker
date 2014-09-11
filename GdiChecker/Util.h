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