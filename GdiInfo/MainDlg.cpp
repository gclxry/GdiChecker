// MainDlg.cpp : implementation of the CMainDlg class
//
/////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "resource.h"

#include "MainDlg.h"
#include "Util.h"
#include "CGDI.h"

//CAppModule _Module;

BOOL CMainDlg::PreTranslateMessage(MSG* pMsg)
{
	return CWindow::IsDialogMessage(pMsg);
}

BOOL CMainDlg::OnIdle()
{
	UIUpdateChildWindows();
	return FALSE;
}

LRESULT CMainDlg::OnInitDialog(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
	// center the dialog on the screen
	CenterWindow();

	// set icons
	//HICON hIcon = AtlLoadIconImage(IDR_MAINFRAME, LR_DEFAULTCOLOR, ::GetSystemMetrics(SM_CXICON), ::GetSystemMetrics(SM_CYICON));
	//SetIcon(hIcon, TRUE);
	//HICON hIconSmall = AtlLoadIconImage(IDR_MAINFRAME, LR_DEFAULTCOLOR, ::GetSystemMetrics(SM_CXSMICON), ::GetSystemMetrics(SM_CYSMICON));
	//SetIcon(hIconSmall, FALSE);

	// register object for message filtering and idle updates
	//CMessageLoop* pLoop = _Module.GetMessageLoop();
	//ATLASSERT(pLoop != NULL);
	//pLoop->AddMessageFilter(this);
	//pLoop->AddIdleHandler(this);

	UIAddChildWindowContainer(m_hWnd);

  IniWindowsText();
  IniList();

	return TRUE;
}

LRESULT CMainDlg::OnDestroy(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
	// unregister message filtering and idle updates
	//CMessageLoop* pLoop = _Module.GetMessageLoop();
	//ATLASSERT(pLoop != NULL);
	//pLoop->RemoveMessageFilter(this);
	//pLoop->RemoveIdleHandler(this);

	return 0;
}

LRESULT CMainDlg::OnOK(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	// TODO: Add validation code 
	CloseDialog(wID);
	return 0;
}

LRESULT CMainDlg::OnCancel(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	CloseDialog(wID);
	return 0;
}

void CMainDlg::CloseDialog(int nVal)
{
	DestroyWindow();
	::PostQuitMessage(nVal);
}

void CMainDlg::IniWindowsText()
{
  CString str;
  TCHAR moudleName[MAX_PATH];
  GetModuleFileName(NULL, moudleName, MAX_PATH);
  str.Format(_T("%s_%d_gdi info"), PathFindFileName(moudleName), GetCurrentProcessId());
  SetWindowText(str);
}

void CMainDlg::UpdateList()
{
  CGDI gdi;
  gdi.SetPID(GetCurrentProcessId());
  gdi.GetProcessGDIInfo();
  vector<GDI_INFO> gi = gdi.GetGDI();
  m_list.ResetContent();
  for (vector<GDI_INFO>::iterator iter = gi.begin(); iter != gi.end(); ++iter)
  {
    CString itemString;
    itemString.Format(_T("%0x  %s"), iter->handle, GetGdiType(iter->handle));
    int item = m_list.AddString(itemString);
    m_list.SetItemData(item, (DWORD_PTR)iter->handle);
  }
}

void CMainDlg::IniList()
{
  m_list.Attach(GetDlgItem(IDC_LIST2));
  UpdateList();
}