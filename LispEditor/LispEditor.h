// LispEditor.h : main header file for the LISPEDITOR application
//

#if !defined(AFX_LISPEDITOR_H__39BD7428_2098_4470_BF4D_FAD4D6B96C0C__INCLUDED_)
#define AFX_LISPEDITOR_H__39BD7428_2098_4470_BF4D_FAD4D6B96C0C__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"       // main symbols

/////////////////////////////////////////////////////////////////////////////
// CLispEditorApp:
// See LispEditor.cpp for the implementation of this class
//

class CLispEditorApp : public CWinApp
{
public:
	CLispEditorApp();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CLispEditorApp)
	public:
	virtual BOOL InitInstance();
	//}}AFX_VIRTUAL

// Implementation
	//{{AFX_MSG(CLispEditorApp)
	afx_msg void OnAppAbout();
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

extern CLispEditorApp theApp;

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_LISPEDITOR_H__39BD7428_2098_4470_BF4D_FAD4D6B96C0C__INCLUDED_)
