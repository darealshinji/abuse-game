// LispEditorView.h : interface of the CLispEditorView class
//
/////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_LISPEDITORVIEW_H__C831F1E9_94CD_4C14_A9DF_55E565119862__INCLUDED_)
#define AFX_LISPEDITORVIEW_H__C831F1E9_94CD_4C14_A9DF_55E565119862__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CLispEditorCntrItem;

class CLispEditorView : public CRichEditView
{
protected: // create from serialization only
	CLispEditorView();
	DECLARE_DYNCREATE(CLispEditorView)

// Attributes
public:
	CLispEditorDoc* GetDocument();
    CHARFORMAT DefaultCF;
    long LastOpenParenIndex, LastCloseParenIndex;
// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CLispEditorView)
	public:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	protected:
	virtual void OnInitialUpdate(); // called first time after construct
	virtual BOOL OnPreparePrinting(CPrintInfo* pInfo);
	//}}AFX_VIRTUAL

// Implementation
public:
	void MatchParen(long Pos);
	long FindNextCloseParen();
	long IsPosQuoted(long Pos);
	long IsPosCommented(long Pos);
	void FormatWord(int Type, long Start, long End);
	void ColorRange(long Start, long End);
	CString GetCurrentWord(long &lFirst, long &lLast);
	long FindPrevOpenParen();
	virtual ~CLispEditorView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// Generated message map functions
protected:
	//{{AFX_MSG(CLispEditorView)
	afx_msg void OnDestroy();
	afx_msg void OnChar(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

#ifndef _DEBUG  // debug version in LispEditorView.cpp
inline CLispEditorDoc* CLispEditorView::GetDocument()
   { return (CLispEditorDoc*)m_pDocument; }
#endif

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_LISPEDITORVIEW_H__C831F1E9_94CD_4C14_A9DF_55E565119862__INCLUDED_)
