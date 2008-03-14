// AbuseInstaller.h : main header file for the ABUSEINSTALLER application
//

#if !defined(AFX_ABUSEINSTALLER_H__1792A1C1_7B11_4F9E_B13F_D7F73A2E7AAA__INCLUDED_)
#define AFX_ABUSEINSTALLER_H__1792A1C1_7B11_4F9E_B13F_D7F73A2E7AAA__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// main symbols

/////////////////////////////////////////////////////////////////////////////
// CAbuseInstallerApp:
// See AbuseInstaller.cpp for the implementation of this class
//

class CAbuseInstallerApp : public CWinApp
{
public:
	CAbuseInstallerApp();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAbuseInstallerApp)
	public:
	virtual BOOL InitInstance();
	//}}AFX_VIRTUAL

// Implementation

	//{{AFX_MSG(CAbuseInstallerApp)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_ABUSEINSTALLER_H__1792A1C1_7B11_4F9E_B13F_D7F73A2E7AAA__INCLUDED_)
