// MainDlg.h : interface of the CMainDlg class
//
/////////////////////////////////////////////////////////////////////////////

#pragma once
#include "resource.h"
#define WM_INJECT (WM_USER+8806)
//#define WM_NOTIFY_INJECTER (WM_USER+8807)

class CMainDlg : public CDialogImpl<CMainDlg>, public CUpdateUI<CMainDlg>,
		public CMessageFilter, public CIdleHandler
{
public:
	enum { IDD = IDD_DIALOG_GDI };

	virtual BOOL PreTranslateMessage(MSG* pMsg);
	virtual BOOL OnIdle();

	BEGIN_UPDATE_UI_MAP(CMainDlg)
	END_UPDATE_UI_MAP()

	BEGIN_MSG_MAP(CMainDlg)
		MESSAGE_HANDLER(WM_INITDIALOG, OnInitDialog)
		MESSAGE_HANDLER(WM_DESTROY, OnDestroy)
    COMMAND_ID_HANDLER(IDCANCEL, OnCancel)
    MESSAGE_HANDLER(WM_INJECT, OnInject)
    MSG_WM_PAINT(OnPaint)
    COMMAND_HANDLER(IDC_LIST2, LBN_SELCHANGE, OnSelChange)\
    COMMAND_ID_HANDLER(ID_REFRESH, OnRefresh)
	END_MSG_MAP()

// Handler prototypes (uncomment arguments if needed):
//	LRESULT MessageHandler(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
//	LRESULT CommandHandler(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
//	LRESULT NotifyHandler(int /*idCtrl*/, LPNMHDR /*pnmh*/, BOOL& /*bHandled*/)

	LRESULT OnInitDialog(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
	LRESULT OnDestroy(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
  LRESULT OnInject(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
  LRESULT OnCancel(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/);

  LRESULT OnSelChange(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
  LRESULT OnRefresh(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
  void OnPaint(CDCHandle dc);

	void CloseDialog(int nVal);
  void IniWindowsText();
  void UpdateList();
  void IniList();
  void IniPaintRect();

  void ShowGdi(HGDIOBJ gdi);
  void ShowOBJ_BITMAP(HGDIOBJ gdi);
  void ShowOBJ_BRUSH(HGDIOBJ gdi);
  void ShowOBJ_DC(HGDIOBJ gdi);
  void ShowOBJ_PEN(HGDIOBJ gdi);
  void ShowOBJ_REGION(HGDIOBJ gdi);
  void ShowOBJ_FONT(HGDIOBJ gdi);


private:
  CListBox m_list;
  CRect m_PaintRect;
  HGDIOBJ m_gdidHandle;
};
