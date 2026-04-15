// XMLUTF.h : main header file for the XMLUTF application
//

#if !defined(AFX_XMLUTF_H__0540F376_B935_4F3C_AB24_D546C49D39DE__INCLUDED_)
#define AFX_XMLUTF_H__0540F376_B935_4F3C_AB24_D546C49D39DE__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// main symbols

/////////////////////////////////////////////////////////////////////////////
// XMLUTFApp:
// See XMLUTF.cpp for the implementation of this class
//

class XMLUTFApp : public CWinApp
{
public:
	XMLUTFApp();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(XMLUTFApp)
	public:
	virtual BOOL InitInstance();
	//}}AFX_VIRTUAL

// Implementation

	//{{AFX_MSG(XMLUTFApp)
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_XMLUTF_H__0540F376_B935_4F3C_AB24_D546C49D39DE__INCLUDED_)
