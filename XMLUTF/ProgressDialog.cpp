//////////////////////////////////////////////////////////////////////
// ProgressDialog.cpp
//
// Refer to XMLUTFDlg.cpp for software documentation
//////////////////////////////////////////////////////////////////////


//////////////////////////////////////////////////////////////////////
// Include Files
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "XMLUTF.h"
#include "XMLUTFDlg.h"
#include "ProgressDialog.h"


//////////////////////////////////////////////////////////////////////
// Definitions
//////////////////////////////////////////////////////////////////////

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


//////////////////////////////////////////////////////////////////////
// ProgressDialog
//////////////////////////////////////////////////////////////////////

ProgressDialog::ProgressDialog(CWnd* pParent /*=NULL*/)
	: CDialog(ProgressDialog::IDD, pParent)
{
    m_Parent = (XMLUTFDlg *) pParent;
	//{{AFX_DATA_INIT(ProgressDialog)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void ProgressDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(ProgressDialog)
	DDX_Control(pDX, IDC_PROGRESS, m_ProgressCtrl);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(ProgressDialog, CDialog)
	//{{AFX_MSG_MAP(ProgressDialog)
	ON_WM_TIMER()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


void ProgressDialog::OnCancel()
{
    m_Parent->m_ConversionAborted = true;

    SetDlgItemText(
        IDC_STATUS,
        "Aborting conversion..."
    );

    CWnd* abortButton = GetDlgItem(IDCANCEL);

    if (abortButton)
    {
        abortButton->EnableWindow(FALSE);
    }

    // Do not call CDialog::OnCancel().
    // The worker thread finishes on its own,
    // m_ConversionInProgress will become false,
    // and OnTimer will close the dialog.
}

BOOL ProgressDialog::OnInitDialog() 
{
	CDialog::OnInitDialog();

    GetDlgItem(IDC_STATUS)->SetWindowText("Counting XML files...");

    BeginWaitCursor();

    SetTimer(1,100,NULL);

    return TRUE;
}


BOOL ProgressDialog::DestroyWindow() 
{
    EndWaitCursor();
	
	return CDialog::DestroyWindow();
}


void ProgressDialog::OnTimer(UINT nIDEvent)
{
    if (nIDEvent == 1)
    {
        char source_path[MAX_PATH] = { 0 };

        int file_count = 0;
        int files_opened = 0;
        bool conversion_in_progress = false;

        EnterCriticalSection(&m_Parent->m_Mutex);

        file_count = m_Parent->m_XmlFileCount;
        files_opened = m_Parent->m_NumXmlFilesOpened;
        conversion_in_progress = m_Parent->m_ConversionInProgress;

        strcpy_s(
            source_path,
            MAX_PATH,
            m_Parent->m_CurrentSourcePath + m_Parent->m_PathTrim
        );

        LeaveCriticalSection(&m_Parent->m_Mutex);

        if (file_count > 0)
        {
            CString status;

            int percent = 0;

            if (file_count > 0)
            {
                percent = int(
                    100LL * files_opened / file_count
                );

                if (percent > 100)
                    percent = 100;
            }

            status.Format(
                "Processed %d of %d XML files (%d%%)",
                files_opened,
                file_count,
                percent
            );

            SetDlgItemText(IDC_STATUS, status);

            m_ProgressCtrl.SetRange32(0, file_count);
            m_ProgressCtrl.SetPos(
                min(files_opened, file_count)
            );
        }
        else
        {
            CString status;

            status.Format(
                "Counting XML files... (%d)",
                files_opened
            );

            SetDlgItemText(IDC_STATUS, status);
        }

        SetDlgItemText(
            IDC_PATH,
            source_path
        );

        // IMPORTANT:
        // Close ProgressDialog only when the worker has truly finished.
        if (!conversion_in_progress)
        {
            KillTimer(1);

            m_ProgressCtrl.SetPos(file_count);

            SetDlgItemText(
                IDC_STATUS,
                "Conversion complete"
            );

            OnOK();
            return;
        }
    }

    CDialog::OnTimer(nIDEvent);
}


//////////////////////////////////////////////////////////////////////
// End ProgressDialog.cpp
//////////////////////////////////////////////////////////////////////

