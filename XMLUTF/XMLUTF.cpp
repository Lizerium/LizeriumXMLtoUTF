// XMLUTF.cpp : Defines the class behaviors for the application.
//

#include "stdafx.h"
#include "XMLUTF.h"
#include "XMLUTFDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// XMLUTFApp

BEGIN_MESSAGE_MAP(XMLUTFApp, CWinApp)
	//{{AFX_MSG_MAP(XMLUTFApp)
	//}}AFX_MSG
	ON_COMMAND(ID_HELP, CWinApp::OnHelp)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// XMLUTFApp construction

XMLUTFApp::XMLUTFApp()
{
}

/////////////////////////////////////////////////////////////////////////////
// The one and only XMLUTFApp object

XMLUTFApp theApp;

/////////////////////////////////////////////////////////////////////////////
// XMLUTFApp initialization

BOOL XMLUTFApp::InitInstance()
{
	// Standard initialization

#ifdef _AFXDLL
	Enable3dControls();			// Call this when using MFC in a shared DLL
#else
	Enable3dControlsStatic();	// Call this when linking to MFC statically
#endif

	XMLUTFDlg dlg;
	m_pMainWnd = &dlg;
	int nResponse = dlg.DoModal();
	if (nResponse == IDOK)
	{
	}
	else if (nResponse == IDCANCEL)
	{
	}

	// Since the dialog has been closed, return FALSE so that we exit the
	//  application, rather than start the application's message pump.
	return FALSE;
}
