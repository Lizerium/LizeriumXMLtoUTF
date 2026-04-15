/******************************************************************************
 * XMLUTFDlg.cpp
 *
 *
 *		XML to Freelancer UTF Conversion Utility
 *
 *		Written by Sir Lancelot (a.k.a. Sirlancelot, Elviis, Yud, vEct0r)
 *		email: elvviis@hotmail.com (or elvviis@yahoo.com)
 *		Placed in the Public Domain on 05-Nov-2004
 *
 *		Updated by Jason Hood (a.k.a adoxa)
 *		email: jadoxa@yahoo.com.au
 *		http://freelancer.adoxa.cjb.net/
 *
 *
 * ----------------------------------------------------------------------------
 * PURPOSE:
 *
 *		This utility converts XML files to Microsoft Freelancer UTF (Universal
 *		Tree Format) data files.  The XML files may be generated using the
 *		UTFXML Utility and hand-edited before converting back to UTF format.
 *		This utility allows you to convert a single XML file or numerous XML
 *		files in nested folders in a single pass.
 *
 *
 * ----------------------------------------------------------------------------
 * NOTES:
 *
 *		This is the fourth release of the XML to UTF Conversion Utility.
 *		This application was built using Microsoft Visual C++ 6.0
 *
 *		If you make changes to this program, please send a copy to the
 *		original author at the email address listed above.	If it enhances
 *		the utility, I may include the changes in a future release,
 *
 *		If you understand the UTF files and want to work on a Mod, but you don't
 *		have a project in mind, I invite you to join the BeyondEarth team.	One
 *		of the following URLs should work:
 *
 *			http://phpbb.completeforum.com/index.php?mforum=beyondearth
 *			http://s8.invisionfree.com/beyondearthmod/
 *
 *			   email: lancerfree2003@yahoo.com (Emperor Tuna)
 *			   email: CoolGuy7432@hotmail.com (Jadean)
 *			   email: elvviis@hotmail.com (Sirlancelot)
 *
 *		Source code files for additional utilities are posted on:
 *			http://games.groups.yahoo.com/group/beyondearth/
 *
 *
 * ----------------------------------------------------------------------------
 * TODO:
 *
 *		Enable the HELP button and provide some help!
 *		If "stringfirst" attribute was set in the UTFXML tag, put the String
 *		   Segment before the Tree Segment.
 *
 * ----------------------------------------------------------------------------
 * CREDITS:
 *
 *		For researching/publishing of UTF and VMeshRef structs, fl_crc32
 *		  Mario "HCl" Brito (mbrito@student.dei.uc.pt)
 *		  Matthew "dizzy" Ruggiero
 *		For FLModelTool, FLAddRadius, CMP Exporter, SUR file structs
 *		  Colin Sandby, Harrier, Anton (hitchhiker54@yahoo.com), Twex,
 *		  Phantom Fox, Free Spirit, Dr Del, CCCP, shsan, Skyshooter, Brutus
 *
 *		Sorry I don't exactly know who did what, but they all did something
 *		which paved the way for this utility.  If I left anyone out, please
 *		email me at the above email address and I will update the list.
 *
 *
 * ----------------------------------------------------------------------------
 * REVISION HISTORY:
 *
 *		04-Nov-2004  V1.0a Sir Lancelot
 *							Original design and implementation
 *		06-Nov-2004  V1.1  Sir Lancelot
 *							Check resulting UTF file size against the original file size.
 *							Parse "unk234" attribute
 *
 *		14-Feb-2010  V2.0  Jason Hood
 *							NOT COMPATIBLE with earlier version.
 *							Don't exit automatically.
 *							Create the destination path (final directory only).
 *							Ignore case more often.
 *							Allow multi-line comments.
 *							New attribute "hash", used to generate the hash code
 *								for new audio entries.
 *
 *		21-Mar-2010  V2.1  Jason Hood
 *							Removed compatibility code (although "stringfirst" and
 *								"prepaddata" will still work).
 *							Ignore tabs.
 *							Fixed multi-line comments.
 *
 *		18-Aug-2010	 V2.2  Jason Hood
 *							Fixed integer ALE RGB conversions.
 *							Continue if Freelancer not apparently installed.
 *							Recognise name for Alchemy type values.
 *							Recognise Q for quaternion values.
 *							Command line options.
 *							Add the summary to the log.
 *							Create the complete destination path.
 *							Write the log to the temporary directory.
 *
 ******************************************************************************/


//////////////////////////////////////////////////////////////////////
// Include Files
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "XMLUTF.h"
#include "XMLUTFDlg.h"
#include "ProgressDialog.h"
#include "store.h"
#include <cmath>


//////////////////////////////////////////////////////////////////////
// Definitions
//////////////////////////////////////////////////////////////////////

#define XMLUTF_VERSION	"XMLUTF Version 2.2 built 19-Aug-2010"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


// Case insensitive version of strstr.
char *stristr(LPCSTR s1, LPCSTR s2)
{
	char *i1 = strdup(s1);
	char *i2 = strdup(s2);
	strlwr(i1);
	strlwr(i2);
	char *rc = strstr(i1, i2);
	if (rc)
	{
		rc += s1 - i1;
	}
	free(i2);
	free(i1);
	return rc;
}


class XMLUTFCommandLineInfo //: public CCommandLineInfo
{
public:
	XMLUTFCommandLineInfo() : subfolders(true), arg(false) { }

	/*virtual*/ void ParseParam(LPCSTR param, BOOL bFlag, BOOL bLast);

	CString dest;			// -o path
	CString src;			// file
	bool	subfolders;		// -O instead of -o

	bool	arg;
};


void XMLUTFCommandLineInfo::ParseParam(LPCSTR param, BOOL bFlag, BOOL bLast)
{
	if (!bFlag)
	{
		if (arg)
		{
			dest = param;
			arg = false;
		}
		else
		{
			src = param;
		}
		return;
	}

	for (; *param; ++param)
	{
		switch (*param)
		{
		case 'O':
			subfolders = false;
			// fall through

		case 'o':
			if (param[1])
			{
				dest = param + 1;
				return;
			}
			arg = true;
			break;
		}
	}
}


//////////////////////////////////////////////////////////////////////
// BackgroundConversionTask entry point
//////////////////////////////////////////////////////////////////////

static UINT AFX_CDECL BackgroundConversionThread( void *args )
{
	return ((XMLUTFDlg *) args)->RunBackgroundConversionThread();
}


//////////////////////////////////////////////////////////////////////
// CAboutDlg class definition
//////////////////////////////////////////////////////////////////////

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

// Dialog Data
	//{{AFX_DATA(CAboutDlg)
	enum { IDD = IDD_ABOUTBOX };
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAboutDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	//{{AFX_MSG(CAboutDlg)
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

	CFont m_FontProgramName;
};


//////////////////////////////////////////////////////////////////////
// CAboutDlg class implementation
//////////////////////////////////////////////////////////////////////

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
	//{{AFX_MSG_MAP(CAboutDlg)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
	//{{AFX_DATA_INIT(CAboutDlg)
	//}}AFX_DATA_INIT
}


void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CAboutDlg)
	//}}AFX_DATA_MAP
}


BOOL CAboutDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();

	LOGFONT logfont = {NULL};
	GetDlgItem(IDC_PROGRAM_NAME)->GetFont()->GetLogFont(&logfont);
	logfont.lfWeight = FW_HEAVY;
	logfont.lfHeight = (LONG) (logfont.lfHeight * 1.5);
	m_FontProgramName.CreateFontIndirect(&logfont);
	GetDlgItem(IDC_PROGRAM_NAME)->SetFont(&m_FontProgramName);

	return TRUE;  // return TRUE unless you set the focus to a control
				  // EXCEPTION: OCX Property Pages should return FALSE
}


//////////////////////////////////////////////////////////////////////
// XMLUTFDlg class implementation
//////////////////////////////////////////////////////////////////////

XMLUTFDlg::XMLUTFDlg(CWnd* pParent /*=NULL*/)
	: CDialog(XMLUTFDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(XMLUTFDlg)
	m_SourcePath = _T("");
	m_DestinationPath = _T("");
	m_XmlFilenames = _T("");
	//}}AFX_DATA_INIT
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	m_XmlDataBuffer = NULL;
	m_LogFile = NULL;
	m_UtfRoot = NULL;
	m_ConversionInProgress = false;
	m_string = NULL;
	m_strcap = 0;
}


void XMLUTFDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(XMLUTFDlg)
	DDX_Control(pDX, IDC_XML_FILENAMES, m_XmlFilenamesComboBox);
	DDX_Text(pDX, IDC_SOURCE_PATH, m_SourcePath);
	DDX_Text(pDX, IDC_DESTINATION_PATH, m_DestinationPath);
	DDX_CBString(pDX, IDC_XML_FILENAMES, m_XmlFilenames);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(XMLUTFDlg, CDialog)
	//{{AFX_MSG_MAP(XMLUTFDlg)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_CONVERT, OnConvert)
	ON_BN_CLICKED(IDC_HELP_BUTTON, OnHelpButton)
	ON_BN_CLICKED(IDC_BROWSE_XML_FILENAME, OnBrowseXmlFilename)
	ON_CBN_SELCHANGE(IDC_XML_FILENAMES, OnSelchangeXmlFilenames)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// XMLUTFDlg message handlers

BOOL XMLUTFDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// Add "About..." menu item to system menu.

	// IDM_ABOUTBOX must be in the system command range.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		CString strAboutMenu;
		strAboutMenu.LoadString(IDS_ABOUTBOX);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	SetIcon(m_hIcon, TRUE); 		// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon

	// Initialize the Critical Section for the background conversion thread
	InitializeCriticalSection(&m_Mutex);

	char app_path[MAX_PATH];
	char source_path[MAX_PATH];
	char dest_path[MAX_PATH];

	REGSAM sam = KEY_READ + KEY_WRITE;
	DWORD Size;

	HKEY hKey = NULL;
	if (RegCreateKeyEx(HKEY_LOCAL_MACHINE, FREELANCER_KEYPATH, 0, NULL, 0, sam, NULL, &hKey, NULL) != ERROR_SUCCESS)
	{
		CString msg = "Unable to create/open Freelancer's registry key.";
		msg += "\n\nHKLM\\";
		msg += FREELANCER_KEYPATH;
		MessageBox(msg, "Error");
		EndDialog(0);
		return FALSE;
	}

	Size = sizeof(app_path);
	if (RegQueryValueEx(hKey, "AppPath", 0, NULL, (BYTE *) app_path, &Size) != ERROR_SUCCESS)
	{
		BROWSEINFO bi;
		bi.hwndOwner = NULL;
		bi.pidlRoot = NULL;
		bi.pszDisplayName = app_path;
		bi.lpszTitle = "Select path to Freelancer...";
		bi.ulFlags = 0;
		bi.lpfn = NULL;
		LPITEMIDLIST pidl = SHBrowseForFolder(&bi);
		if (pidl == NULL)
		{
			EndDialog(0);
			return FALSE;
		}
		SHGetPathFromIDList(pidl, app_path);
		CoTaskMemFree(pidl);
		RegSetValueEx(hKey, "AppPath", 0, REG_SZ, (BYTE *) app_path, strlen(app_path));
	}

	Size = sizeof(source_path);
	if (RegQueryValueEx( hKey, "XMLUTF SourcePath", 0, NULL, (BYTE *) source_path, &Size ) != ERROR_SUCCESS)
	{
		Size = sizeof(source_path);
		if (RegQueryValueEx( hKey, "UTFXML DestPath", 0, NULL, (BYTE *) source_path, &Size ) != ERROR_SUCCESS)
		{
			strcpy(source_path, app_path);
			strcat(source_path, "\\XML");
		}
		RegSetValueEx(hKey, "XMLUTF SourcePath", 0, REG_SZ, (BYTE *) source_path, strlen(source_path));
	}

	Size = sizeof(dest_path);
	if (RegQueryValueEx( hKey, "XMLUTF DestPath", 0, NULL, (BYTE *) dest_path, &Size ) != ERROR_SUCCESS)
	{
		strcpy(dest_path, source_path);
		int i = strlen(dest_path);
		if (i >= 4 && STRIEQ(dest_path+i-4, "\\XML"))
		{
			i -= 4;
		}
		strcpy(dest_path+i, "\\UTF");
		RegSetValueEx(hKey, "XMLUTF DestPath", 0, REG_SZ, (BYTE *) dest_path, strlen(dest_path));
	}

	RegCloseKey( hKey );

	XMLUTFCommandLineInfo options;
	//ParseCommandLine(options);
	for (int a = 1; a < __argc; ++a)
	{
		LPCSTR arg = __argv[a];
		BOOL flag = FALSE;
		if (*arg == '-' || *arg == '/')
		{
			++arg;
			flag = TRUE;
		}
		options.ParseParam(arg, flag, (a == __argc - 1));
	}

	m_Quiet = !options.src.IsEmpty();
	if (m_Quiet)
	{
		LPSTR name;
		GetFullPathName(options.src, MAX_PATH, source_path, &name);
		m_XmlFilenamesComboBox.SetWindowText(source_path);
		if (name)
			name[-1] = '\0';
	}
	else
	{
		m_XmlFilenamesComboBox.AddString("All XML files");
		m_XmlFilenamesComboBox.SetCurSel(0);
	}

	if (!options.dest.IsEmpty())
	{
		GetFullPathName(options.dest, MAX_PATH, dest_path, NULL);
	}
	else if (m_Quiet)
	{
		strcpy(dest_path, app_path);
		strcat(dest_path, "\\DATA");
	}

	m_SourcePath = source_path;
	m_DestinationPath = dest_path;

	SetDlgItemText(IDC_SOURCE_PATH,		 m_SourcePath);
	SetDlgItemText(IDC_DESTINATION_PATH, m_DestinationPath);

	CheckDlgButton(IDC_RECURSIVE,		  TRUE);
	CheckDlgButton(IDC_CREATE_SUBFOLDERS, options.subfolders);

	if (m_Quiet)
	{
		OnConvert();
		EndDialog(0);
	}

	return TRUE;  // return TRUE  unless you set the focus to a control
}


BOOL XMLUTFDlg::DestroyWindow()
{
	// Make sure there is no conversion in progress
	if (m_ConversionInProgress)
	{
		// Tell the conversion to abort
		m_ConversionAborted = true;

		// Wait up to 5 seconds for the conversion to abort
		int count = 50; // five seconds
		while (m_ConversionInProgress && (count > 0))
		{
			// Wait 100 ms
			Sleep(100);
			count--;
		}
	}

	delete m_XmlDataBuffer;
	delete m_string;

	// We are done with the critical section mutex
	DeleteCriticalSection(&m_Mutex);

	// Tell MFC to destroy this dialog
	return CDialog::DestroyWindow();
}


void XMLUTFDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialog::OnSysCommand(nID, lParam);
	}
}


// If you add a minimize button to your dialog, you will need the code below
//	to draw the icon.  For MFC applications using the document/view model,
//	this is automatically done for you by the framework.

void XMLUTFDlg::OnPaint() 
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


HCURSOR XMLUTFDlg::OnQueryDragIcon()
{
	return (HCURSOR) m_hIcon;
}


void XMLUTFDlg::OnBrowseXmlFilename() 
{
	UpdateData();

	CString path = m_XmlFilenames;
	if (m_XmlFilenames.Left(4) == "All ")
	{
		path = m_SourcePath + "\\*.xml";
	}

	CFileDialog dlg(true, "All XML Files", path,
		OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT | OFN_NOCHANGEDIR,
		"All XML Files|*.xml||");
	dlg.m_ofn.lStructSize = 0x58;

	if (dlg.DoModal() == IDOK)
	{
		m_XmlFilenames = dlg.GetPathName();
		m_XmlFilenamesComboBox.SetWindowText(m_XmlFilenames);
		CFile f;
		f.SetFilePath(m_XmlFilenames);
		CString path = f.GetFilePath();
		CString name = f.GetFileName();
		m_SourcePath =	path.Left(path.GetLength() - name.GetLength() - 1);
		SetDlgItemText(IDC_SOURCE_PATH, m_SourcePath);
	}
}


void XMLUTFDlg::OnCancel() 
{
	CDialog::OnCancel();
}


void XMLUTFDlg::OnHelpButton() 
{
	// TODO: Display help
}


void XMLUTFDlg::OnSelchangeXmlFilenames() 
{
	m_XmlFilenamesComboBox.GetWindowText(m_XmlFilenames);
}


void XMLUTFDlg::OnConvert()
{
	UpdateData(TRUE);

	m_Recursive		   = IsDlgButtonChecked(IDC_RECURSIVE);
	m_CreateSubfolders = IsDlgButtonChecked(IDC_CREATE_SUBFOLDERS);

	if (!SetCurrentDirectory(m_SourcePath))
	{
		MessageBox("Source Path is invalid", "Error", MB_ICONERROR);
		return;
	}

	char source_path[MAX_PATH];
	GetCurrentDirectory(sizeof(source_path), source_path);
	char *data = stristr(source_path, "DATA");
	if (data && (data == source_path || data[-1] == '\\') &&
				(data[4] == '\\' || !data[4]))
	{
		m_PathTrim = data + 5 - source_path;
	}
	else
	{
		m_PathTrim = strlen(source_path) + 1;
	}

	CreateAndSetPath(m_DestinationPath);
	GetCurrentDirectory(sizeof(m_CurrentDestinationPath), m_CurrentDestinationPath);

	// Open the log file
	char temppath[MAX_PATH];
	GetTempPath(MAX_PATH, temppath);
	SetCurrentDirectory(temppath);
	m_LogFile = fopen("XMLUTF.log", "w");

	if (!m_LogFile)
	{
		MessageBox("Unable to create XMLUTF.log in temporary folder!", "Error", MB_ICONERROR);
		return;
	}

	fprintf(m_LogFile, "%s\n\n", XMLUTF_VERSION);
	fprintf(m_LogFile, "Source path: %s\n", source_path);
	fprintf(m_LogFile, "Destination path: %s\n", m_CurrentDestinationPath);

	if (!m_Quiet)
	{
		REGSAM sam = KEY_READ + KEY_WRITE;
		HKEY hKey = NULL;
		RegOpenKeyEx( HKEY_LOCAL_MACHINE, FREELANCER_KEYPATH, 0, sam, &hKey );
		RegSetValueEx(hKey, "XMLUTF SourcePath", 0, REG_SZ, (BYTE *) source_path, strlen(source_path));
		RegSetValueEx(hKey, "XMLUTF DestPath", 0, REG_SZ, (BYTE *) m_CurrentDestinationPath, strlen(m_CurrentDestinationPath));
		RegCloseKey( hKey );
	}

	SetCurrentDirectory(m_SourcePath);

	// Initialize variables
	if (!m_XmlDataBuffer)
	{
		m_XmlDataBuffer = new char[XML_BUFFER_SIZE];
	}
	m_UtfFile = NULL;
	m_Depth = 0;
	m_ConversionAborted = false;
	m_ConversionInProgress = true;
	m_CountingXmlFiles = false;
	m_XmlFileCount = 0;
	m_NumXmlFilesOpened = 0;
	m_NumUtfFilesCreated = 0;
	m_UtfNodeCount = 0;
	m_string = NULL;
	m_strcap = 0;
	m_AnimHeader = NULL;
	m_mtxidx = 0;

	// Did the user specify a file name?
	bool all = (m_XmlFilenames.Left(4) == "All ");
	if (all)
	{
		ShowWindow(SW_HIDE);

		// Launch the background conversion thread
		CWinThread *winthread = AfxBeginThread(BackgroundConversionThread, this, THREAD_PRIORITY_BELOW_NORMAL);

		// Display the progress bar
		ProgressDialog progress(this);
		progress.DoModal();
	}
	else
	{
		// Process a single file
		BeginWaitCursor();
		LPCSTR filename;
		filename = strrchr(m_XmlFilenames, '\\');
		if (!filename++)
		{
			filename = (LPCSTR) m_XmlFilenames;
		}
		strcpy(m_CurrentSourcePath, m_SourcePath);
		ProcessXmlFile(filename);
		EndWaitCursor();
	}

	if (m_ConversionAborted)
	{
		Log("\nConversion Aborted\n");
		MessageBox("Conversion Aborted", "XML to UTF", MB_ICONINFORMATION);
	}
	else
	{
		Log("\n------------------------------------------------------------\n");
		// Display the conversion summary
		CString msg;
		msg.Format("Examined %d XML files.\n"
				   "Created %d UTF files.",
				   m_NumXmlFilesOpened,
				   m_NumUtfFilesCreated);
		Log("%s\n", (LPCSTR) msg);
		if (!m_Quiet)
			MessageBox(msg, "XML to UTF Conversion Results", MB_ICONINFORMATION);
	}

	if (m_LogFile)
	{
		fclose(m_LogFile);
		m_LogFile = NULL;
	}

	if (all)
	{
		ShowWindow(SW_NORMAL);
	}
}


UINT XMLUTFDlg::RunBackgroundConversionThread()
{
	// Count all XML files in the folder
	m_CountingXmlFiles = true;
	ProcessFolder(".");

	// Update the file count for the progress bar
	EnterCriticalSection(&m_Mutex);
	m_XmlFileCount = m_NumXmlFilesOpened;
	m_NumXmlFilesOpened = 0;
	LeaveCriticalSection(&m_Mutex);

	// Process all files in the folder
	m_CountingXmlFiles = false;
	ProcessFolder(".");

	// Set the completion indicator
	m_ConversionInProgress = false;

	return (0);
}


void XMLUTFDlg::ProcessFolder(LPCSTR folder)
{
	// Save the current source path
	char current_source_path[MAX_PATH];
	GetCurrentDirectory(sizeof(current_source_path), current_source_path);

	// Update the current source path
	EnterCriticalSection(&m_Mutex);
	strcpy(m_CurrentSourcePath, current_source_path);
	LeaveCriticalSection(&m_Mutex);

	// Process all files in this directory
	WIN32_FIND_DATA fdata;
	HANDLE h = FindFirstFile("*", &fdata);
	do
	{
		if (m_ConversionAborted)
		{
			break;
		}

		int len = strlen(fdata.cFileName);
		if (fdata.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
		{
			// This is a directory
			if (m_Recursive && (fdata.cFileName[0] != '.'))
			{
				// Ignore included directories
				if (fdata.cFileName[len-1] == '_' ||
					(len > 4 && fdata.cFileName[len-4] == '_' &&
					 (STRIEQ(fdata.cFileName+len-3, "3db") ||
					  STRIEQ(fdata.cFileName+len-3, "anm") ||
					  STRIEQ(fdata.cFileName+len-3, "cmp") ||
					  STRIEQ(fdata.cFileName+len-3, "dfm") ||
					  STRIEQ(fdata.cFileName+len-3, "mat") ||
					  STRIEQ(fdata.cFileName+len-3, "txm") ||
					  STRIEQ(fdata.cFileName+len-3, "utf") ||
					  STRIEQ(fdata.cFileName+len-3, "vms"))))
				{
					continue;
				}
				// Do recursive processing on the subfolder
				SetCurrentDirectory(fdata.cFileName);
				ProcessFolder(fdata.cFileName);
				SetCurrentDirectory("..");

				// Update the current source path
				EnterCriticalSection(&m_Mutex);
				strcpy(m_CurrentSourcePath, current_source_path);
				LeaveCriticalSection(&m_Mutex);
			}
		}
		else
		{
			if (len > 4 && STRIEQ(fdata.cFileName+len-4, ".xml"))
			{
				ProcessXmlFile(fdata.cFileName);
				SetCurrentDirectory(current_source_path);
			}
		}
	} while (FindNextFile(h, &fdata));
}


void XMLUTFDlg::ProcessXmlFile(LPCSTR filename)
{
	m_XmlFile = fopen(filename, "r");
	if (m_XmlFile)
	{
		// Initialize variables
		m_XmlLineNumber = 0;
		m_UtfNodeCount = 0;
		m_AbortXmlFileProcessing = false;
		m_ExporterVersion = NULL;

		// The XML signature must be at the top
		ReadXML();
		if (!STREQ(m_XmlDataPtr, "<?xml version=\"1.0\" encoding=\"ISO-8859-1\"?>\n"))
		{
			// Ignore XML files which do not have the XML signature at the top
			fclose(m_XmlFile);
			m_XmlFile = NULL;
			return;
		}

		// The UTFXML record must be next, and must contain a filename
		ReadXML();
		SkipSpaces();
		if ((strstr(m_XmlDataPtr, "<UTFXML ") == 0) ||
			(strstr(m_XmlDataPtr, "\">") == 0))
		{
			// Ignore XML files which do not have the UTFXML signature.
			fclose(m_XmlFile);
			m_XmlFile = NULL;
			return;
		}

		// Increment the counter
		m_NumXmlFilesOpened++;

		if (m_CountingXmlFiles)
		{
			fclose(m_XmlFile);
			m_XmlFile = NULL;
			return;
		}

		// Start a new log entry
		Log("\n-------------------------------------------------------------\n"
			"Processing \"%s\"\n", filename);

		char *utf_filename = GetXmlAttr("filename");
		if (!utf_filename)
		{
			Log("   Error: Expecting filename on line %d\n", m_XmlLineNumber);
			fclose(m_XmlFile);
			m_XmlFile = NULL;
			return;
		}

		char *utf_path = GetXmlAttr("path");

		m_StringFirst = false;
		char *string_first = GetXmlAttr("stringfirst");
		if (string_first)
		{
			if (STREQ(string_first, "true"))
			{
				// Setting stringfirst="true" will cause the string
				// segment to be put before the tree segment.
				m_StringFirst = true;
			}
			delete string_first;
		}

		m_PrePadData = false;
		char *prepaddata = GetXmlAttr("prepaddata");
		if (prepaddata)
		{
			if (STREQ(prepaddata, "true"))
			{
				// Setting prepaddata="true" will cause the Data Segment
				// to be preceeded with the DWORD 0xFFFFFFFF
				m_PrePadData = true;
			}
			delete prepaddata;
		}

		DWORD unk[3];
		FILETIME ft;
		unk[0] = GetHexAttr("unk1");
		unk[1] = GetHexAttr("unk2");
		unk[2] = GetHexAttr("unk3");
		if (!GetFileTimeAttr("time", ft))
		{
			SYSTEMTIME st;
			GetLocalTime(&st);
			SystemTimeToFileTime(&st, &ft);
		}
		WORD d, t;
		FileTimeToDosDateTime(&ft, &d, &t);
		m_time1 = m_time2 = m_time3 = d | (t << 16);

		// Move to the third line in the XML file
		ReadXML();

		// Start at the base of the destination path
		SetCurrentDirectory(m_DestinationPath);

		// If the user wants to create subfolders
		// and a path is specified
		if (m_CreateSubfolders && utf_path)
		{
			// Create the folder(s) and set the current directory
			if (strnicmp(utf_path, "DATA\\", 5) == 0)
			{
				utf_path += 5;
			}
			CreateAndSetPath(utf_path);
		}

		// Create the UTF Root
		m_UtfRoot = new UTFNodeEntry;
		m_UtfNodeCount++;
		memset(m_UtfRoot, 0, sizeof(UTFNodeEntry));
		m_UtfRoot->name = new char[2];
		strcpy(m_UtfRoot->name, "\\");

		ProcessXmlBranch(m_UtfRoot);

		if (m_AbortXmlFileProcessing)
		{
			Log("   XML to UTF Conversion aborted\n");
		}
		else if (!STREQ(m_XmlDataPtr, "</UTFXML>\n"))
		{
			Log("   Error parsing XML at line %d : Expected </UTFXML> signature\n", m_XmlLineNumber);
		}
		else
		{
			WriteUtfFile(utf_filename, unk, &ft);
		}

		fclose(m_XmlFile);
		m_XmlFile = NULL;

		DestroyTree(m_UtfRoot);
		m_UtfRoot = NULL;
		RELEASE_ATTR(utf_path)
		RELEASE_ATTR(utf_filename)
		RELEASE_ATTR(m_ExporterVersion)
	}
}


// Open an include file.
void XMLUTFDlg::OpenIncludeFile(LPCSTR filename)
{
	char tmp_filename[MAX_PATH];
	sprintf(tmp_filename, "%s\\%s", m_CurrentSourcePath, filename);
	m_XmlFile = fopen(tmp_filename, "r");
	if (m_XmlFile)
	{
		// Initialize variables
		m_XmlLineNumber = 0;

		// The XML signature must be at the top
		ReadXML();
		if (!STREQ(m_XmlDataPtr, "<?xml version=\"1.0\" encoding=\"ISO-8859-1\"?>\n"))
		{
			// Ignore XML files which do not have the XML signature at the top
			fclose(m_XmlFile);
			m_XmlFile = NULL;
			return;
		}
		// Skip the comment
		ReadXML();
		SkipSpaces();

		// Skip the opening tag (already read as part of the include)
		ReadXML();
	}
}


// Reads a single record from the XML file
void XMLUTFDlg::ReadXML()
{
	m_XmlDataBuffer[0] = 0;
	if (fgets(m_XmlDataBuffer, XML_BUFFER_SIZE, m_XmlFile))
	{
		m_XmlLineNumber++;
	}

	m_XmlDataPtr = m_XmlDataBuffer;

	for (;;)
	{
		// Skip leading spaces
		while (*m_XmlDataPtr == ' ' || *m_XmlDataPtr == '\t')
		{
			m_XmlDataPtr++;
		}

		// Is it a comment?
		if (strncmp(m_XmlDataPtr, "<!--", 4))
		{
			// No, assume it is a tag
			return;
		}

		// Find the end of the XML comment
		m_XmlDataPtr += 4;
		int start = m_XmlLineNumber;
		char *p;
		while (!(p = strstr(m_XmlDataPtr, "-->")))
		{
			m_XmlDataPtr = m_XmlDataBuffer;
			m_XmlDataBuffer[0] = 0;
			if (fgets(m_XmlDataBuffer, XML_BUFFER_SIZE, m_XmlFile))
			{
				m_XmlLineNumber++;
			}
			else
			{
				Log("   Error: Unterminated comment.  Line %d\n", start);
				return;
			}
		}

		// Move to the end of the comment
		m_XmlDataPtr = p+3;
	}
}


void XMLUTFDlg::SkipSpaces()
{
	while (*m_XmlDataPtr == ' ' || *m_XmlDataPtr == '\t')
	{
		m_XmlDataPtr++;
	}

	// Reached end of line, start on the next
	while (((*m_XmlDataPtr == '\n') || (*m_XmlDataPtr == 0)) && !feof(m_XmlFile))
	{
		ReadXML();
	}
}


void XMLUTFDlg::SkipXmlTag()
{
	char *p = strchr(m_XmlDataPtr, '>');
	if (p)
	{
		m_XmlDataPtr = p + 1;
	}

	// Skip trailing spaces
	SkipSpaces();
}


char *XMLUTFDlg::GetXmlName()
{
	char *name = NULL;
	char *p = strchr(m_XmlDataPtr, '<');
	if (p)
	{
		char *end = strpbrk(++p, "\t >");
		if (end)
		{
			if (*end == '>' && end[-1] == '/')
			{
				--end;
			}
			int len = end - p;
			name = new char[len+1];
			memcpy(name, p, len);
			name[len] = 0;
		}
		else
		{
			Log("   Error parsing XML name on line %d\n", m_XmlLineNumber);
		}
	}

	return (name);
}


char *XMLUTFDlg::GetXmlAttr(char *attr)
{
	char *value = NULL;
	char what[256];
	int len = sprintf(what, " %s=\"", attr);
	char *p = strstr(m_XmlDataPtr, what);
	if (p)
	{
		p += len;
		char *end = strchr(p, '"');
		if (end)
		{
			len = end - p;
			value = new char[len+1];
			memcpy(value, p, len);
			value[len] = 0;
		}
		else
		{
			Log("   Error parsing XML attribute \"%s\" on line %d\n", attr, m_XmlLineNumber);
		}
	}

	return (value);
}


bool XMLUTFDlg::IsTag()
{
	BYTE b;
	if (GetByte(&b))
	{
		m_XmlDataPtr--;
		return false;
	}
	return true;
}


bool XMLUTFDlg::IsTag(LPCSTR tag, bool fail /* = false */)
{
	if (IsTag())
	{
		int len = strlen(tag);
		if (strncmp(m_XmlDataPtr+1, tag, len) == 0 &&
			(m_XmlDataPtr[1+len] == ' '  ||
			 m_XmlDataPtr[1+len] == '\t' ||
			 m_XmlDataPtr[1+len] == '>'))
		{
			return true;
		}
	}
	if (fail)
	{
		Log("   Error: expecting %s on line %d\n", tag, m_XmlLineNumber);
		m_AbortXmlFileProcessing = true;
	}
	return false;
}


void XMLUTFDlg::WriteUtfFile(LPCSTR filename, DWORD *unk, FILETIME *ft)
{
	// Initialize variables
	m_UtfNodeIndex = 0;
	m_UtfStringCount = 0;
	m_UtfStringSize = 0;
	m_UtfDataOffset = (m_PrePadData) ? 4 : 0;

	// Determine how many nodes we need
	DWORD nodes = m_UtfNodeCount;

	// Create arrays for the nodes and strings
	m_UtfStringArray = new char *[m_UtfNodeCount+1];
	m_UtfStringOffset = new int[m_UtfNodeCount+1];
	m_UtfNode = new UTFNode[nodes];
	memset(m_UtfNode, 0, sizeof(UTFNode) * nodes);

	// Add a null string at the beginning of the list
	AddUtfString("");

	// Populate the nodes and string arrays
	ProcessUtfNode(m_UtfRoot);

	// Create UTF Header
	UTFHeader header;
	memcpy(header.UTF, "UTF ", 4);
	header.version = 0x0101;
	header.node_size = 44;
	header.tree_size = nodes * sizeof(UTFNode);
	header.string_size = m_UtfStringSize;
	header.string_alloc = (header.string_size + 3) & ~3;
	
	// If "stringfirst" attribute was set, put the String Segment before the Tree Segment
	if (m_StringFirst)
	{
		// The String segment follows the header
		header.string_offset = sizeof(UTFHeader);
		// The Tree segement is next
		header.tree_offset = header.string_offset + header.string_alloc;
		// The Data segment is always last
		header.data_offset = header.tree_offset + header.tree_size;
	}
	else
	{
		// The Tree segment follows the header
		header.tree_offset = sizeof(UTFHeader);
		// The String segement is next
		header.string_offset = header.tree_offset + header.tree_size;
		// The Data segment is always last
		header.data_offset = header.string_offset + header.string_alloc;
	}
	header.unk1 = unk[0];
	header.unk2 = unk[1];
	header.unk3 = unk[2];
	header.timestamp = *ft;

	if (m_ExporterVersion)
	{
		Log("   Exporter Version = \"%s\"\n", m_ExporterVersion);
	}

	// Write the Header, Tree, Strings, and Data segments
	m_UtfFile = fopen(filename, "wb");
	if (m_UtfFile)
	{
		// Write the UTF Header
		fwrite(&header, sizeof(header), 1, m_UtfFile);
		
		if (!m_StringFirst)
		{
			// Write the UTF Tree Segment
			fwrite(m_UtfNode, header.tree_size, 1, m_UtfFile);
		}
		
		// Write the UTF String Segment
		for (int i = 0; i < m_UtfStringCount; i++)
		{
			fwrite(m_UtfStringArray[i], strlen(m_UtfStringArray[i])+1, 1, m_UtfFile);
		}

		// Add padding after the String segment
		int padding_size = header.string_alloc - header.string_size;
		if (padding_size > 0)
		{
			DWORD padding = 0;
			fwrite(&padding, padding_size, 1, m_UtfFile);
		}

		if (m_StringFirst)
		{
			// Write the UTF Tree Segment
			fwrite(m_UtfNode, header.tree_size, 1, m_UtfFile);
		}

		if (m_PrePadData)
		{
			DWORD prepad = 0xFFFFFFFF;
			fwrite(&prepad, 4, 1, m_UtfFile);
		}

		// Write the UTF Data Segment
		WriteUtfData(m_UtfRoot);

		// Done!
		m_UtfFileSize = ftell(m_UtfFile);
		fclose(m_UtfFile);

		Log("   Created UTF file '%s' with %d nodes, size=%d\n", filename, m_UtfNodeCount, m_UtfFileSize);
		m_NumUtfFilesCreated++;
	}

	// Free the memory allocated for arrays
	m_StrMap.clear();
	delete m_UtfStringArray;
	delete m_UtfStringOffset;
	delete m_UtfNode;
}


// Returns the offset in the string segment
int XMLUTFDlg::AddUtfString(char *string)
{
	DWORD hash = CreateID(string);

	// See if the string is already in the list
	StrIter iter = m_StrMap.find(hash);
	if (iter != m_StrMap.end())
	{
		// The string is already in the list
		// No need to add a duplicate copy
		return (m_UtfStringOffset[iter->second]);
	}

	// Add the string to the end of the list
	int index = m_UtfStringCount++;
	m_StrMap[hash] = index;
	m_UtfStringArray[index] = string;
	m_UtfStringOffset[index] = m_UtfStringSize;
	m_UtfStringSize += strlen(string) + 1;

	return (m_UtfStringOffset[index]);
}


// This recursive procedure climbs the UTF tree
int XMLUTFDlg::ProcessUtfNode(UTFNodeEntry *node)
{
	UTFNode *previous = NULL;
	int branch_offset = 0;

	while (node)
	{
		int index = m_UtfNodeIndex++;
		UTFNode *utf = &m_UtfNode[index];
		utf->name = AddUtfString(node->name);
		utf->flags = 0;
		utf->next = 0;
		if (node->data_size)
		{
			utf->alloc_size = node->data_alloc;
			utf->size1 = 
			utf->size2 = node->data_size;
			utf->branch = m_UtfDataOffset;
			utf->flags = LEAF;
			m_UtfDataOffset += node->data_alloc;
		}

		// Save the unknowns
		utf->unk = node->unk;
		utf->time1 = node->time1;
		utf->time2 = node->time2;
		utf->time3 = node->time3;

		// Process the branch nodes
		if (node->child)
		{
			utf->flags |= BRANCH;
			utf->branch = ProcessUtfNode(node->child);
		}

		int offset = index * sizeof(UTFNode);

		if (!branch_offset)
		{
			branch_offset = offset;
		}

		if (previous)
		{
			previous->next = offset;
		}

		previous = utf;
		node = node->next;
	}

	return (branch_offset);
}


// This recursive procedure writes the pieces of the UTF Data Segment
void XMLUTFDlg::WriteUtfData(UTFNodeEntry *node)
{
	while (node)
	{
		int index = m_UtfNodeIndex++;
		UTFNode *utf = &m_UtfNode[index];
		if (node->data_size)
		{
			// Write the data for this node
			if (node->data)
			{
				// We already have the data in memory
				fwrite(node->data, node->data_alloc, 1, m_UtfFile);
			}
			else if (node->data_filename)
			{
				// We need to read the data from a file
				FILE *f = fopen(node->data_path, "rb");
				if (f)
				{
					BYTE *data = new BYTE[node->data_alloc];
					memset(data, 0, node->data_alloc);
					// Read the data from the data file, then write it to the UTF file
					if (fread(data, node->data_size, 1, f) != 1)
					{
						Log("   Error reading data from '%s'\n", node->data_filename);
					}
					fclose(f);
					if (fwrite(data, node->data_alloc, 1, m_UtfFile) != 1)
					{
						Log("   Error writing data to UTF file\n");
					}
					delete data;
				}
				else
				{
					Log("   Error opening file '%s'\n", node->data_filename);
				}
			}
			else
			{
				// No data for this leaf!
				Log("   Error: gone insane writing UTF data\n");
			}
		}

		// Process the branch nodes
		if (node->child)
		{
			WriteUtfData(node->child);
		}

		node = node->next;
	}
}


void XMLUTFDlg::CreateAndSetPath(const char * path)
{
	if (SetCurrentDirectory(path))
		return;

	char dir[MAX_PATH];
	char *folder = dir;
	strcpy(dir, path);
	// Skip over the root.
	if (*folder == '\\')
	{
		++folder;
		SetCurrentDirectory("\\");
	}
	else if (folder[1] == ':' && folder[2] == '\\')
	{
		folder += 3;
		char save = *folder;
		*folder = '\0';
		SetCurrentDirectory(dir);
		*folder = save;
	}
	folder = strtok(folder, "\\");
	while (folder && *folder)
	{
		CreateDirectory(folder, NULL);
		SetCurrentDirectory(folder);
		folder = strtok(NULL, "\\");
	}
}


// This procedure recursively destroys the branches on a tree
void XMLUTFDlg::DestroyTree(UTFNodeEntry * branch)
{
	while (branch)
	{
		UTFNodeEntry * next = branch->next;
		if (branch->child)
		{
			DestroyTree(branch->child);
		}
		if (branch->data)
		{
			delete branch->data;
		}
		if (branch->data_filename)
		{
			delete branch->data_filename;
		}
		if (branch->data_path)
		{
			delete branch->data_path;
		}
		if (branch->name)
		{
			delete branch->name;
		}
		delete branch;
		branch = next;
	}
}


void XMLUTFDlg::Log(char *format, ...)
{
	char buffer[512];

	va_list args;
	va_start(args, format);
	vsprintf(buffer, format, args);
	va_end(args);

	TRACE(buffer);

	if (m_LogFile)
	{
		fprintf(m_LogFile, buffer);
	}
}


void XMLUTFDlg::ProcessXmlBranch(UTFNodeEntry * pParentNode)
{
	UTFNodeEntry * node = NULL;
	UTFNodeEntry * last_branch = NULL;
	FILE * old_file;
	int old_line_number;
	char *old_data_ptr;
	char *old_data_buffer;

	while (!m_AbortXmlFileProcessing)
	{
		char *xml_name = GetXmlName();
		char *name = xml_name;
		if (xml_name[0] == '/')
		{
			RELEASE_ATTR(xml_name);
			break;
		}

		char *alt_name = GetXmlAttr("name");
		if (alt_name)
		{
			name = alt_name;
		}
		else
		{
			alt_name = GetXmlAttr("hash");
			if (alt_name)
			{
				DWORD crc = CreateID(alt_name);
				delete alt_name;
				name = alt_name = new char[16];
				sprintf(alt_name, "0x%08X", crc);
			}
		}

		if (STREQ(name, "UTF_ROOT"))
		{
			node = pParentNode;
		}
		else
		{
			node = new UTFNodeEntry;
			m_UtfNodeCount++;
			memset(node, 0, sizeof(UTFNodeEntry));
			node->name = new char[strlen(name)+1];
			strcpy(node->name, name);

			if (last_branch)
			{
				last_branch->next = node;
			}
			else
			{
				pParentNode->child = node;
			}
		}

		node->unk = GetHexAttr("unk1");

		node->time1 = m_time1;
		node->time2 = m_time2;
		node->time3 = m_time3;
		DWORD t = GetTimeAttr("time");
		if (t)
		{
			node->time1 =
			node->time2 =
			node->time3 = t;
		}
		else
		{
			t = GetTimeAttr("time1");
			if (t) node->time1 = t;
			t = GetTimeAttr("time2");
			if (t) node->time2 = t;
			t = GetTimeAttr("time3");
			if (t) node->time3 = t;
		}

		char *type = GetXmlAttr("type");
		if (type)
		{
			// This is a Leaf node
			m_LeafNode = node;

			ProcessXmlLeaf(xml_name, name, type);

			RELEASE_ATTR(type);
		}
		else
		{
			// This is a Branch node
			char *include = GetXmlAttr("include");
			// Is this a branch with no leaves?
			if (strstr(m_XmlDataPtr, "/>") && !include)
			{
				// TRACE("Branch with no leaves: \"%s\"\n", name);
			}
			else
			{
				if (include)
				{
					old_file = m_XmlFile;
					old_line_number = m_XmlLineNumber;
					old_data_ptr = m_XmlDataPtr;
					old_data_buffer = new char[strlen(m_XmlDataBuffer)+1];
					strcpy(old_data_buffer, m_XmlDataBuffer);
					OpenIncludeFile(include);
					if (!m_XmlFile)
					{
						Log("   Error: invalid include file '%s' on line %d\n", include, old_line_number);
						m_AbortXmlFileProcessing = true;
						goto done_include;
					}
					else
					{
						Log("   Processing \"%s\"\n", include);
					}
				}
				else
				{
					// Move to the end of the XML tag
					SkipXmlTag();
				}


				DWORD time1, time2, time3;
				time1 = m_time1;
				time2 = m_time2;
				time3 = m_time3;
				m_time1 = node->time1;
				m_time2 = node->time2;
				m_time3 = node->time3;

				// Process whatever is below the branch
				ProcessXmlBranch(node);

				m_time1 = time1;
				m_time2 = time2;
				m_time3 = time3;

				if (include)
				{
					fclose(m_XmlFile);
				done_include:
					m_XmlFile = old_file;
					m_XmlLineNumber = old_line_number;
					m_XmlDataPtr = old_data_ptr;
					strcpy(m_XmlDataBuffer, old_data_buffer);
					delete old_data_buffer;
					delete include;
				}
			}
		}

		// Save the last branch for the linked list
		last_branch = node;

		if (strstr(m_XmlDataPtr, "/>"))
		{
			// Skip the terminator
			SkipXmlTag();
		}
		else if (!m_AbortXmlFileProcessing)
		{
			char terminator[256];
			sprintf(terminator, "</%s>", xml_name);
			if (strstr(m_XmlDataPtr, terminator)==0)
			{
				Log("   Error: Expecting %s on line %d\n", terminator, m_XmlLineNumber);
				m_AbortXmlFileProcessing = true;
				return;
			}
			// Skip the terminator
			SkipXmlTag();
		}

		RELEASE_ATTR(xml_name);
		RELEASE_ATTR(alt_name);
	}
}


void XMLUTFDlg::ProcessXmlLeaf(char *xml_name, char *name, char *type)
{
	// Is the data stored in a file?
	if (STREQ(type, "file"))
	{
		char *data_filename = GetXmlAttr("filename");
		if (!data_filename)
		{
			Log("   Error: type=\"file\" requires filename on line %d\n", m_XmlLineNumber);
			m_AbortXmlFileProcessing = true;
		}

		if (strstr(m_XmlDataPtr, "\"/>") == 0)
		{
			Log("   Warning: type=\"file\" should be followed by /> on line %d\n", m_XmlLineNumber);
		}

		// Open the data file to determine its size
		char tmp_filename[MAX_PATH];
		sprintf(tmp_filename, "%s\\%s", m_CurrentSourcePath, data_filename);
		FILE *f = fopen(tmp_filename, "rb");
		if (!f)
		{
			Log("   Error: Unable to open file '%s' (Line %d)\n", data_filename, m_XmlLineNumber);
			m_AbortXmlFileProcessing = true;
			delete data_filename;
			return;
		}

		// Determine the file size
		fseek(f, 0, SEEK_END);
		DWORD file_size = ftell(f);
		fclose(f);

		// Save the filename for later processing
		m_LeafNode->data_filename = data_filename;
		m_LeafNode->data_size	  = file_size;
		m_LeafNode->data_alloc	  = (file_size + 3) & ~3;
		m_LeafNode->data_path	  = new char[strlen(tmp_filename)+1];
		strcpy(m_LeafNode->data_path, tmp_filename);

		// Note: Don't release data_filename.
		// It will be released when the tree is destroyed
		return;
	}

	// Fetch the size, if available
	int size = 0;
	char *size_str = GetXmlAttr("size");
	if (size_str)
	{
		size = atoi(size_str);
		RELEASE_ATTR(size_str);
	}
	else
	{
		size_str = GetXmlAttr("count");
		if (size_str)
		{
			size = atoi(size_str);
			RELEASE_ATTR(size_str);
		}
	}

	// These nodes have attributes
	if (STREQ(type, "VMeshData"))
	{
		ProcessVMeshData();
	}
	else if (STREQ(type, "ALEffectLib"))
	{
		ProcessALEffectLib();
	}
	else if (STREQ(type, "AlchemyNodeLibrary"))
	{
		ProcessAlchemyNodeLibrary();
	}
	else
	{
		// The data follows the XML tag
		SkipXmlTag();

		if (STREQ(type, "text"))
		{
			ProcessText();
			if (STRIEQ(name, "exporter version"))
			{
				m_ExporterVersion = new char[m_LeafNode->data_size];
				strcpy(m_ExporterVersion, (char *) m_LeafNode->data);
			}
		}
		else if (STREQ(type, "byte"))
		{
			ProcessByte();
		}
		else if (STREQ(type, "short array"))
		{
			ProcessShort();
		}
		else if (STREQ(type, "int"))
		{
			ProcessInt(1);
		}
		else if (STREQ(type, "int array"))
		{
			ProcessInt();
		}
		else if (STREQ(type, "float"))
		{
			ProcessFloat(1);
		}
		else if (STREQ(type, "float array"))
		{
			ProcessFloat();
		}
		else if (STREQ(type, "RGB"))
		{
			ProcessRGB();
		}
		else if (STREQ(type, "Vector"))
		{
			ProcessFloat(3);
		}
		else if (STREQ(type, "Matrix"))
		{
			ProcessFloat(9);
		}
		else if (STREQ(type, "Transform"))
		{
			ProcessFloat(12);
		}
		else if (STREQ(type, "hex"))
		{
			ProcessHex(size);
		}
		else if (STREQ(type, "VWireData"))
		{
			ProcessVWireData();
		}
		else if (STREQ(type, "VMeshRef"))
		{
			ProcessVMeshRef();
		}
		else if (STREQ(type, "Header"))
		{
			ProcessHeader();
		}
		else if (STREQ(type, "Frames"))
		{
			ProcessFrames();
		}
		else if (STREQ(type, "Fix"))
		{
			ProcessFix();
		}
		else if (STREQ(type, "Rev"))
		{
			ProcessRev();
		}
		else if (STREQ(type, "Cyl"))
		{
			ProcessCyl();
		}
		else if (STREQ(type, "Sphere"))
		{
			ProcessSphere();
		}
		else if (STREQ(type, "Frame_rects"))
		{
			ProcessFrame_rects();
		}
		else
		{
			Log("   Error: Unrecognized type \"%s\"\n", type);
		}
	}

	if (!m_AbortXmlFileProcessing)
	{
		char terminator[256];
		sprintf(terminator, "</%s>", xml_name);
		if (strstr(m_XmlDataPtr, terminator) == 0)
		{
			Log("   Error: Expecting %s on line %d\n", terminator, m_XmlLineNumber);
			m_AbortXmlFileProcessing = true;
		}
	}
}


void XMLUTFDlg::ProcessText()
{
	char *text = m_XmlDataPtr;
	int size = strcspn(m_XmlDataPtr, "<\n");
	m_XmlDataPtr += size;
	if (*m_XmlDataPtr != '<')
	{
		Log("   Warning: Multi-line type=\"text\" not supported.  Line %d\n", m_XmlLineNumber);
	}
	if (*text == '"')
	{
		++text;
		--size;
		if (m_XmlDataPtr[-1] == '"')
		{
			--size;
		}
		else
		{
			Log("   Warning: Unterminated string or '<' within string.  Line %d\n", m_XmlLineNumber);
		}
	}
	++size; 	// include the null terminator

	DWORD alloc = (size + 3) & ~3;
	char *data = new char[alloc];
	memset(data, 0, alloc);
	memcpy(data, text, size-1);

	// Append the text to the leaf
	m_LeafNode->data_size  = size;
	m_LeafNode->data_alloc = alloc;
	m_LeafNode->data = (BYTE *) data;
}


void XMLUTFDlg::ProcessByte()
{
	DWORD * data = new DWORD;
	m_LeafNode->data_size  = 1;
	m_LeafNode->data_alloc = 4;
	m_LeafNode->data = (BYTE *) data;

	*data = GetBYTE();

	BYTE junk;
	if (GetByte(&junk))
	{
		Log("   Warning: Unexpected data following byte at %d\n", m_XmlLineNumber);
	}
}


void XMLUTFDlg::ProcessShort()
{
	Store data;

	while (!IsTag())
	{
		put(data, GetWORD());
	}

	m_LeafNode->data_size  = data.size();
	m_LeafNode->data_alloc = data.align();
	m_LeafNode->data = (BYTE *) data.data();
}


void XMLUTFDlg::ProcessInt(int size)
{
	DWORD * data = new DWORD[size];
	m_LeafNode->data_size  =
	m_LeafNode->data_alloc = size * sizeof(DWORD);
	m_LeafNode->data = (BYTE *) data;

	for (int i = 0; i < size; i++)
	{
		data[i] = GetDWORD();
	}

	BYTE junk;
	if (GetByte(&junk))
	{
		Log("   Warning: Unexpected data following int at %d\n", m_XmlLineNumber);
	}
}


void XMLUTFDlg::ProcessInt()
{
	Store data;

	while (!IsTag())
	{
		put(data, GetDWORD());
	}

	m_LeafNode->data_size  =
	m_LeafNode->data_alloc = data.size();
	m_LeafNode->data = (BYTE *) data.data();
}


void XMLUTFDlg::ProcessFloat(int size)
{
	float * data = new float[size];
	m_LeafNode->data_size  =
	m_LeafNode->data_alloc = size * sizeof(float);
	m_LeafNode->data = (BYTE *) data;

	for (int i = 0; i < size; i++)
	{
		data[i] = GetFloat();
	}

	BYTE junk;
	if (GetByte(&junk))
	{
		Log("   Warning: Unexpected data following float at %d\n", m_XmlLineNumber);
	}
}


void XMLUTFDlg::ProcessFloat()
{
	Store data;

	while (!IsTag())
	{
		put(data, GetFloat());
	}

	m_LeafNode->data_size  =
	m_LeafNode->data_alloc = data.size();
	m_LeafNode->data = (BYTE *) data.data();
}


void XMLUTFDlg::ProcessRGB()
{
	ProcessFloat(3);
	float * rgb = (float *) m_LeafNode->data;
	if (rgb[0] > 1 || rgb[1] > 1 || rgb[2] > 1)
	{
		rgb[0] /= 255;
		rgb[1] /= 255;
		rgb[2] /= 255;
	}
}


void XMLUTFDlg::ProcessHex(int size)
{
	DWORD alloc = (size + 3) & ~3;
	BYTE * data = new BYTE[alloc];
	memset(data, 0, alloc);
	m_LeafNode->data_size  = size * sizeof(BYTE);
	m_LeafNode->data_alloc = alloc;
	m_LeafNode->data = data;

	for (int i = 0; i < size; i++)
	{
		data[i] = GetHex();
	}

	BYTE junk;
	if (GetByte(&junk))
	{
		Log("   Warning: Unexpected data following hex at %d\n", m_XmlLineNumber);
	}
}


void XMLUTFDlg::ProcessFrame_rects()
{
	FrameRects fr;
	Store data;

	while (!IsTag())
	{
		fr.frame = GetDWORD();
		for (int i = 0; i < 4; ++i)
		{
			fr.rect[i] = GetFloat();
		}
		put(data, fr);
	}

	m_LeafNode->data_size  =
	m_LeafNode->data_alloc = data.size();
	m_LeafNode->data = (BYTE *) data.data();
}


bool XMLUTFDlg::GetByte(BYTE *byte)
{
	BYTE b;
	b = *m_XmlDataPtr;
	if ((b == '\n') || (b == '\0'))
	{
		ReadXML();
		b = *m_XmlDataPtr;
	}

	if (b == '<')
	{
		// Is it a comment?
		if (strncmp(m_XmlDataPtr, "<!--", 4))
		{
			// No, assume it is a tag
			return false;
		}

		// Find the end of the XML comment
		m_XmlDataPtr += 4;
		int start = m_XmlLineNumber;
		char *p;
		while (!(p = strstr(m_XmlDataPtr, "-->")))
		{
			m_XmlDataPtr = m_XmlDataBuffer;
			m_XmlDataBuffer[0] = 0;
			if (fgets(m_XmlDataBuffer, XML_BUFFER_SIZE, m_XmlFile))
			{
				m_XmlLineNumber++;
			}
			else
			{
				Log("   Error: Unterminated comment.  Line %d\n", start);
				return false;
			}
		}

		// Move to the end of the comment
		m_XmlDataPtr = p+3;

		// Skip spaces after the comment
		SkipSpaces();

		// Use recursion to get the data following the comment
		return (GetByte(byte));
	}

	*byte = b;
	m_XmlDataPtr++;
	return true;
}


char * XMLUTFDlg::GetNumeric(bool is_float)
{
	char buffer[32];
	int i=0;
	char b;
	bool hex = false;

	// Skip any leading spaces
	SkipSpaces();

	// Is the value hexadecimal?
	if (!is_float && (((m_XmlDataPtr[0] == '0') && (m_XmlDataPtr[1] == 'x'))
					|| (m_XmlDataPtr[0] == '#')))
	{
		m_XmlDataPtr += (m_XmlDataPtr[0] == '#') ? 1 : 2;
		buffer[i++] = '0';
		buffer[i++] = 'x';
		hex = true;
	}
	m_deg = false;
	if (*m_XmlDataPtr == '-')
	{
		buffer[i++] = '-';
		++m_XmlDataPtr;
	}
	while ((b = *m_XmlDataPtr) != '<')
	{
		++m_XmlDataPtr;
		if (isdigit(b) || (hex && isxdigit(b)))
		{
			buffer[i++] = b;
		}
		else if (is_float && ((b == '.') || (b == 'e') || (b == '-') || (b == '+')))
		{
			buffer[i++] = b;
		}
		else if ((b == ',') || (b == ';') || (b == ':') || (b == '\n'))
		{
			// successful
			break;
		}
		else if (is_float && b == '°')
		{
			m_deg = true;
		}
		else
		{
			Log("   Warning: Invalid numeric data at line %d\n", m_XmlLineNumber);
			break;
		}
	}

	// Now, skip any trailing spaces
	SkipSpaces();

	// Create a buffer for the results
	buffer[i++] = '\0';
	char *result = new char[i];
	strcpy(result, buffer);
	return (result);
}


DWORD XMLUTFDlg::GetDWORD()
{
	DWORD value = 0;

	// Skip leading spaces
	SkipSpaces();

	char *data = GetNumeric(false);
	if (*data == '-')
	{
		value = atoi(data);
	}
	else
	{
		value = strtoul(data, 0, 0);
	}
	delete data;

	return (value);
}


float XMLUTFDlg::GetFloat()
{
	if (m_mtxidx)
	{
		--m_mtxidx;
		return m_matrix[8-m_mtxidx];
	}

	// Skip leading spaces
	SkipSpaces();

	// If it starts with "R ", let's assume a rotation.
	if (m_XmlDataPtr[0] == 'R' && m_XmlDataPtr[1] == ' ')
	{
		m_XmlDataPtr++;
		Vector rot;
		GetVector(rot);
		EulerMatrix(rot);
		m_mtxidx = 8;
		return m_matrix[0];
	}

	// If it starts with "Q ", let's assume a quaternion.
	if (m_XmlDataPtr[0] == 'Q' && m_XmlDataPtr[1] == ' ')
	{
		m_XmlDataPtr++;
		double angle = GetFloat() / 2 * 3.1415926535897932 / 180;
		double s = sin(angle);
		m_matrix[6] = GetFloat() * s;
		m_matrix[7] = GetFloat() * s;
		m_matrix[8] = GetFloat() * s;
		m_mtxidx = 3;
		return cosf(angle);
	}

	// If it starts with '#', let's assume a color.
	if (m_XmlDataPtr[0] == '#')
	{
		DWORD col = GetDWORD();
		m_mtxidx = 2;
		m_matrix[7] = ((col >> 8) & 0xFF) / 255.0f;
		m_matrix[8] = (col & 0xFF) / 255.0f;
		return ((col >> 16) & 0xFF) / 255.0f;
	}

	float value;
	// Some VMeshData texture coordinates don't seem to be floats,
	// so they were written as hex.
	if (m_XmlDataPtr[0] == '0' && m_XmlDataPtr[1] == 'x')
	{
		DWORD val = GetDWORD();
		value = *(float *) &val;
	}
	else
	{
		char *data = GetNumeric(true);
		value = (float) atof(data);
		delete data;
	}

	if (m_deg)
	{
		value = float(value * 3.1415926535897932 / 180);
	}
	return (value);
}


DWORD XMLUTFDlg::GetCRC(bool ignore /* = true */)
{
	DWORD value;

	// Skip leading spaces
	SkipSpaces();

	// If it starts with 0x, it's the CRC
	if (m_XmlDataPtr[0] == '0' && m_XmlDataPtr[1] == 'x')
	{
		value = GetDWORD();
	}
	// Otherwise it's a string, so encode it
	else
	{
		char *name = GetString();
		value = (ignore) ? fl_crc32(name) : fl_crc32_b(name);
	}

	return (value);
}


BYTE XMLUTFDlg::GetHex()
{
	// Skip leading spaces
	SkipSpaces();

	// Get the two hex digits
	BYTE upper, lower;
	if ((!GetByte(&upper)) ||
		(!GetByte(&lower)))
	{
		Log("   Warning: Expecting (more) hex data at line %d\n", m_XmlLineNumber);
		return 0;
	}

	// Skip trailing spaces
	SkipSpaces();

	if (!isxdigit(upper) || !isxdigit(lower))
	{
		Log("   Warning: Invalid hex data at line %d\n", m_XmlLineNumber);
		return 0;
	}

	if (isdigit(upper))
	{
		upper -= '0';
	}
	else
	{
		upper = toupper(upper) - 'A' + 10;
	}

	if (isdigit(lower))
	{
		lower -= '0';
	}
	else
	{
		lower = toupper(lower) - 'A' + 10;
	}

	return ((upper << 4) + lower);
}


WORD XMLUTFDlg::GetWORD()
{
	DWORD value = GetDWORD();
	if ((int) value > 65535 || (int) value < -32768)
	{
		Log("   Warning: Value exceeds 65535 on line %d\n", m_XmlLineNumber);
	}

	return ((WORD) value);
}


BYTE XMLUTFDlg::GetBYTE()
{
	DWORD value = GetDWORD();
	if ((int) value > 255 || (int) value < -128)
	{
		Log("   Warning: Value exceeds 255 on line %d\n", m_XmlLineNumber);
	}

	return ((BYTE) value);
}


DWORD XMLUTFDlg::GetHexAttr(char *attr)
{
	DWORD value = 0;
	char *hex = GetXmlAttr(attr);
	if (hex)
	{
		value = strtoul(hex, 0, 0);
		delete hex;
	}
	return (value);
}


float XMLUTFDlg::GetFloatAttr(char *attr)
{
	float value = 0;
	char *flt = GetXmlAttr(attr);
	if (flt)
	{
		value = (float) atof(flt);
		delete flt;
	}
	return (value);
}


DWORD XMLUTFDlg::GetTimeAttr(char *attr)
{
	DWORD value = 0;
	char *t = GetXmlAttr(attr);
	if (t)
	{
		SYSTEMTIME st;
		if (sscanf(t, "%hd-%hd-%hd %hd:%hd:%hd",
					  &st.wYear, &st.wMonth, &st.wDay,
					  &st.wHour, &st.wMinute, &st.wSecond) == 6)
		{
			value = ((st.wYear - 1980) << 9) | (st.wMonth << 5) | (st.wDay) |
					(st.wHour << 27) | (st.wMinute << 21) | ((st.wSecond >> 1) << 16);
		}
		delete t;
	}
	return (value);
}


// I would just use the above, but for some reason there are pre-1980 times.
bool XMLUTFDlg::GetFileTimeAttr(char *attr, FILETIME& ft)
{
	bool rc = false;
	char *t = GetXmlAttr(attr);
	if (t)
	{
		SYSTEMTIME st;
		st.wMilliseconds = 0;
		if (sscanf(t, "%hd-%hd-%hd %hd:%hd:%hd",
					  &st.wYear, &st.wMonth, &st.wDay,
					  &st.wHour, &st.wMinute, &st.wSecond) == 6)
		{
			SystemTimeToFileTime(&st, &ft);
			rc = true;
		}
		delete t;
	}
	return (rc);
}


// Retrieve a string, which is either comma- or newline-terminated, or enclosed
// in quotes.  It uses a global buffer, so DON'T delete it.
char *XMLUTFDlg::GetString()
{
	char *name;
	char *end;

	// Skip leading spaces
	SkipSpaces();

	if (*m_XmlDataPtr == '"')
	{
		name = ++m_XmlDataPtr;
		end = m_XmlDataPtr + strcspn(m_XmlDataPtr, "\"\n");
		if (*end != '"')
		{
			m_XmlDataPtr = end;
			Log("   Warning: Unterminated string at line %d\n", m_XmlLineNumber);
		}
		else
		{
			m_XmlDataPtr = end + 1;
		}
	}
	else
	{
		name = m_XmlDataPtr;
		end = m_XmlDataPtr + strcspn(m_XmlDataPtr, "<,\n");
		m_XmlDataPtr = end;
	}

	if (*m_XmlDataPtr == ',')
	{
		++m_XmlDataPtr;
	}

	size_t len = end - name;
	if (len > m_strcap)
	{
		delete m_string;
		m_string = new char[len+1];
		m_strcap = len;
	}
	memcpy(m_string, name, len);
	m_string[len] = 0;

	return m_string;
}


void XMLUTFDlg::GetVector(Vector &vec)
{
	vec.x = GetFloat();
	vec.y = GetFloat();
	vec.z = GetFloat();
}


void XMLUTFDlg::GetMatrix(Matrix &mat)
{
	float *array = (float *) &mat;
	for (int i = 0; i < 9; ++i)
	{
		array[i] = GetFloat();
	}
}


// Convert rotation vector (pitch/yaw/roll) to orientation matrix.	From the
// formula in FLOrientation.xls, verified to match EulerMatrix in common.dll.
void XMLUTFDlg::EulerMatrix(const Vector &rot)
{
	double phi	 = rot.x * 3.1415926535897932 / 180;
	double theta = rot.y * 3.1415926535897932 / 180;
	double psi	 = rot.z * 3.1415926535897932 / 180;

	double sin_phi	 = sin(phi);
	double cos_phi	 = cos(phi);
	double sin_theta = sin(theta);
	double cos_theta = cos(theta);
	double sin_psi	 = sin(psi);
	double cos_psi	 = cos(psi);

	m_matrix[0] = float(cos_theta * cos_psi);
	m_matrix[1] = float(sin_phi * sin_theta * cos_psi +  cos_phi * -sin_psi);
	m_matrix[2] = float(cos_phi * sin_theta * cos_psi + -sin_phi * -sin_psi);
	m_matrix[3] = float(cos_theta * sin_psi);
	m_matrix[4] = float(sin_phi * sin_theta * sin_psi +  cos_phi * cos_psi);
	m_matrix[5] = float(cos_phi * sin_theta * sin_psi + -sin_phi * cos_psi);
	m_matrix[6] = float(-sin_theta);
	m_matrix[7] = float(sin_phi * cos_theta);
	m_matrix[8] = float(cos_phi * cos_theta);
}
