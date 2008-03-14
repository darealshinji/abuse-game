// AbuseInstallerDlg.cpp : implementation file
//

#include "stdafx.h"
#include "AbuseInstaller.h"
#include "AbuseInstallerDlg.h"
#include "afxdisp.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CAbuseInstallerDlg dialog

CAbuseInstallerDlg::CAbuseInstallerDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CAbuseInstallerDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CAbuseInstallerDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
	// Note that LoadIcon does not require a subsequent DestroyIcon in Win32
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	InitInstance();
}

void CAbuseInstallerDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CAbuseInstallerDlg)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}

BOOL CAbuseInstallerDlg::InitInstance()
{
    AfxEnableControlContainer();
    return TRUE;
}

BEGIN_MESSAGE_MAP(CAbuseInstallerDlg, CDialog)
	//{{AFX_MSG_MAP(CAbuseInstallerDlg)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_BN_CLICKED(IDC_INSTALL_PLAY_BUTTON, OnInstallPlayButtonClick)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CAbuseInstallerDlg message handlers

BOOL CAbuseInstallerDlg::OnInitDialog()
{
    CDialog::OnInitDialog();

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon
	
	// TODO: Add extra initialization here

    HKEY InstalledKey;

    unsigned char ExePath[1024];
    ZeroMemory(ExePath, sizeof(ExePath));
    long result = RegOpenKeyEx(HKEY_LOCAL_MACHINE, "Software\\PosseSoft\\Abuse32", 0, KEY_READ, &InstalledKey);
    if (result == ERROR_SUCCESS) {
        unsigned long Type, DataSize;
        result = RegQueryValueEx(InstalledKey, "InstallDir", NULL, &Type, ExePath, &DataSize);
        strcat((char*) ExePath, "\\uninst.isu");
        if (result == ERROR_SUCCESS) {
            CFileFind Find;
            if (Find.FindFile((LPCSTR) ExePath)) {
                PostQuitMessage(0);
                _RPT0(_CRT_WARN, "Abuse found; Exiting.\n");
            }
        }
    }
	return TRUE;  // return TRUE  unless you set the focus to a control
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CAbuseInstallerDlg::OnPaint() 
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, (WPARAM) dc.GetSafeHdc(), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}

// The system calls this to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CAbuseInstallerDlg::OnQueryDragIcon()
{
	return (HCURSOR) m_hIcon;
}

void CAbuseInstallerDlg::OnInstallPlayButtonClick() 
{
	// TODO: Add your control notification handler code here
	PROCESS_INFORMATION ProcessInfo;
    STARTUPINFO StartupInfo;
    ZeroMemory(&StartupInfo, sizeof(StartupInfo));
    StartupInfo.cb = sizeof(StartupInfo);
    CreateProcess("Install\\Abuse Setup.exe", "", NULL, NULL, FALSE, 0, NULL, "Install\\", &StartupInfo, &ProcessInfo);
}
