// LispEditorDoc.h : interface of the CLispEditorDoc class
//
/////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_LISPEDITORDOC_H__9F73AF5D_7EE6_4A07_B8C2_FA78349759FD__INCLUDED_)
#define AFX_LISPEDITORDOC_H__9F73AF5D_7EE6_4A07_B8C2_FA78349759FD__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


class CLispEditorDoc : public CRichEditDoc
{
protected: // create from serialization only
	CLispEditorDoc();
	DECLARE_DYNCREATE(CLispEditorDoc)

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CLispEditorDoc)
	public:
	virtual BOOL OnNewDocument();
	virtual void Serialize(CArchive& ar);
	virtual BOOL OnSaveDocument(LPCTSTR lpszPathName);
	virtual BOOL OnOpenDocument(LPCTSTR lpszPathName);
	//}}AFX_VIRTUAL
	virtual CRichEditCntrItem* CreateClientItem(REOBJECT* preo) const;

// Implementation
public:
	virtual ~CLispEditorDoc();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// Generated message map functions
protected:
	//{{AFX_MSG(CLispEditorDoc)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_LISPEDITORDOC_H__9F73AF5D_7EE6_4A07_B8C2_FA78349759FD__INCLUDED_)
