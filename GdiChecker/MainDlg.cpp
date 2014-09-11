// MainDlg.cpp : implementation of the CMainDlg class
//
/////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "resource.h"

#include "aboutdlg.h"
#include "MainDlg.h"

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