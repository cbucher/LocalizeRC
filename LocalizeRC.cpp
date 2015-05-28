// LocalizeRC.cpp : Defines the class behaviors for the application.
//

#include "stdafx.h"
#include "resource.h"
#include "IniEx.h"
#include "LocalizeRC.h"
#include "LocalizeRCDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CLocalizeRCApp

BEGIN_MESSAGE_MAP(CLocalizeRCApp, CWinApp)
	ON_COMMAND(ID_HELP, CWinApp::OnHelp)
END_MESSAGE_MAP()


// CLocalizeRCApp construction

CLocalizeRCApp::CLocalizeRCApp()
{
	// TODO: add construction code here,
	// Place all significant initialization in InitInstance
}


// The one and only CLocalizeRCApp object

CLocalizeRCApp theApp;


// CLocalizeRCApp initialization

BOOL CLocalizeRCApp::InitInstance()
{
	// InitCommonControls() is required on Windows XP if an application
	// manifest specifies use of ComCtl32.dll version 6 or later to enable
	// visual styles.  Otherwise, any window creation will fail.
	InitCommonControls();

	//AfxEnableControlContainer(); no ActiveX support
	
	AfxInitRichEdit();

	CWinApp::InitInstance();

	SetRegistryKey(_T("LocalizeRC"));

	CurLangID = STANDARD_LANGID;

	if( !LoadLangIDDLL( static_cast<LANGID>( GetProfileInt( SEC_LASTPROJECT, ENT_LANGID, 0 ) )) )
		if( !LoadLangIDDLL(GetUserDefaultLangID()) )
			LoadLangIDDLL(GetSystemDefaultLangID());

	CLocalizeRCDlg dlg;
	dlg.LangID = CurLangID;

	m_pMainWnd = &dlg;
	INT_PTR nResponse = dlg.DoModal();
	if (nResponse == IDOK)
	{
		// TODO: Place code here to handle when the dialog is
		//  dismissed with OK
	}
	else if (nResponse == IDCANCEL)
	{
		// TODO: Place code here to handle when the dialog is
		//  dismissed with Cancel
	}

	if( CurLangID != STANDARD_LANGID )
		FreeLibrary( AfxGetResourceHandle() );

	// Since the dialog has been closed, return FALSE so that we exit the
	//  application, rather than start the application's message pump.
	return FALSE;
}

BOOL CLocalizeRCApp::LoadLangIDDLL(LANGID LangID)
{
	HINSTANCE hInstance;
	CString strLangIDDLL;
	
	if( LangID == STANDARD_LANGID )	// integrated language is the right one
		return true;
	
	strLangIDDLL.Format( _T("LocalizeRC%.2x.dll"), LangID );
	hInstance = LoadLibrary( strLangIDDLL );
	if( hInstance )
	{
		AfxSetResourceHandle( hInstance );
		CurLangID = LangID;
		return true;
	}
	return false;
}
