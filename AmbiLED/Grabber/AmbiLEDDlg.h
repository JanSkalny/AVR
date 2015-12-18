// AmbiLEDDlg.h : header file
//

#pragma once
#include "display.h"
#include "afxwin.h"

class CRegion;

// CAmbiLEDDlg dialog
class CAmbiLEDDlg : public CDialog
{
// Construction
public:
	CAmbiLEDDlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
	enum { IDD = IDD_AMBILED_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support


// Implementation
protected:
	HICON m_hIcon;

	CRegion *m_pSelected;
	void EnableRegion(BOOL bEnable=TRUE);

	// Generated message map functions
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	void SelectRegion(CRegion* pRegion);

	CDisplay m_Display;
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	CComboBox m_cbOutput;
	afx_msg void OnBnClickedNew();
	afx_msg void OnBnClickedDel();
	CButton m_btnDel;
	afx_msg void OnCbnSelchangeOutput();
};
