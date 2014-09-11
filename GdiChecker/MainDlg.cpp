// MainDlg.cpp : implementation of the CMainDlg class
//
/////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "resource.h"

#include "aboutdlg.h"
#include "MainDlg.h"
#include "Util.h"
#include "CGDI.h"

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
	HICON hIcon = AtlLoadIconImage(IDR_MAINFRAME, LR_DEFAULTCOLOR, ::GetSystemMetrics(SM_CXICON), ::GetSystemMetrics(SM_CYICON));
	SetIcon(hIcon, TRUE);
	HICON hIconSmall = AtlLoadIconImage(IDR_MAINFRAME, LR_DEFAULTCOLOR, ::GetSystemMetrics(SM_CXSMICON), ::GetSystemMetrics(SM_CYSMICON));
	SetIcon(hIconSmall, FALSE);

	// register object for message filtering and idle updates
	CMessageLoop* pLoop = _Module.GetMessageLoop();
	ATLASSERT(pLoop != NULL);
	pLoop->AddMessageFilter(this);
	pLoop->AddIdleHandler(this);

	UIAddChildWindowContainer(m_hWnd);

  IniListCtrl();
  UpdateListCtrl();

	return TRUE;
}

LRESULT CMainDlg::OnDestroy(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
	// unregister message filtering and idle updates
	CMessageLoop* pLoop = _Module.GetMessageLoop();
	ATLASSERT(pLoop != NULL);
	pLoop->RemoveMessageFilter(this);
	pLoop->RemoveIdleHandler(this);

	return 0;
}

LRESULT CMainDlg::OnAppAbout(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	CAboutDlg dlg;
	dlg.DoModal();
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

void CMainDlg::IniListCtrl()
{
  m_process.Attach(GetDlgItem(IDC_LIST_PROCESS));

  LONG lStyle;
  lStyle= m_process.GetWindowLong(GWL_STYLE); //GetWindowLong(m_process.m_hWnd, GWL_STYLE); //获取当前窗口style
  lStyle &= ~LVS_TYPEMASK; //清除显示方式位
  lStyle |= LVS_REPORT; //设置style
  m_process.SetWindowLong(GWL_STYLE, lStyle); //设置style


  DWORD dwStyle = m_process.GetExtendedListViewStyle(); //获取当前扩展样式
  dwStyle |= LVS_EX_FULLROWSELECT; //选中某行使整行高亮（report风格时）
  dwStyle |= LVS_EX_GRIDLINES; //网格线（report风格时）
  m_process.SetExtendedListViewStyle(dwStyle); //设置扩展风格

  m_process.InsertColumn(0, L"process", LVCFMT_LEFT, 150);
  m_process.InsertColumn(1, L"pid", LVCFMT_LEFT, 50);
  m_process.InsertColumn(2, L"system gdi", LVCFMT_LEFT, 80);
  m_process.InsertColumn(3, _T("process gdi"), LVCFMT_LEFT, 80);
  m_process.InsertColumn(4, _T("dc"), LVCFMT_LEFT, 50);
  m_process.InsertColumn(5, _T("region"), LVCFMT_LEFT, 50);
  m_process.InsertColumn(6, _T("bitmap"), LVCFMT_LEFT, 50);
  m_process.InsertColumn(7, _T("pal"), LVCFMT_LEFT, 50);
  m_process.InsertColumn(8, _T("font"), LVCFMT_LEFT, 50);
  m_process.InsertColumn(9, _T("brush"), LVCFMT_LEFT, 50);
  m_process.InsertColumn(10, _T("pen"), LVCFMT_LEFT, 50);
  m_process.InsertColumn(11, _T("extPen"), LVCFMT_LEFT, 60);
  m_process.InsertColumn(12, _T("other"), LVCFMT_LEFT, 50);
}

void CMainDlg::UpdateListCtrl()
{
  vector<PROCESS_INFO> vPi = GetProcessInfo();
  m_process.DeleteAllItems();

  for (vector<PROCESS_INFO>::iterator iter = vPi.begin(); iter != vPi.end(); ++iter)
  {
    CGDI gdi;
    gdi.SetPID(iter->pid);
    gdi.GetProcessGDIInfo();
    vector<GDI_INFO> gi = gdi.GetGDI();

    if (0 == gdi.GetGDINumber())
    {
      continue;
    }

    int nRow = m_process.InsertItem(m_process.GetItemCount(), iter->name);
    m_process.SetItemText(nRow, 1, I2S(iter->pid));
    m_process.SetItemText(nRow, 2, I2S(gdi.GetGDINumber()));
    m_process.SetItemText(nRow, 3, I2S(gi.size()));
    m_process.SetItemText(nRow, 4, I2S(GetGDITypeCount(gi, OBJ_DC)));
    m_process.SetItemText(nRow, 5, I2S(GetGDITypeCount(gi, OBJ_REGION)));
    m_process.SetItemText(nRow, 6, I2S(GetGDITypeCount(gi, OBJ_BITMAP)));
    m_process.SetItemText(nRow, 7, I2S(GetGDITypeCount(gi, OBJ_PAL)));
    m_process.SetItemText(nRow, 8, I2S(GetGDITypeCount(gi, OBJ_FONT)));
    m_process.SetItemText(nRow, 9, I2S(GetGDITypeCount(gi, OBJ_BRUSH)));
    m_process.SetItemText(nRow, 10, I2S(GetGDITypeCount(gi, OBJ_PEN)));
    m_process.SetItemText(nRow, 11, I2S(GetGDITypeCount(gi, OBJ_EXTPEN)));
    m_process.SetItemText(nRow, 12, I2S(GetGDITypeCount(gi, 0)));
  }
}

LRESULT CMainDlg::OnRefresh(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
  UpdateListCtrl();
  return 0;
}

LRESULT CMainDlg::OnDetails(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
  CString temp_path;
  int nRow = m_process.GetSelectionMark();
  if (-1 != nRow)
  {
    CString windows_name;
    CString process_name;
    CString process_id;

    m_process.GetItemText(nRow, 0, process_name);
    m_process.GetItemText(nRow, 1, process_id);
    windows_name.Format(_T("%s_%s_gdi info"), process_name, process_id);
    HWND find_hwnd = ::FindWindow(NULL, windows_name);
    if (NULL == find_hwnd)
    {
      DWORD processID = m_process.GetItemData(nRow);
      CString sCmd;
      CString sDllInject;
      if (IsWow64ProcessEx(processID))
      {
        sCmd.Format(L"\"%s\\GdiInfo64.dll\" %d", temp_path, processID);
        CreateProcessWithCmd(temp_path + L"\\dllinject64.exe", sCmd);
      }
      else
      {
        sCmd.Format(L"\"%s\\GdiInfo.dll\" %d", temp_path, processID);
        CreateProcessWithCmd(temp_path + L"\\dllinject.exe", sCmd);
      }
    }
    else
    {
      // 唤醒窗口
      ::PostMessage(find_hwnd, WM_INJECT, (WPARAM)m_hWnd, 1);
    }
  }

  return 0;
}