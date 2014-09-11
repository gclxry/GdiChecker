// InjectProxy.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include <Windows.h>

BOOL InjectProcess(LPCWSTR szDllName, DWORD dwPid)
{
  HANDLE hProcess = NULL; 
  HANDLE hRemoteThread = NULL; 
  void *pLibRemote = NULL; 
  DWORD dwExitCode = 0; 
  HMODULE hKernel32 = NULL;
  int ret;
  int nBuf;

  hKernel32 = GetModuleHandle(_T("Kernel32")); 
  if(NULL == hKernel32) 
  { 
    return FALSE; 
  } 

  hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, dwPid); 
  if(hProcess == NULL) 
  {
    return FALSE; 
  } 

  nBuf = (lstrlenW(szDllName) + 1) * sizeof(WCHAR);
  pLibRemote = VirtualAllocEx(hProcess, NULL, nBuf,MEM_COMMIT,PAGE_READWRITE); 
  ret = WriteProcessMemory(hProcess,pLibRemote,(void *) szDllName, nBuf,NULL); 
  if(ret == 0) 
  { 
    return FALSE; 
  } 

  hRemoteThread = CreateRemoteThread(hProcess,NULL, 0,(LPTHREAD_START_ROUTINE)GetProcAddress(hKernel32, "LoadLibraryW"),pLibRemote,0,NULL); 
  WaitForSingleObject(hRemoteThread, INFINITE); 
  GetExitCodeThread(hRemoteThread, &dwExitCode); 

  VirtualFreeEx(hProcess, pLibRemote, nBuf, MEM_RELEASE); 
  CloseHandle(hRemoteThread);
  CloseHandle(hProcess);

  return TRUE; 
}

bool EnableDebugPrivilege()   
{   
  HANDLE hToken;   
  LUID sedebugnameValue;   
  TOKEN_PRIVILEGES tkp;   
  if (!OpenProcessToken(GetCurrentProcess(), TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY, &hToken))
  {   
    return   FALSE;   
  }   
  if (!LookupPrivilegeValue(NULL, SE_DEBUG_NAME, &sedebugnameValue))  
  {   
    CloseHandle(hToken);   
    return false;   
  }   
  tkp.PrivilegeCount = 1;   
  tkp.Privileges[0].Luid = sedebugnameValue;   
  tkp.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;   
  if (!AdjustTokenPrivileges(hToken, FALSE, &tkp, sizeof(tkp), NULL, NULL)) 
  {   
    CloseHandle(hToken);   
    return false;   
  }   
  return true;   
}

#define WM_NOTIFY_INJECTER (WM_USER+8807)

void NotifyGdiChecker()
{
  HWND hWnd = FindWindow(L"pjj_class", L"GdiChecker");
  if (NULL != hWnd)
  {
    PostMessage(hWnd, WM_NOTIFY_INJECTER, 0, 0);
  }
}

int _tmain(int argc, _TCHAR* argv[])
{
  //if (3 == argc)
  {
    EnableDebugPrivilege();
    //if (InjectProcess(argv[1], _wtoi(argv[2])))
    if (InjectProcess(L"GdiInfo.dll", 8820))
    {
      _tprintf(_T("inject sucessed!"));
      NotifyGdiChecker();
      return 1;
    }
    else
    {
      DWORD dwError = GetLastError();
      _tprintf(_T("inject failed %d!"), dwError);
    }
  }
  //else
  //{
  //  printf("command line error,please input like this:\n\"dll path\" pid\n");
  //}
	return 0;
}

