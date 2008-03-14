// LispEditorDoc.cpp : implementation of the CLispEditorDoc class
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
// CLispEditorDoc

IMPLEMENT_DYNCREATE(CLispEditorDoc, CRichEditDoc)

BEGIN_MESSAGE_MAP(CLispEditorDoc, CRichEditDoc)
	//{{AFX_MSG_MAP(CLispEditorDoc)
		// NOTE - the ClassWizard will add and remove mapping macros here.
		//    DO NOT EDIT what you see in these blocks of generated code!
	//}}AFX_MSG_MAP
	// Enable default OLE container implementation
	ON_UPDATE_COMMAND_UI(ID_OLE_EDIT_LINKS, CRichEditDoc::OnUpdateEditLinksMenu)
	ON_COMMAND(ID_OLE_EDIT_LINKS, CRichEditDoc::OnEditLinks)
	ON_UPDATE_COMMAND_UI_RANGE(ID_OLE_VERB_FIRST, ID_OLE_VERB_LAST, CRichEditDoc::OnUpdateObjectVerbMenu)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CLispEditorDoc construction/destruction

CLispEditorDoc::CLispEditorDoc()
{
	// TODO: add one-time construction code here

}

CLispEditorDoc::~CLispEditorDoc()
{
}

BOOL CLispEditorDoc::OnNewDocument()
{
	if (!CRichEditDoc::OnNewDocument())
		return FALSE;

	// TODO: add reinitialization code here
	// (SDI documents will reuse this document)

	return TRUE;
}

CRichEditCntrItem* CLispEditorDoc::CreateClientItem(REOBJECT* preo) const
{
	// cast away constness of this
	return new CLispEditorCntrItem(preo, (CLispEditorDoc*) this);
}



/////////////////////////////////////////////////////////////////////////////
// CLispEditorDoc serialization

void CLispEditorDoc::Serialize(CArchive& ar)
{
	if (ar.IsStoring())
	{
		// TODO: add storing code here
	}
	else
	{
		// TODO: add loading code here
	}

	// Calling the base class CRichEditDoc enables serialization
	//  of the container document's COleClientItem objects.
	// TODO: set CRichEditDoc::m_bRTF = FALSE if you are serializing as text
	CRichEditDoc::Serialize(ar);
}

/////////////////////////////////////////////////////////////////////////////
// CLispEditorDoc diagnostics

#ifdef _DEBUG
void CLispEditorDoc::AssertValid() const
{
	CRichEditDoc::AssertValid();
}

void CLispEditorDoc::Dump(CDumpContext& dc) const
{
	CRichEditDoc::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CLispEditorDoc commands

BOOL CLispEditorDoc::OnSaveDocument(LPCTSTR lpszPathName) 
{
    long Start, End;
    FINDTEXTEX Find;
    ZeroMemory(&Find, sizeof(Find));
    GetView()->GetRichEditCtrl().GetSel(Start, End);
    CHARRANGE cr;
    cr.cpMax = End;
    cr.cpMin = Start;
    
    Start--;
    GetView()->GetRichEditCtrl().SetSel(0, -1);
    CString SearchSpace = GetView()->GetRichEditCtrl().GetSelText();
    
    HANDLE fh = CreateFile(lpszPathName, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
    DWORD Count;

    Count = SearchSpace.GetLength();
    WriteFile(fh, (LPCSTR) SearchSpace, SearchSpace.GetLength(), &Count, NULL);

    CloseHandle(fh);
    GetView()->GetRichEditCtrl().SetSel(cr);
    SetModifiedFlag(FALSE);
    return 1;
}

static DWORD CALLBACK ReadStreamInCallback(DWORD dwCookie, LPBYTE pbBuff, LONG cb, LONG *pcb)
{
   CFile* pFile = (CFile*) dwCookie;

   *pcb = pFile->Read(pbBuff, cb);

   return 0;
}

BOOL CLispEditorDoc::OnOpenDocument(LPCTSTR lpszPathName) 
{
	// TODO: Add your specialized creation code here
    CHARFORMAT cf;
    
    GetView()->GetRichEditCtrl().GetDefaultCharFormat(cf);
    cf.crTextColor = RGB(255, 255, 255);
    cf.dwMask = CFM_COLOR | CFM_BOLD | CFM_FACE;
    cf.dwEffects = 0;
    strcpy(cf.szFaceName, "Fixedsys");
    GetView()->GetRichEditCtrl().SetDefaultCharFormat(cf);


    CFile InFile(TEXT(lpszPathName), CFile::modeRead);
    EDITSTREAM es;

    es.dwCookie = (DWORD) &InFile;
    es.pfnCallback = ReadStreamInCallback; 
    GetView()->GetRichEditCtrl().StreamIn(SF_TEXT, es);

    GetView()->GetRichEditCtrl().SetModify(FALSE);
    ((CLispEditorView*) GetView())->ColorRange(0, -1);
    SetModifiedFlag(FALSE);
	return TRUE;
}
