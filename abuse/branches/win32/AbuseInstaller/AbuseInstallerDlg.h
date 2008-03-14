// AbuseInstallerDlg.h : header file
//

#if !defined(AFX_ABUSEINSTALLERDLG_H__EE5C7F9B_FB9E_400D_A045_EA1056A3B573__INCLUDED_)
#define AFX_ABUSEINSTALLERDLG_H__EE5C7F9B_FB9E_400D_A045_EA1056A3B573__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

/////////////////////////////////////////////////////////////////////////////
// CAbuseInstallerDlg dialog

class CAbuseInstallerDlg : public CDialog
{
// Construction
public:
	CAbuseInstallerDlg(CWnd* pParent = NULL);	// standard constructor
	virtual BOOL InitInstance();

// Dialog Data
	//{{AFX_DATA(CAbuseInstallerDlg)
	enum { IDD = IDD_ABUSEINSTALLER_DIALOG };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAbuseInstallerDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	//{{AFX_MSG(CAbuseInstallerDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg void OnInstallPlayButtonClick();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_ABUSEINSTALLERDLG_H__EE5C7F9B_FB9E_400D_A045_EA1056A3B573__INCLUDED_)
