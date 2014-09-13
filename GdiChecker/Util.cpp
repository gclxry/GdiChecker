#include "StdAfx.h"
#include "Util.h"
#include <tlhelp32.h>

vector<PROCESS_INFO> GetProcessInfo()
{
  HANDLE hProcessSnap;
  PROCESSENTRY32 pe32;
  vector<PROCESS_INFO> vPi;

  // Take a snapshot of all processes in the system.
  hProcessSnap = CreateToolhelp32Snapshot( TH32CS_SNAPPROCESS, 0 );
  if( hProcessSnap == INVALID_HANDLE_VALUE )
  {
    return( vPi );
  }

  // Set the size of the structure before using it.
  pe32.dwSize = sizeof( PROCESSENTRY32 );

  // Retrieve information about the first process,
  // and exit if unsuccessful
  if( !Process32First( hProcessSnap, &pe32 ) )
  {
    CloseHandle( hProcessSnap );          // clean the snapshot object
    return( vPi );
  }

  do
  {

    PROCESS_INFO pi;
    pi.name = pe32.szExeFile;
    pi.pid = pe32.th32ProcessID;
    vPi.push_back(pi);

  } while( Process32Next( hProcessSnap, &pe32 ) );

  CloseHandle( hProcessSnap );
  return vPi;
}

CString I2S(int n)
{
  CString str;
  str.Format(_T("%d"), n);
  return str;
}

int GetGDITypeCount(vector<GDI_INFO>& gi, int nType)
{
  int nCount = 0;
  for (vector<GDI_INFO>::iterator iter = gi.begin(); iter != gi.end(); ++iter)
  {
    if (0 == nType)
    {
      if ( iter->type != OBJ_DC && iter->type != OBJ_REGION && iter->type != OBJ_BITMAP 
        && iter->type != OBJ_PAL && iter->type != OBJ_FONT && iter->type != OBJ_BRUSH 
        && iter->type != OBJ_PEN && iter->type != OBJ_EXTPEN)
      {
        nCount++;
      }
    }
    else if(iter->type == nType)
    {
      nCount++;
    }
  }
  return nCount;
}


BOOL IsWow64ProcessEx(int nPid)  
{  
  HANDLE hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, nPid);

  // 判断ntdll中的导出函数,可知是否是64位OS  
  HMODULE hMod=GetModuleHandle(_T("ntdll.dll"));  
  FARPROC x64fun=::GetProcAddress(hMod,"ZwWow64ReadVirtualMemory64");  
  if(!x64fun)
  {
    return FALSE; 
  } 
  // 利用IsWow64Process判断是否是x64进程
  typedef BOOL(WINAPI *pfnIsWow64Process)(HANDLE,PBOOL);  
  pfnIsWow64Process fnIsWow64Process=NULL;  
  hMod=GetModuleHandle(_T("kernel32.dll"));  
  fnIsWow64Process=(pfnIsWow64Process)GetProcAddress(hMod,"IsWow64Process");
  //如果没有导出则判定为32位
  if(!fnIsWow64Process)
  {
    return FALSE;
  }          
  BOOL bX64 = FALSE;  
  if(!fnIsWow64Process(hProcess,&bX64))
  {
    return FALSE;  
  }
  return !bX64;  
}

void CreateProcessWithCmd(CString sProcesName, CString sCmd)
{
  SHELLEXECUTEINFO ShExecInfo = {0};
  ShExecInfo.cbSize = sizeof(SHELLEXECUTEINFO);
  ShExecInfo.fMask = SEE_MASK_NOCLOSEPROCESS;
  ShExecInfo.hwnd = NULL;
  ShExecInfo.lpVerb = NULL;
  ShExecInfo.lpFile = sProcesName;
  ShExecInfo.lpParameters = sCmd;
  ShExecInfo.lpDirectory = NULL;
  ShExecInfo.nShow = SW_HIDE;
  ShExecInfo.hInstApp = NULL;
  ShellExecuteEx(&ShExecInfo);
}

void IniCurrentDirectory()
{

  CString temp_path = GetTempPath();
  CreatePath(temp_path);
  SetCurrentDirectory(temp_path);
}

CString GetTempPath()
{
  TCHAR szPath[MAX_PATH];
  CString temp_path;
  GetTempPath(MAX_PATH, szPath);
  temp_path= szPath;
  temp_path += TEXT("PJJ_GdiChecker");
  return temp_path;
}

BOOL CreatePath(LPCTSTR pszPath)
{
  CString path(pszPath);
  PathRemoveBackslash(path.GetBuffer(path.GetLength()));
  path.ReleaseBuffer();

  if (!PathFileExists(path))
  {
    if (!CreateDirectory(path, NULL))
    {
      return FALSE;
    }
  }
  return TRUE;
}


BOOL UseCustomResource(LPCTSTR fileName, int nResourceName)
{
  //定位我们的自定义资源，这里因为我们是从本模块定位资源，所以将句柄简单地置为NULL即可
  HRSRC hRsrc = FindResource(NULL, MAKEINTRESOURCE(nResourceName), TEXT("BIN"));
  if (NULL == hRsrc)
    return FALSE;
  //获取资源的大小
  DWORD dwSize = SizeofResource(NULL, hRsrc);
  if (0 == dwSize)
    return FALSE;
  //加载资源
  HGLOBAL hGlobal = LoadResource(NULL, hRsrc);
  if (NULL == hGlobal)
    return FALSE;
  //锁定资源
  LPVOID pBuffer = LockResource(hGlobal);
  if (NULL == pBuffer)
    return FALSE;

  BOOL bRt = FALSE;

  FILE* fp;
  _tfopen_s(&fp, fileName, _T("wb"));

  if (fp != NULL)
  {
    if (dwSize == fwrite(pBuffer, sizeof(char), dwSize, fp))
      bRt = TRUE;
    fclose(fp);
  }

  FreeResource(hGlobal);
  return bRt;
}