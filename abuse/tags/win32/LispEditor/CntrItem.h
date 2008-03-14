// CntrItem.h : interface of the CLispEditorCntrItem class
//

#if !defined(AFX_CNTRITEM_H__A94C7F92_A9FA_4D1C_81DF_9305CCA2FBE5__INCLUDED_)
#define AFX_CNTRITEM_H__A94C7F92_A9FA_4D1C_81DF_9305CCA2FBE5__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CLispEditorDoc;
class CLispEditorView;

class CLispEditorCntrItem : public CRichEditCntrItem
{
	DECLARE_SERIAL(CLispEditorCntrItem)

// Constructors
public:
	CLispEditorCntrItem(REOBJECT* preo = NULL, CLispEditorDoc* pContainer = NULL);
		// Note: pContainer is allowed to be NULL to enable IMPLEMENT_SERIALIZE.
		//  IMPLEMENT_SERIALIZE requires the class have a constructor with
		//  zero arguments.  Normally, OLE items are constructed with a
		//  non-NULL document pointer.

// Attributes
public:
	CLispEditorDoc* GetDocument()
		{ return (CLispEditorDoc*)CRichEditCntrItem::GetDocument(); }
	CLispEditorView* GetActiveView()
		{ return (CLispEditorView*)CRichEditCntrItem::GetActiveView(); }

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CLispEditorCntrItem)
	public:
	protected:
	//}}AFX_VIRTUAL

// Implementation
public:
	~CLispEditorCntrItem();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_CNTRITEM_H__A94C7F92_A9FA_4D1C_81DF_9305CCA2FBE5__INCLUDED_)
