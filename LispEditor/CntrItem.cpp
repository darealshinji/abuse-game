// CntrItem.cpp : implementation of the CLispEditorCntrItem class
//

#include "stdafx.h"
#include "LispEditor.h"

#include "LispEditorDoc.h"
#include "LispEditorView.h"
#include "CntrItem.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CLispEditorCntrItem implementation

IMPLEMENT_SERIAL(CLispEditorCntrItem, CRichEditCntrItem, 0)

CLispEditorCntrItem::CLispEditorCntrItem(REOBJECT* preo, CLispEditorDoc* pContainer)
	: CRichEditCntrItem(preo, pContainer)
{
	// TODO: add one-time construction code here
	
}

CLispEditorCntrItem::~CLispEditorCntrItem()
{
	// TODO: add cleanup code here
	
}

/////////////////////////////////////////////////////////////////////////////
// CLispEditorCntrItem diagnostics

#ifdef _DEBUG
void CLispEditorCntrItem::AssertValid() const
{
	CRichEditCntrItem::AssertValid();
}

void CLispEditorCntrItem::Dump(CDumpContext& dc) const
{
	CRichEditCntrItem::Dump(dc);
}
#endif

/////////////////////////////////////////////////////////////////////////////
