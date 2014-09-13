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
  IniPaintRect();

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

LRESULT CMainDlg::OnCancel(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
  CloseDialog(wID);
  return 0;
}

void CMainDlg::CloseDialog(int nVal)
{
	//DestroyWindow();
	//::PostQuitMessage(nVal);
  ShowWindow(SW_HIDE);
  SetMsgHandled(FALSE);
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

LRESULT CMainDlg::OnInject(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
  HWND source_hwnd = (HWND)wParam;
  if (NULL != source_hwnd)
  {
    // »½ÐÑ´°¿Ú
    if (1 == lParam)
    {
      ShowWindow(SW_SHOWNA);
      SetForegroundWindow(m_hWnd);
    }
    //::PostMessage(checkerHWND, WM_INJECT, (WPARAM)m_hWnd, 1);
  }
  SetMsgHandled(TRUE);
  return 0;
}

LRESULT CMainDlg::OnSelChange(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
  CString itemString;
  HGDIOBJ gdiHandle;

  int item = m_list.GetCurSel();
  m_list.GetText(item, itemString);
  gdiHandle = (HGDIOBJ)m_list.GetItemData(item);
  m_gdidHandle = gdiHandle;
  InvalidateRect(&m_PaintRect);
  UpdateWindow();
  ShowGdi(gdiHandle);
  SetMsgHandled(TRUE);
  return 0;
}

void CMainDlg::ShowGdi(HGDIOBJ gdi)
{
  DWORD gidType = GetObjectType(gdi);
  switch (gidType)
  {
  case OBJ_BITMAP:
    ShowOBJ_BITMAP(gdi);
    break;

  case OBJ_BRUSH:
    ShowOBJ_BRUSH(gdi);
    break;

  case OBJ_DC:
  case OBJ_MEMDC:
    ShowOBJ_DC(gdi);
    break;

  case OBJ_PEN:
  case OBJ_EXTPEN:
    ShowOBJ_PEN(gdi);
    break;

  case OBJ_REGION:
    ShowOBJ_REGION(gdi);
    break;

  case OBJ_FONT:
    ShowOBJ_FONT(gdi);
    break;

  default:
    break;
  }
}

void CMainDlg::ShowOBJ_BITMAP(HGDIOBJ gdi)
{
  
  CDC compatible_dc;
  CClientDC dc(m_hWnd);
  CBitmapHandle bitmap((HBITMAP)gdi);

  if (NULL != bitmap.m_hBitmap)
  {
    BITMAP bmp;
    ZeroMemory(&bmp, sizeof(bmp));
    bitmap.GetBitmap(&bmp);

    CString gdiInfo;
    gdiInfo.Format(_T("Width=%ld  Height=%ld  BitsPixel=%u"), bmp.bmWidth, bmp.bmHeight, bmp.bmBitsPixel);
    dc.TextOut(m_PaintRect.left, m_PaintRect.top + 5, gdiInfo, gdiInfo.GetLength());

    compatible_dc.CreateCompatibleDC(dc);
    CBitmap old_bitmap = compatible_dc.SelectBitmap(bitmap);

    dc.StretchBlt(m_PaintRect.left, m_PaintRect.top + 50, bmp.bmWidth>500?500:bmp.bmWidth, bmp.bmHeight>380?380:bmp.bmHeight, compatible_dc, 0, 0, bmp.bmWidth, bmp.bmHeight, SRCCOPY);
    compatible_dc.SelectBitmap(old_bitmap);
  }
}

void CMainDlg::ShowOBJ_BRUSH(HGDIOBJ gdi)
{
  CClientDC dc(m_hWnd);
  CBrushHandle brush((HBRUSH)gdi);

  if (NULL != brush.m_hBrush)
  {
    CRect rect;
    rect.left = m_PaintRect.left + 10;
    rect.top = m_PaintRect.top + 50;
    rect.right = rect.left + 100;
    rect.bottom = rect.top + 100;

    CBrush old_brush = dc.SelectBrush(brush);
    dc.FillRect(rect, brush);
    dc.SelectBrush(old_brush);
  }
}

void CMainDlg::ShowOBJ_DC(HGDIOBJ gdi)
{
  
  CClientDC dc(m_hWnd);
  CDCHandle compatible_dc((HDC)gdi);

  if (NULL != compatible_dc.m_hDC)
  {
    CString gdiInfo;
    dc.BitBlt(m_PaintRect.left, m_PaintRect.top, 500, 380, compatible_dc, 0, 0, SRCCOPY);
  }	
}


void CMainDlg::ShowOBJ_PEN(HGDIOBJ gdi)
{
  CClientDC dc(m_hWnd);
  CPenHandle pen((HPEN)gdi);

  if (NULL != pen.m_hPen)
  {
    CPen old_pen = dc.SelectPen(pen); 
    dc.MoveTo(m_PaintRect.left, m_PaintRect.top+50);  
    dc.LineTo(m_PaintRect.right - 30, m_PaintRect.top+50);
    dc.SelectPen(old_pen);
  }
}

void CMainDlg::ShowOBJ_REGION(HGDIOBJ gdi)
{
  CClientDC dc(m_hWnd);
  CRgnHandle rgn((HRGN)gdi);

  if (NULL != rgn.m_hRgn)
  {
    RECT rect;
    rgn.GetRgnBox(&rect);
    CString strInfo;
    strInfo.Format(_T("left=%d top=%d right=%d bottom=%d"),rect.left, rect.top, rect.right, rect.bottom);
    dc.TextOut(m_PaintRect.left, m_PaintRect.top + 5, strInfo, strInfo.GetLength());
  }
}

void CMainDlg::ShowOBJ_FONT(HGDIOBJ gdi)
{
  CClientDC dc(m_hWnd);
  CFontHandle font((HFONT)gdi);

  if (NULL != font.m_hFont)
  {
    LOGFONT logFont;
    font.GetLogFont(&logFont);
    CString strInfo;
    strInfo.Format(_T("%s"), logFont.lfFaceName);
   dc.TextOut(m_PaintRect.left, m_PaintRect.top + 5, strInfo, strInfo.GetLength());
  }
}

void CMainDlg::IniPaintRect()
{
  m_PaintRect.left = 250;
  m_PaintRect.top = 15;
  m_PaintRect.right = m_PaintRect.left + 500;
  m_PaintRect.bottom = m_PaintRect.top + 380;
}

void CMainDlg::OnPaint(CDCHandle dc)
{
  ShowGdi(m_gdidHandle);
  SetMsgHandled(FALSE);
}

LRESULT CMainDlg::OnRefresh(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
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
  return 0;
}