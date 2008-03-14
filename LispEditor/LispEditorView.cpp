// LispEditorView.cpp : implementation of the CLispEditorView class
//

#include "stdafx.h"
#include "LispEditor.h"
#include "MainFrm.h"

#include "LispEditorDoc.h"
#include "CntrItem.h"
#include "LispEditorView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

enum {
    LISP_SYMBOL,
    LISP_NUMBER,
    LISP_STRING,
    LISP_OPERATOR,
    LISP_COMMENT,
    LISP_WORD,
    LISP_WHITESPACE
} LISP_PARTS;


/////////////////////////////////////////////////////////////////////////////
// CLispEditorView

IMPLEMENT_DYNCREATE(CLispEditorView, CRichEditView)

BEGIN_MESSAGE_MAP(CLispEditorView, CRichEditView)
	//{{AFX_MSG_MAP(CLispEditorView)
	ON_WM_DESTROY()
	ON_WM_CHAR()
	ON_WM_KEYDOWN()
	ON_WM_LBUTTONUP()
	//}}AFX_MSG_MAP
	// Standard printing commands
	ON_COMMAND(ID_FILE_PRINT, CRichEditView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_DIRECT, CRichEditView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_PREVIEW, CRichEditView::OnFilePrintPreview)
END_MESSAGE_MAP()


/* --------------------------------------------------------------------------- 
/////////////////////////////////////////////////////////////////////////////
// CLispEditorView construction/destruction
 ------------------------------------------------------------------------- /**/
CLispEditorView::CLispEditorView()
{
	// TODO: add construction code here
    m_nWordWrap = WrapNone;
    LastOpenParenIndex = -1;
    LastCloseParenIndex = -1;
}

/* --------------------------------------------------------------------------- 
 ------------------------------------------------------------------------- /**/
CLispEditorView::~CLispEditorView()
{
}

/* --------------------------------------------------------------------------- 
 ------------------------------------------------------------------------- /**/
BOOL CLispEditorView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: Modify the Window class or styles here by modifying
	//  the CREATESTRUCT cs

	return CRichEditView::PreCreateWindow(cs);
}

/* --------------------------------------------------------------------------- 
 ------------------------------------------------------------------------- /**/
void CLispEditorView::OnInitialUpdate()
{
	CRichEditView::OnInitialUpdate();
    GetRichEditCtrl().SetBackgroundColor(FALSE, RGB(0, 0, 100));
    CHARFORMAT cf;
    
    GetRichEditCtrl().GetDefaultCharFormat(cf);
    cf.crTextColor = RGB(255, 255, 255);
    cf.dwMask = CFM_COLOR | CFM_BOLD | CFM_FACE;
    cf.dwEffects = 0;
    strcpy(cf.szFaceName, "Fixedsys");
    GetRichEditCtrl().SetDefaultCharFormat(cf);
    GetDocument()->SetModifiedFlag(FALSE);

    // Set the printing margins (720 twips = 1/2 inch).
	SetMargins(CRect(720, 720, 720, 720));
}

/* --------------------------------------------------------------------------- 
/////////////////////////////////////////////////////////////////////////////
// CLispEditorView printing
 ------------------------------------------------------------------------- /**/
BOOL CLispEditorView::OnPreparePrinting(CPrintInfo* pInfo)
{
	// default preparation
	return DoPreparePrinting(pInfo);
}

/* --------------------------------------------------------------------------- 
 ------------------------------------------------------------------------- /**/
void CLispEditorView::OnDestroy()
{
	// Deactivate the item on destruction; this is important
	// when a splitter view is being used.
   CRichEditView::OnDestroy();
   COleClientItem* pActiveItem = GetDocument()->GetInPlaceActiveItem(this);
   if (pActiveItem != NULL && pActiveItem->GetActiveView() == this)
   {
      pActiveItem->Deactivate();
      ASSERT(GetDocument()->GetInPlaceActiveItem(this) == NULL);
   }
}

#ifdef _DEBUG
/* --------------------------------------------------------------------------- 
/////////////////////////////////////////////////////////////////////////////
// CLispEditorView diagnostics
 ------------------------------------------------------------------------- /**/
void CLispEditorView::AssertValid() const
{
	CRichEditView::AssertValid();
}

/* --------------------------------------------------------------------------- 
 ------------------------------------------------------------------------- /**/
void CLispEditorView::Dump(CDumpContext& dc) const
{
	CRichEditView::Dump(dc);
}

/* --------------------------------------------------------------------------- 
 ------------------------------------------------------------------------- /**/
CLispEditorDoc* CLispEditorView::GetDocument() // non-debug version is inline
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CLispEditorDoc)));
	return (CLispEditorDoc*)m_pDocument;
}
#endif //_DEBUG

/* --------------------------------------------------------------------------- 
/////////////////////////////////////////////////////////////////////////////
// CLispEditorView message handlers
 ------------------------------------------------------------------------- /**/
int IsSeparator(unsigned char c) {
    if (c == '_')
        return 0;
    if (c < '0')
        return 1;
    if (c > '9' && c < 'A')
        return 1;
    if (c > 'Z' && c < 'a')
        return 1;
    if (c > 'z')
        return 1;

    return 0;

}

/* --------------------------------------------------------------------------- 
 ------------------------------------------------------------------------- /**/
int WordType(CString Word) {
    if (((LPCSTR) Word)[0] >= '0' && ((LPCSTR) Word)[0] <= '9')
        return LISP_NUMBER;
    if (((LPCSTR) Word)[0] == '"')
        return LISP_STRING;
    switch (((LPCSTR) Word)[0]) {
        case ';':
            return LISP_COMMENT;
            break;
        case '/':
            if (Word.GetLength() > 1)
                return LISP_COMMENT;
            else
                return LISP_OPERATOR;
            break;

        case '=':
        case '.':
        case '`':
        case '-':
        case '+':
        case '*':
        case '<':
        case '>':
            return LISP_OPERATOR;
            break;
        case ' ':
        case '\t':
        case '\n':
            return LISP_WHITESPACE;
    }
    if (IsSeparator(((LPCSTR) Word)[0]))
        return LISP_SYMBOL;
    return LISP_WORD;
}

/* --------------------------------------------------------------------------- 
 ------------------------------------------------------------------------- /**/
void CLispEditorView::OnChar(UINT nChar, UINT nRepCnt, UINT nFlags) 
{
	// TODO: Add your message handler code here and/or call default
	CRichEditView::OnChar(nChar, nRepCnt, nFlags);
    GetRichEditCtrl().HideSelection(TRUE, FALSE);
    CHARFORMAT cf, cfbak;
    GetRichEditCtrl().GetDefaultCharFormat(cf);
    cf.dwMask = CFM_BOLD | CFM_COLOR;
    cfbak = cf;
    long Start, End;
    GetRichEditCtrl().GetSel(Start, End);
    
    
    long First, Last;
    int Line;
    Line = GetRichEditCtrl().LineFromChar(Start);
    First = GetRichEditCtrl().LineIndex(Line);
    Last = GetRichEditCtrl().LineIndex(Line+1);
    if (Last == -1)
        Last = GetRichEditCtrl().GetTextLength();
    ColorRange(GetRichEditCtrl().LineIndex(Line), GetRichEditCtrl().LineIndex(Line + 1));
    
    switch (nChar) {
        case '\t': {
            } break;
    }

    MatchParen(Start);
    GetRichEditCtrl().SetSel(Start, End);
    GetRichEditCtrl().SetSelectionCharFormat(cfbak);
    GetRichEditCtrl().HideSelection(FALSE, FALSE);
}

/* --------------------------------------------------------------------------- 
 ------------------------------------------------------------------------- /**/
long CLispEditorView::FindNextCloseParen()
{
    long Start;
    FINDTEXTEX Find;
    ZeroMemory(&Find, sizeof(Find));
    CHARRANGE cr;
    GetRichEditCtrl().GetSel(cr);

    GetRichEditCtrl().SetSel(cr.cpMin, -1);
    Start = 0;
    CString SearchSpace = GetRichEditCtrl().GetSelText();
    int NumOpen = 0;
    while (Start < SearchSpace.GetLength() && NumOpen > -1 && Start < 2000) {
        switch (((LPCSTR) SearchSpace)[Start]) {
            case '(':
                if (IsPosCommented(Start + cr.cpMin) == -1 && IsPosQuoted(Start + cr.cpMin) == -1)
                    NumOpen++;
                break;
            case ')':
                if (IsPosCommented(Start + cr.cpMin) == -1 && IsPosQuoted(Start + cr.cpMin) == -1)
                    NumOpen--;
                break;
        }
        Start++;
/*        long NewStart;
        NewStart =  */
    }

    GetRichEditCtrl().SetSel(cr);
    if (NumOpen > -1)
        return -1;
    else
        return Start + cr.cpMin - 1;
}

/* --------------------------------------------------------------------------- 
 ------------------------------------------------------------------------- /**/
long CLispEditorView::FindPrevOpenParen()
{
    long Start;
    FINDTEXTEX Find;
    ZeroMemory(&Find, sizeof(Find));
    CHARRANGE cr;
    GetRichEditCtrl().GetSel(cr);

    Start = cr.cpMin;
    
    GetRichEditCtrl().SetSel(0, Start);
    CString SearchSpace = GetRichEditCtrl().GetSelText();
    Start--;
    int NumOpen = 0;
    while (Start >= 0 && NumOpen < 1 && (cr.cpMin - Start) < 2000) {
        switch (((LPCSTR) SearchSpace)[Start]) {
            case '(':
                if (IsPosCommented(Start) == -1 && IsPosQuoted(Start) == -1)
                    NumOpen++;
                break;
            case ')':
                if (IsPosCommented(Start) == -1 && IsPosQuoted(Start) == -1)
                    NumOpen--;
                break;
        }
        Start--;
/*        long NewStart;
        NewStart =  */
    }

    GetRichEditCtrl().SetSel(cr);
//    _RPT2(_CRT_WARN, "Found open paren at position %d; NumOpen: %d\n", Start, NumOpen);
    if (NumOpen < 1)
        return -1;
    else
        return Start + 1;
}

/* --------------------------------------------------------------------------- 
 ------------------------------------------------------------------------- /**/
void CLispEditorView::MatchParen(long Pos)
{
    CHARFORMAT cf;
    GetRichEditCtrl().GetDefaultCharFormat(cf);
    cf.dwMask = CFM_BOLD | CFM_COLOR;

    if (LastOpenParenIndex != -1)
        FormatWord(LISP_SYMBOL, LastOpenParenIndex, LastOpenParenIndex + 1);
    if (LastCloseParenIndex != -1)
        FormatWord(LISP_SYMBOL, LastCloseParenIndex, LastCloseParenIndex + 1);
    
    cf.crTextColor = RGB(220, 55, 55);
    cf.dwEffects = 0;

    GetRichEditCtrl().SetSel(Pos, Pos);
    LastOpenParenIndex = FindPrevOpenParen();
    if (LastOpenParenIndex != -1) {
        GetRichEditCtrl().SetSel(LastOpenParenIndex, LastOpenParenIndex + 1);
        GetRichEditCtrl().SetSelectionCharFormat(cf);
    }

    GetRichEditCtrl().SetSel(Pos, Pos);
    LastCloseParenIndex = FindNextCloseParen();
    if (LastCloseParenIndex != -1) {
        GetRichEditCtrl().SetSel(LastCloseParenIndex, LastCloseParenIndex + 1);
        GetRichEditCtrl().SetSelectionCharFormat(cf);
    }
    if ((LastOpenParenIndex == -1 || LastCloseParenIndex == -1) && (LastOpenParenIndex != -1 || LastCloseParenIndex != -1))
        ((CMainFrame*) theApp.m_pMainWnd)->GetStatusBar()->SetPaneText(0, "Detected Mismatched Paren", TRUE);
    else
        ((CMainFrame*) theApp.m_pMainWnd)->GetStatusBar()->SetPaneText(0, "Ready", TRUE);

}

/* --------------------------------------------------------------------------- 
 ------------------------------------------------------------------------- /**/
void CLispEditorView::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags) 
{
	// TODO: Add your message handler code here and/or call default
    CRichEditView::OnKeyDown(nChar, nRepCnt, nFlags); // Call this first so that the caret is updated

    if (nFlags & 256 && GetRichEditCtrl().GetSelectionType() == SEL_EMPTY) {
        CPoint Pt = GetRichEditCtrl().GetCaretPos();
/*        Pt.x = GetRichEditCtrl().LineIndex(-1);
        Pt.y =
             */
        GetRichEditCtrl().HideSelection(TRUE, FALSE);
        CHARFORMAT cf, cfbak;
        GetRichEditCtrl().GetDefaultCharFormat(cf);
        cf.dwMask = CFM_BOLD | CFM_COLOR;
        cfbak = cf;
        long Start, End;
        GetRichEditCtrl().GetSel(Start, End);
        MatchParen(Start);
    
        if (Start <= End)
            GetRichEditCtrl().SetSel(Start, End);
        else
            GetRichEditCtrl().SetSel(End, Start);
        GetRichEditCtrl().SetSelectionCharFormat(cfbak);
        GetRichEditCtrl().SetCaretPos(Pt);
        GetRichEditCtrl().HideSelection(FALSE, FALSE);
    }
}

/* --------------------------------------------------------------------------- 
 ------------------------------------------------------------------------- /**/
void CLispEditorView::OnLButtonUp(UINT nFlags, CPoint point)
{
	// TODO: Add your message handler code here and/or call default
    if (GetRichEditCtrl().GetSelectionType() == SEL_EMPTY) {
        GetRichEditCtrl().HideSelection(TRUE, FALSE);
        CHARFORMAT cf, cfbak;
        GetRichEditCtrl().GetDefaultCharFormat(cf);
        cf.dwMask = CFM_BOLD | CFM_COLOR;
        cfbak = cf;
        long Start, End;
        GetRichEditCtrl().GetSel(Start, End);
        MatchParen(Start);
    
        if (Start <= End)
            GetRichEditCtrl().SetSel(Start, End);
        else
            GetRichEditCtrl().SetSel(End, Start);
        GetRichEditCtrl().SetSelectionCharFormat(cfbak);
        GetRichEditCtrl().HideSelection(FALSE, FALSE);
    }
	CRichEditView::OnLButtonUp(nFlags, point);
}

/* --------------------------------------------------------------------------- 
 ------------------------------------------------------------------------- /**/
CString CLispEditorView::GetCurrentWord(long &lFirst, long &lLast)
{
    int* buf;
    int BufLen;
    CString ret;
    long Start, End, Line;
    GetRichEditCtrl().GetSel(Start, End);
    Line = GetRichEditCtrl().LineFromChar(Start);
    BufLen = GetRichEditCtrl().LineLength();
    buf = (int*) malloc(4 + BufLen);
    ZeroMemory(buf, 4 + BufLen);
    buf[0] = BufLen;
    GetRichEditCtrl().GetLine(Line, (LPTSTR) buf, BufLen);
    CString SearchSpace = (char*) buf;
    delete buf;

    int Index, FirstChar, LastChar;
    Index = Start - GetRichEditCtrl().LineIndex() - 1;
    
    int Quote1Pos, Quote2Pos = -1, CommentPos = 0;
    do {
        CommentPos = SearchSpace.Find(';', Quote2Pos + 1);
        if (CommentPos == -1)
            CommentPos = 16000; // Hopefully this will never be exceeded...

        Quote1Pos = SearchSpace.Find('"', Quote2Pos + 1);
        if (Quote1Pos != -1) {
            Quote2Pos = SearchSpace.Find('"', Quote1Pos + 1);
            if (Quote2Pos == -1)
                Quote2Pos = SearchSpace.GetLength();
        }
    } while (Quote1Pos != -1 && (Quote1Pos > Index || Quote2Pos < Index) && Quote1Pos < CommentPos);
    
    if (Quote1Pos != -1 && Quote1Pos <= Index && Quote2Pos >= Index && Quote1Pos < CommentPos) {
        ret = SearchSpace.Mid(Quote1Pos, Quote2Pos);
        lFirst = GetRichEditCtrl().LineIndex() + Quote1Pos;
        lLast = GetRichEditCtrl().LineIndex() + Quote2Pos + 1;
        return ret;
    }
    else if (CommentPos != 16000) {
        ret = SearchSpace.Right(SearchSpace.GetLength() - CommentPos);
        lFirst = GetRichEditCtrl().LineIndex() + CommentPos;
        lLast = GetRichEditCtrl().LineIndex() + SearchSpace.GetLength() + 1;
        return ret;
    }
        
    if (IsSeparator(((LPCSTR) SearchSpace)[Index])) {
        lFirst = Index;
        lLast = Index + 1;
        ret = (((LPCSTR) SearchSpace)[Index]);
        _RPT1(_CRT_WARN, "Current Word is %s\n", (LPCSTR) ret);
        return ret;
    }
    while (Index > 0 && !IsSeparator(((LPCSTR) SearchSpace)[Index]))
        Index--;
    if (IsSeparator(((LPCSTR) SearchSpace)[Index]))
        Index++;
    FirstChar = Index;
    Index = Start - GetRichEditCtrl().LineIndex() - 1;
    while (Index < SearchSpace.GetLength() && !IsSeparator(((LPCSTR) SearchSpace)[Index]))
        Index++;
    LastChar = Index;

    ret = SearchSpace.Right(SearchSpace.GetLength() - FirstChar);
    ret = ret.Left(LastChar - FirstChar);
//    _RPT2(_CRT_WARN, "Found open paren at position %d; NumOpen: %d\n", Start, NumOpen);
    _RPT1(_CRT_WARN, "Current Word is %s\n", (LPCSTR) ret);
    lFirst = GetRichEditCtrl().LineIndex() + FirstChar;
    lLast = GetRichEditCtrl().LineIndex() + LastChar;
//    lLast = LastChar - FirstChar - 1;
    return ret;
}

/* --------------------------------------------------------------------------- 
 ------------------------------------------------------------------------- /**/
void CLispEditorView::ColorRange(long Start, long End)
{
    CString SearchSpace, CurrentWord, StatusText;

    
    int QuotePos, CommentPos;
    CommentPos = IsPosCommented(Start);
    if (CommentPos != -1)
        Start = CommentPos;
    else {
        QuotePos = IsPosQuoted(Start);
        if (QuotePos != -1)
            Start = QuotePos;
    }

    CHARRANGE cr;
    GetRichEditCtrl().GetSel(cr);
    GetRichEditCtrl().SetSel(Start, End);
    SearchSpace = GetRichEditCtrl().GetSelText();

    int Index = 0, FirstChar = 0;
    while (FirstChar < SearchSpace.GetLength()) {
        
        StatusText.Format("Formatting -- %d%%", (int) ((float) FirstChar / SearchSpace.GetLength() * 100));
        ((CMainFrame*) theApp.m_pMainWnd)->GetStatusBar()->SetPaneText(0, (LPCSTR) StatusText, TRUE);
        while (((LPCSTR) SearchSpace)[Index] == ' ')
            Index++;
        FirstChar = Index;
        while (Index < SearchSpace.GetLength() && (!IsSeparator(((LPCSTR) SearchSpace)[Index])))
            Index++;
        if (Index == FirstChar) {
            unsigned char c = ((LPCSTR) SearchSpace)[Index];
            switch (c) {
                case ';': {
                        int EndOfLine = SearchSpace.Find('\n', Index);
                        if (EndOfLine == -1)
                            EndOfLine = SearchSpace.GetLength();
                        Index = EndOfLine;

                    } break;
                case '/': {
                        if (((LPCSTR) SearchSpace)[Index+1] == '*') {
                            int EndComment = SearchSpace.Find("*/", Index + 2);
                            if (EndComment != -1)
                                Index = EndComment + 2;
                            else {
                                Index = SearchSpace.GetLength();
                            }
                        }
                        else
                            Index++;
                    } break;
                case '"': {
                        int LastQuote = SearchSpace.Find('"', Index + 1);
                        if (LastQuote != -1)
                            Index = LastQuote + 1;
                        else {
                            LastQuote = SearchSpace.Find('\n', Index + 1);
                            if (LastQuote != -1)
                                Index = LastQuote;
                            else
                                Index = SearchSpace.GetLength();
                        }

                    } break;
                default:
                    Index++;
            }
        }
        CurrentWord = SearchSpace.Mid(FirstChar, Index - FirstChar);
//        _RPT1(_CRT_WARN, "Current Word is %s\n", (LPCSTR) CurrentWord);
        FormatWord(WordType(CurrentWord), Start + FirstChar, Start + Index);
        FirstChar = Index;
    }
    GetRichEditCtrl().SetSel(cr);
    ((CMainFrame*) theApp.m_pMainWnd)->GetStatusBar()->SetPaneText(0, "Ready", TRUE);
}

/* --------------------------------------------------------------------------- 
 ------------------------------------------------------------------------- /**/
/*void *compile(char *&s)
{
    void *ret = NULL;
    if (!read_ltoken(s, n))
        lerror(NULL, "unexpected end of program");
    if (streq(n, "nil"))
        return NULL;
    else 
        if (toupper(n[0]) == 'T' && !n[1])
            return true_symbol;
        else if (n[0] == '\'') {                // short hand for quote function
            void *cs=new_cons_cell(),*c2=NULL;
            p_ref r1(cs), r2(c2);
            
            ((cons_cell *)cs)->car = (cons_cell*) quote_symbol;
            c2 = new_cons_cell();
            ((cons_cell *)c2)->car = (cons_cell*) compile(s);
            ((cons_cell *)c2)->cdr = NULL;
            ((cons_cell *)cs)->cdr = (cons_cell*) c2;
            ret = cs;
        }
        else if (n[0] == '`') {                 // short hand for backquote function
            void *cs = new_cons_cell(), *c2 = NULL;
            p_ref r1(cs), r2(c2);
            
            ((cons_cell *)cs)->car = (cons_cell*) backquote_symbol;
            c2 = new_cons_cell();
            ((cons_cell *)c2)->car = (cons_cell*) compile(s);
            ((cons_cell *)c2)->cdr = NULL;
            ((cons_cell *)cs)->cdr = (cons_cell*) c2;
            ret = cs;
        }
        else if (n[0] == ',') {                 // short hand for comma function
            void *cs = new_cons_cell(), *c2 = NULL;
            p_ref r1(cs), r2(c2);
            
            ((cons_cell *)cs)->car = (cons_cell*) comma_symbol;
            c2 = new_cons_cell();
            ((cons_cell *)c2)->car = (cons_cell*) compile(s);
            ((cons_cell *)c2)->cdr = NULL;
            ((cons_cell *)cs)->cdr = (cons_cell*) c2;
            ret = cs;
        }
        else if (n[0] == '(') {                 // make a list of everything in ()
            void *first = NULL, *cur = NULL, *last = NULL;   
            p_ref r1(first), r2(cur), r3(last);
            int done = 0;
            do {
                char *tmp = s;
                if (!read_ltoken(tmp, n))       // check for the end of the list
                    lerror(NULL, "unexpected end of program");
                if (n[0] == ')') {
                    done = 1;
                    read_ltoken(s, n);                // read off the ')'
                }
                else {
                    if (n[0] == '.' && !n[1]) {
                        if (!first)
                            lerror(s, "token '.' not allowed here\n");	      
                        else {
                            read_ltoken(s, n);              // skip the '.'
                            ((cons_cell *)last)->cdr = (cons_cell*) compile(s);          // link the last cdr to 
                            last = NULL;
                        }
                    }
                    else 
                        if (!last && first)
                            lerror(s, "illegal end of dotted list\n");
                        else {
                            cur = new_cons_cell();
                            p_ref r1(cur);
                            if (!first) 
                                first = cur;
                            ((cons_cell*) cur)->car = (cons_cell*) compile(s);	
                            if (last)
                                ((cons_cell *)last)->cdr = (cons_cell*) cur;
                            last = cur;
                        }
                }
            }
            while (!done);
            ret = comp_optimize(first);
        }
        else if (n[0]==')')
            lerror(s,"mismatched )");
        else if (isdigit(n[0]) || (n[0]=='-' && isdigit(n[1]))) {
            lisp_number *num=new_lisp_number(0);
            sscanf(n,"%d",&num->num);
            ret=num;
        }
        else if (n[0]=='"') {
            ret=new_lisp_string(str_token_len(s));
            char *start=lstring_value(ret);
            for (;*s && (*s!='"' || s[1]=='"');s++,start++) {
                if (*s=='\\') {
                    s++;
                    if (*s=='n') *start='\n';
                    if (*s=='r') *start='\r';
                    if (*s=='t') *start='\t';
                    if (*s=='\\') *start='\\';
                }
                else 
                    *start=*s;
                if (*s=='"') 
                    s++;
            }
            *start=0;
            s++;
        }
        else if (n[0]=='#') {
            if (n[1]=='\\') {
                read_ltoken(s,n);                   // read character name
                if (streq(n,"newline"))
                    ret=new_lisp_character('\n');
                else if (streq(n,"space"))
                    ret=new_lisp_character(' ');       
                else 
                    ret=new_lisp_character(n[0]);       
            }
            else if (n[1]==0) {                           // short hand for function
                void *cs=new_cons_cell(),*c2=NULL;
                p_ref r4(cs),r5(c2);
                ((cons_cell *)cs)->car = (cons_cell*) make_find_symbol("function");
                c2=new_cons_cell();
                ((cons_cell *)c2)->car = (cons_cell*) compile(s);
                ((cons_cell *)cs)->cdr = (cons_cell*) c2;
                ret=cs;
            }
            else {
                lbreak("Unknown #\\ notation : %s\n",n);
            }
        }
        else 
            return make_find_symbol(n);
        
    return ret;
}
*/

/* --------------------------------------------------------------------------- 
 ------------------------------------------------------------------------- /**/
void CLispEditorView::FormatWord(int Type, long Start, long End)
{
    CHARFORMAT cf;
    GetRichEditCtrl().GetDefaultCharFormat(cf);
    cf.dwMask = CFM_COLOR | CFM_BOLD;
    cf.dwEffects = 0;
    switch (Type) {
        case LISP_SYMBOL:
            cf.crTextColor = RGB(0, 220, 0);
            break;
        
        case LISP_NUMBER:
            cf.crTextColor = RGB(0, 255, 255);
            break;

        case LISP_STRING:
            cf.crTextColor = RGB(255, 0, 0);
            break;

        case LISP_OPERATOR:
            cf.crTextColor = RGB(200, 200, 0);
            break;
        case LISP_COMMENT:
            cf.crTextColor = RGB(150, 150, 150);
            break;
        case LISP_WHITESPACE:
            break;
        default:
            cf.crTextColor = RGB(255, 255, 255);
            break;

    }
    GetRichEditCtrl().SetSel(Start, End);
    GetRichEditCtrl().SetSelectionCharFormat(cf);
}

/* --------------------------------------------------------------------------- 
    Returns the character index of the opening quote if Pos is within a
    comment; otherwise returns -1
 ------------------------------------------------------------------------- /**/
long CLispEditorView::IsPosCommented(long Pos)
{
    int* buf;
    int BufLen, Start, Line;
    CString ret;

    Line = GetRichEditCtrl().LineFromChar(Pos);
    BufLen = GetRichEditCtrl().LineLength();
    Start = GetRichEditCtrl().LineIndex(Line);

    buf = (int*) malloc(4 + BufLen);    ZeroMemory(buf, 4 + BufLen);    buf[0] = BufLen;
    GetRichEditCtrl().GetLine(Line, (LPTSTR) buf, BufLen);
    CString SearchSpace = (char*) buf;
    delete buf;

    int CommentPos = -1;
    do {
        CommentPos = SearchSpace.Find(';', CommentPos + 1);
        if (CommentPos != -1 && (CommentPos + Start < Pos) && IsPosQuoted(CommentPos + Start) == -1)
            return CommentPos + Start; // We found a commented out section, return the start index
    } while (CommentPos != -1);

    FINDTEXTEX ft;
    ZeroMemory(&ft, sizeof(ft));
    ft.chrg.cpMax = Pos;
    int BlockStart, BlockEnd = 0;

    do {
        ft.chrg.cpMin = BlockEnd;
        ft.lpstrText = "/*";
        BlockStart = GetRichEditCtrl().FindText(0, &ft);
        if (BlockStart != -1) {
            ft.chrg.cpMin = BlockStart + 2;
            ft.lpstrText = "*/";
            BlockEnd = GetRichEditCtrl().FindText(0, &ft);
            if (BlockEnd == -1)
                BlockEnd = Pos + 1;
        }
    } while (BlockStart != -1 && BlockEnd < Pos);

    if (BlockStart != -1 && BlockStart <= Pos && BlockEnd > Pos) {
        if (IsPosQuoted(BlockStart) == -1)
            return BlockStart;
    }

    return -1;
}

/* --------------------------------------------------------------------------- 
    Returns the character index of the opening quote if Pos is contained within
    quotes; otherwise returns -1
 ------------------------------------------------------------------------- /**/
long CLispEditorView::IsPosQuoted(long Pos)
{
    int* buf;
    int BufLen, Line, Start;
    CString ret;

    Line = GetRichEditCtrl().LineFromChar(Pos);
    BufLen = GetRichEditCtrl().LineLength();
    Start = GetRichEditCtrl().LineIndex(Line);
    Pos -= Start;
    buf = (int*) malloc(4 + BufLen);
    ZeroMemory(buf, 4 + BufLen);
    buf[0] = BufLen;
    GetRichEditCtrl().GetLine(Line, (LPTSTR) buf, BufLen);
    CString SearchSpace = (char*) buf;
    delete buf;

    int Quote1Pos, Quote2Pos = -1;
    do {
        Quote1Pos = SearchSpace.Find('"', Quote2Pos + 1);
        if (Quote1Pos != -1) {
            Quote2Pos = SearchSpace.Find('"', Quote1Pos + 1);
            if (Quote2Pos == -1)
                Quote2Pos = SearchSpace.GetLength();
        }
    } while (Quote1Pos != -1 && Quote2Pos < Pos);
    
    if (Quote1Pos != -1 && Quote1Pos <= Pos && Quote2Pos > Pos)
        if (IsPosCommented(Quote1Pos + Start) == -1)
            return Quote1Pos + Start;

    return -1;
}