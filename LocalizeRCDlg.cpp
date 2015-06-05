// LocalizeRCDlg.cpp : implementation file
//

#include "stdafx.h"
#include "resource.h"
#include "IniEx.h"
#include "LocalizeRC.h"
#include "LocalizeRCDlg.h"

#include <math.h>
#include <boost/regex/mfc.hpp>

#define OLDFILEFORMAT -2

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

//#define _MFC_VER 0x0420

int _LineCount( CString &srchBuf, int startPos = 0, int endPos = -1 )
{
	// Purpose: Get count of text lines in a specified area of a string buffer
	// Parameters:
	//	srchBuf			(in)	The input string buffer to search for lines. Must be null terminated.
	//	startPos		(in)	Character offset into buffer where search is to begin (default = 0)
	//	endPos			(in)	Character offset into buffer one beyond last character to search.
	//							Default (-1) searches entire length (up to a null terminator)
	// Note: all character offsets are zero based
	//
	// Returns: number of text lines found in specified area of string buffer
	//
	int lfnd = startPos;
	int lend = srchBuf.GetLength();
	if ( endPos > -1 && endPos < lend )
		lend = endPos;
	//
	int linecount = 0;
	while ( lfnd < lend )
	{
		lfnd = srchBuf.Find( _T('\n'), lfnd ) + 1;
		if ( lfnd < 0 || lfnd > lend )
			break;
		//
		linecount++;
	};
	return linecount;
}

int _GetLinePos( CString &srchBuf, int startPos, int line )
{
	// Get buffer offset in characters to the start of a specified line number
	// Parameters:
	//	srchBuf			(in)	The input string buffer to search. Must be null terminated.
	//	startPos		(in)	Character offset into buffer to begin the search (0 based)
	//	line			(in)	The line number whose position is needed (0 based)
	//
	// Returns: Character offset into srchBuf to the start of the specified line, or -1
	//			if line is <0, line exceeds number of lines in the buffer, or startPos exceeds
	//			number characters in string buffer.

	int lfnd = startPos; // Search begins at this offset
	int lend = srchBuf.GetLength(); // Search ends at this offset
	int iRet = -1;

	if ( startPos > -1 && startPos < lend )
	{
		for ( int i = 0 ; i < line ; i++ )
		{
			lfnd = srchBuf.Find( _T('\n'), lfnd ) + 1;
			if ( lfnd < 0 || lfnd > lend )
			{
				lfnd = -1; // Search failed
				break;
			}
		}
		iRet = lfnd;
	}
	return iRet;
}

int _GetLengthOfValue( CString &strbuf, int startpos )
{
	// Purpose: return length of a number string including any leading spaces
	// Parameters:
	// strbuf			(in)	String buffer containing substring
	// startpos			(in)	Starting character offset into string buffer to substring
	//
	// Returns: length of number including any leading spaces; returns 0 if not a number
	//
	int digits = 0;
	int retlen = 0;

	for ( int i = startpos; ; i++, retlen++ )
	{
		TCHAR ch = strbuf.GetAt(i);
		if ( ch == _T(' ') && digits == 0 ) // Allow leading spaces
			continue;
		else if ( ch < _T('0') || ch > _T('9') ) // Done with number
			break;
		else // Count off a digit
			digits++;
		//
	}
	return retlen;
}
// CAboutDlg dialog used for App About
class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

// Dialog Data
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

// Implementation
protected:
	DECLARE_MESSAGE_MAP()
private:
	CString m_strAbout;
public:
	virtual BOOL OnInitDialog();
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
, m_strAbout(_T(""))
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_ABOUT, m_strAbout);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
END_MESSAGE_MAP()


BOOL CAboutDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	m_strAbout.LoadString( IDS_ABOUT );
	UpdateData( false );

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

// CLocalizeRCDlg dialog
CLocalizeRCDlg::CLocalizeRCDlg(CString& strWorkspace, CWnd* pParent /*=NULL*/)
	: CDialog(CLocalizeRCDlg::IDD, pParent)
	, m_bCopy(FALSE)
	, m_nObsoleteItems(0)
	, m_strWorkspace(strWorkspace)
	, m_strEdit(_T(""))
	, m_strTextmode(_T(""))
	, m_bNoSort(FALSE)
	, m_strInputRC(_T(""))
	, m_strLangINI(_T(""))
	, m_strOutputRC(_T(""))
	, m_strAbout(_T(""))
{
	if(m_strWorkspace.IsEmpty())
	{
		// last used Workspace from registry
		m_strWorkspace = AfxGetApp()->GetProfileString(SEC_LASTPROJECT, ENT_WORKSPACE);
	}
}

void CLocalizeRCDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Check(pDX, IDC_COPY, m_bCopy);
	DDX_CBIndex(pDX, IDC_OBS_ITEMS, m_nObsoleteItems);
	DDX_Text(pDX, IDC_WORKSPACE, m_strWorkspace);
	DDX_Control(pDX, IDC_LANGUAGE, m_CtrlLanguage);
	DDX_Text(pDX, IDC_EDIT, m_strEdit);
	DDX_Text(pDX, IDC_TEXTMODE, m_strTextmode);
	DDX_Check(pDX, IDC_NOSORT, m_bNoSort);
	DDX_Text(pDX, IDC_INPUTRC, m_strInputRC);
	DDX_Text(pDX, IDC_LANGINI, m_strLangINI);
	DDX_Text(pDX, IDC_OUTPUTRC, m_strOutputRC);
	DDX_Text(pDX, IDC_ABOUT, m_strAbout);
}

BEGIN_MESSAGE_MAP(CLocalizeRCDlg, CDialog)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	//}}AFX_MSG_MAP
	ON_BN_CLICKED(IDC_CREATEINI, OnBnClickedCreateini)
	ON_BN_CLICKED(IDC_OPENINI, OnBnClickedOpenini)
	ON_BN_CLICKED(IDC_CREATEOUTPUT, OnBnClickedCreateoutput)
	ON_WM_DESTROY()
	ON_BN_CLICKED(IDC_REVERSEINI, OnBnClickedReverseini)
	ON_BN_CLICKED(IDC_CHNG_WORKSPACE, OnBnClickedChngWorkspace)
	ON_CBN_SELCHANGE(IDC_LANGUAGE, OnCbnSelchangeLanguage)
	ON_CBN_SELCHANGE(IDC_OBS_ITEMS, OnCbnSelchangeObsItems)
	ON_BN_KILLFOCUS(IDC_COPY, OnBnKillfocusCopy)
	ON_BN_KILLFOCUS(IDC_NOSORT, OnBnKillfocusNosort)
	ON_BN_CLICKED(IDC_NEW_WORKSPACE, OnBnClickedNewWorkspace)
	ON_BN_CLICKED(IDC_CHNG_INPUTRC, OnBnClickedChngInputrc)
	ON_BN_CLICKED(IDC_CHNG_LANGINI, OnBnClickedChngLangini)
	ON_BN_CLICKED(IDC_CHNG_OUTPUTRC, OnBnClickedChngOutputrc)
END_MESSAGE_MAP()


// CLocalizeRCDlg message handlers
BOOL CLocalizeRCDlg::OnInitDialog()
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

	// LoadIcon only loads the 32x32 icon, therefore use ::LoadImage for other sizes (16x16)
	hLargeIcon = AfxGetApp()->LoadIcon ( IDR_MAINFRAME );
	hSmallIcon = (HICON) ::LoadImage ( AfxGetResourceHandle(), 
                                         MAKEINTRESOURCE(IDR_MAINFRAME),
                                         IMAGE_ICON, 16, 16, LR_DEFAULTCOLOR );

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(hLargeIcon, TRUE);		// Set big icon
	SetIcon(hSmallIcon, FALSE);		// Set small icon

	// display text mode (UNICODE/ANSI)
#ifdef UNICODE
	m_strTextmode.LoadString( IDS_UNICODE );
#else
	m_strTextmode.LoadString( IDS_ANSI );
#endif
	m_strAbout.LoadString( IDS_ABOUT );
	UpdateData(false);

	// get installed languages
	CFileFind Finder;
	TCHAR szFilename[MAX_PATH];
	GetModuleFileName( NULL, szFilename, MAX_PATH );
	
	AddLanguage( &m_CtrlLanguage, _T("09"), LangID );
	
	// the last 2 chars are the LanguageID in hexadecimal form
	CString strSearch;
	strSearch.Format( _T("%sLocalizeRC??.dll"), GetFolder(szFilename) );

	BOOL bResult = Finder.FindFile( strSearch );
	CString strLangCode;

	while( bResult )
	{
		bResult = Finder.FindNextFile();
		// extract 2-digit language code
		strLangCode = Finder.GetFileTitle().Right(2);
		AddLanguage( &m_CtrlLanguage, strLangCode, LangID );
	}
	Finder.Close();

	// Load last workspace
	LoadWorkspace(false);

	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CLocalizeRCDlg::OnSysCommand(UINT nID, LPARAM lParam)
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
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CLocalizeRCDlg::OnPaint() 
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, hSmallIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}

// The system calls this function to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CLocalizeRCDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(hSmallIcon);
}

#define NUMKEYWORDS 8
LPCTSTR strKeyword[NUMKEYWORDS] = 
{
	_T("ACCELERATORS"),
	_T("DIALOG"),
	_T("DIALOGEX"),
	_T("MENU"),
	_T("MENUEX"),
	_T("STRINGTABLE"),
	_T("DLGINIT"),
	_T("code_page")
};
	
// random string, that indicates an error
#define ERR_STR _T("asfdshkagzuwrthgadsfhgkh12385143258")

BOOL CLocalizeRCDlg::OpenInputRC(BOOL bShowError)
{
	return OpenRCFile( m_strInputRC, newRCdata, bShowError );
}

void CLocalizeRCDlg::LogUserMessage( int strID )
{
	CString msg;
	msg.FormatMessage( strID );
	LogUserMessage( msg );
}

void CLocalizeRCDlg::LogUserMessage( CString msg )
{
	m_strEdit.Append( msg );
	m_strEdit.Append( _T("\n") );
	UpdateData( false );
}


BOOL CLocalizeRCDlg::OpenRCFile( CString filename, CString &strbuf, BOOL bShowError )
{
	// The file from which to load the contents of rich edit control
	CStdioFile* pFile = NULL;
	
	if( filename.IsEmpty() )
	{
		if( bShowError )
			LogUserMessage( IDS_ERR_FILENAMEEMPTY );
		// clear contents in edit-control
		strbuf = _T("");
		UpdateData( false );
		return false;
	}

	BOOL bIsUnicode = CStdioUnicodeFile::IsUnicode( filename );

	try
	{
	#ifdef UNICODE	
		if( bIsUnicode )
		{
			pFile = new CStdioUnicodeFile( filename, CFile::modeRead | CFILEFLAG_UNICODEHELPER );
			// skip first two bytes
			pFile->Seek( 2, CFile::begin );
		}
		else
			pFile = new CStdioUnicodeFile( filename, CFile::modeRead );
	#else
		if( bIsUnicode )
		{
			CString strErr;
			strErr.Format( IDS_UNICODEFILE, filename );
			AfxMessageBox( strErr );

			// clear contents in edit-control
			strbuf = _T("");
			UpdateData( false );
			return false;	// cancel
		}
		else
			pFile = new CStdioUnicodeFile( filename, CFile::modeRead );
	#endif
	}
	catch( CFileException* e )
	{
		TCHAR szCause[255];
		e->GetErrorMessage(szCause, 255);
		AfxMessageBox(szCause);
		e->Delete();

		// clear contents in edit-control
		strbuf = _T("");
		UpdateData( false );
		return false;	// cancel
	}
	// clear contents in edit-control
	strbuf = _T("");

	// fill edit-control with contents of the input RC-file
	// Update dialog size/pos data from an existing output RC file, if any
	CString strLine;
	while( pFile->ReadString( strLine ) )
		strbuf += strLine + _T("\r\n");
	//
	UpdateData( false );

	pFile->Close();
	delete pFile;
	if ( bShowError ) // Report open status if ok to show it
	{
		CString msg;
		msg.FormatMessage( IDS_LFILEOPENED, filename );
		LogUserMessage( msg );
	}
	return true;
}

void CLocalizeRCDlg::RemoveNewRCFileRESItems( )
{
	// Disable ICON, BITMAP, and CURSOR entries that reference the "Res" folder.
	// NOTE: HTML entries are a special case - these are not disabled
	//
	// This cleanup is normally done only if the user has opted to NOT copy the Res
	// folder, which may be the case for a language DLL since those resources can
	// continue to be loaded from the main app.
	// 
	LogUserMessage( IDS_LRESREMOVAL );
	boost::tregex expr( _T("^\\w+\\h+(ICON|BITMAP|CURSOR)\\h+\"(?i)res.+\"$") );
	boost::tregex_iterator m1(boost::make_regex_iterator(newRCdata, expr, boost::match_not_dot_newline)), mEnd;
	int startpos = 0; // Starting position of current match
	int processedcount = 0; // Total number of match hits processed
	while ( m1 != mEnd ) // Iterate all matching entries and comment out each one
	{
		startpos = (*m1)[0].first - newRCdata;
		CString chk = newRCdata.Mid( startpos, (*m1)[0].length() );
		// Note that edits must not alter the length of the buffer since it will affect
		// the active regex search iteration
		// Comment out the line by overwriting first 2 characters with //
		newRCdata.SetAt( startpos++, _T('/') ); 
		newRCdata.SetAt( startpos, _T('/') );
		m1++;
		processedcount++;
	} // endwhile: iterate all matching entries
	CString msg;
	msg.FormatMessage( IDS_LRESREMSTATS, processedcount );
	LogUserMessage( msg );
}

void CLocalizeRCDlg::MergeOldRCFileDesignInfo( CString &oldRCdata )
{
	// Preserve dialog designer info changes made to accommodate translation string size
	// differences in a previously generated output RC file
	// Parameters:
	// oldRCdata		(in)	Reference containing buffered contents of existing RC data
	//							containing possible dialog layout updates (read only)
	//
	// Progress information is reported to the log output

	CString msg; // For user messages
	int secitmsfound = 0; // Items found by regex parser
	int secitmsskipped = 0;
	int secitmsupdated = 0;

	// Pointers into old RC data
	int designinfosecpos = -1; // Buffer character offset to start of DESIGNINFO section
	int subsectionpos = 0; // Buffer character offset to current sub-section
	CString linekey; // Key for current sub-section line entry
	//
	// Pointers into new RC data
	int ndesigninfosecpos = -1; // Buffer character offset to start of DESIGNINFO section
	int nsubsectionpos = 0; // Buffer character offset to matching sub-section

	LogUserMessage( IDS_LMERGINGOLDRC2 );
	// Find the design info section in the new RC data
	ndesigninfosecpos = newRCdata.Find( _T("GUIDELINES DESIGNINFO"), 0);
	if ( ndesigninfosecpos < 0 ) // The section doesn't exist in the new data (not expected)
		return;
	//
	// Find the design info section in old RC data and parse it
	boost::tregex expr( _T("^GUIDELINES DESIGNINFO\\s+BEGIN$((?:\\s+(.+?(?:, *DIALOG))\\s+(?:.+?)END$)+)") );
	boost::tregex_iterator m1(boost::make_regex_iterator(oldRCdata, expr)), mEnd;
	while ( m1 != mEnd ) // ToDo: Don't really need an iterator here since there's only one section
	{
		// Get design info section position in buffer and body text handy
		designinfosecpos = (*m1)[1].first - oldRCdata; // Start pos in buffer of design info section
		CString dinfobody = CString( (*m1)[1].first, (*m1)[1].length() ); // Body of design info section
		//
		// Parse section for sub-section blocks
		boost::tregex expr2( _T("(?:\\s+(.+?(?:, *DIALOG))\\s+(?:.+?)END$)") );
		boost::tregex_iterator m2(boost::make_regex_iterator(dinfobody, expr2)), mEnd2;
		while ( m2 != mEnd2 ) // Iterate over all sub-sections in the design info section
		{
			secitmsfound = 0; // Reset items stats for this sub-section
			secitmsskipped = 0;
			secitmsupdated = 0;
			// Get sub-section position in buffer, key name, and body text handy
			subsectionpos = ((*m2)[0].first - dinfobody) + designinfosecpos; // Start pos in buffer of this sub-section
			CString dinfoblk = CString( (*m2)[0].first, (*m2)[0].length() ); // Current sub-section text
			CString dinfokey = CString( (*m2)[1].first, (*m2)[1].length() ); // Key for current sub-section
			// Try to find corresponding sub-section in new data
			nsubsectionpos = newRCdata.Find( dinfokey, ndesigninfosecpos );
			if ( nsubsectionpos < 0 ) // Sub-section not found in new data
			{
				msg.FormatMessage( IDS_LDESIGNSECSKIPNF, dinfokey );
				continue; // Nothing to update for this section...
			}
			//
			// Parse sub-section for numeric margin values
			boost::tregex expr3( _T("^(.*?(?:,| ))( *\\d+)") );
			boost::tregex_iterator m3(boost::make_regex_iterator(dinfoblk, expr3, boost::match_not_dot_newline)), mEnd3;
			while ( m3 != mEnd3 ) // Iterate over all numeric margin settings in the sub-section body
			{
				CString subentrykey = CString( (*m3)[1].first, (*m3)[1].length() ); // Current entry key
				subentrykey = subentrykey.TrimLeft(); // Clean up the key
				CString subentryval = CString( (*m3)[2].first, (*m3)[2].length() ); // Current entry value
				// Try to find the corresponding entry in the new data
				int nsubentrypos = newRCdata.Find( subentrykey, nsubsectionpos );
				if ( nsubentrypos < 0 ) // Entry not found in the new RC data
				{
					secitmsskipped++;
					continue;
				}
				//
				// Update the entry value in new data from matching entry in old data
				int nvalpos = newRCdata.Find( _T(","), nsubentrypos ) + 1; // Starting pos of number string
//				int nvallen = newRCdata.Find( _T("\n"), nvalpos ) - nvalpos; // Length of number string
				int nvallen = _GetLengthOfValue( newRCdata, nvalpos );
				if ( nvalpos < 1 || nvallen < 1 ) // Safety check
					continue;
				//
				CString chk = newRCdata.Mid( nvalpos, nvallen ); // Number string in new data
				if ( chk != subentryval ) // Need to update
				{
					newRCdata.Delete( nvalpos, nvallen );
					newRCdata.Insert( nvalpos, subentryval );
					secitmsupdated++;
				}
				secitmsfound++;
				m3++;
			} // endwhile: iterate sub-section entries
			m2++;
			msg.FormatMessage( IDS_LDESIGNSECSTAT, dinfokey, secitmsfound, secitmsupdated, secitmsskipped );
			LogUserMessage( msg );
		} // endwhile: iterate sub-sections
		m1++;
	} // endwhile: iterate design info sections
}

void CLocalizeRCDlg::MergeOldRCFileDialogLayout( CString &oldRCdata )
{
	// Preserve dialog layout changes made to accommodate translation string size
	// differences in a previously generated output RC file
	// Parameters:
	// oldRCdata		(in)	Reference containing buffered contents of existing RC data
	//							containing possible dialog layout updates (read only)
	//
	// Progress information is reported to the log output
	//
	CString msg; // For user messages
	int secitmsfound = 0; // Items found by regex parser
	int secitmsskipped = 0;
	int secitmsupdated = 0;
	LogUserMessage( IDS_LMERGINGOLDRC );
	boost::tregex dsetexpr( _T("^(.*?(?:,| ))( ?\\d+, *\\d+, *\\d+, *\\d+(, *\\d+)*)") );
	boost::tregex_iterator m1(boost::make_regex_iterator(oldRCdata, dsetexpr, boost::match_not_dot_newline)), mEnd;

	int nsecpos = -1; // Start position of a section key in new RC data
	int nendpos = -1; // Ending position of a section key in new RC data
	int nseclines = 0; // Number of lines of text in section in new RC data

	int secpos = -1; // Start position of a section key in old RC data
	int endpos = -1; // Ending position of a section key in old RC data
	int seclines = 0; // Number of lines of text in section in old RC data
	CString seckey; // Key for current section
	int iDEX = -1;

	// Find each size/pos data set, which is 4 or 5 numbers separated by commas, and replace
	// corresponding data set (according to preceding key name) in new RC output.
	while ( m1 != mEnd ) // Scan old RC data for size/pos data
	{
		// Get the item key and try to find it in the new RC output
		// In this the "key" is everything to the left of the size/pos hit on the line
		// which is not reliable due to text translations and static controls that lack a unqiue ID.
		// Logic compensates for this by verifying the two files match up by section key (which are
		// unique) and then using line offset within the matching section.
		CString key = CString( (*m1)[1].first, (*m1)[1].length() );
		CString val = CString( (*m1)[2].first, (*m1)[2].length() );
		CString val5 = CString( (*m1)[3].first, (*m1)[3].length() );

		// Skip any false hits that are not to be adjusted
		// Known exmaples are FILEVERSION and PRODUCTVERSION
		if ( key.Find(_T("FILEVERSION")) > -1 || key.Find(_T("PRODUCTVERSION")) > -1 ) // Skip false hits...
		{
			m1++;
			continue;
		}
		//
		if ( key.Find( _T(" DIALOG ")) > -1 || (iDEX=key.Find(_T(" DIALOGEX "))) > -1 ) // Key is a dialog section
		{
			if ( !seckey.IsEmpty() ) // Just finished a prior section - report stats
			{
				msg.FormatMessage( IDS_LDIALOGSECFOUND, seckey, secitmsfound, secitmsupdated, secitmsskipped );
				LogUserMessage( msg );
			}
			seckey = key.Trim(); // Start of new section (trim spaces - seckey is used in log output)
			secitmsfound = 0;
			secitmsskipped = 0;
			secitmsupdated = 0;
			nendpos = -1;
			nseclines = 0;
			// Try to find the matching dialog section in the new RC output data
			nsecpos  = newRCdata.Find( key ); // Find section key in new RC output, if present
			if ( nsecpos < 0 ) // Allow section types DIALOG and DIALOGEX to match each other
			{
				if ( iDEX > -1 ) // The section key is a DIALOGEX
					key.Replace( _T(" DIALOGEX "), _T(" DIALOG ") ); // Try DIALOG
				else // The section key is a DIALOG
					key.Replace( _T(" DIALOG "), _T(" DIALOGEX ") ); // Try DIALOGEX
				//
				nsecpos  = newRCdata.Find( key ); // One more try...
			}
			if ( nsecpos > -1 ) // Match was found in new RC data
			{
				// Get vital data on the dialog section in the new RC data
				nendpos  = newRCdata.Find( _T("END"), nsecpos ); // Find end marker for the section
				nseclines = _LineCount( newRCdata, nsecpos, nendpos ); // Compute number of lines of text in new section
				//
				// Get the same vital data on the matching dialog section in the old RC data
				secpos  = (*m1)[1].first - oldRCdata; // Offset to start of matching section key in old RC data
				endpos = -1;
				seclines = 0;
				if ( secpos > -1 ) // Safety check, should not normally fail
				{
					endpos  = oldRCdata.Find( _T("END"), secpos ); // Find end marker for the section in old RC data
					seclines = _LineCount( oldRCdata, secpos, endpos ); // Compute number of lines of text in old section
				}
				if ( seclines != nseclines ) // Don't attempt updates if sections are different due to changes
				{
					msg.FormatMessage( IDS_LSECSKIPSIZE, key );
					LogUserMessage( msg );
					nsecpos = -1;
					seckey.Empty(); // Clear out section key for next pass
				}
				//
			} 
			else // Key not found in new data
			{
				msg.FormatMessage( IDS_LSECSKIPNF, key );
				LogUserMessage( msg );
				nsecpos = -1;
				seckey.Empty(); // Clear out section key for next pass
			} // endif: match found in new RC data
		}
		if ( nsecpos > -1 ) // There's a matching section key in new RC data
		{
			// Attempt to update the size/pos values for the section entyr in the new RC data
			// using values from the matching line entry in the old RC data.
			//
			// Get line number within section for the current key in old RC data
			int keyline = _LineCount( oldRCdata, secpos, (*m1)[2].first - oldRCdata );
			// Get starting position of the matching lines within both RC data sets
			int nlinepos = _GetLinePos( newRCdata, nsecpos, keyline );
			if ( nlinepos > -1 ) // Must have valid line offset into new RC data
			{
				// Verify that at least the initial part of keyword on the line matches in both files to
				// ensure a key match. Note that entire key isn't compared because it likely won't match due
				// to translations of literal text.
				//
				// We will allow matches between xTEXT controls (CTEXT, LTEXT, and RTEXT), but not EDITTEXT
				CString newkey = newRCdata.Mid(nlinepos, 10); // Isolate keys to compare
				CString oldkey = key.Left(10);
				BOOL keymatch = (oldkey == newkey); // Initial match result
				if ( !keymatch ) // Match failed, check for allowable xTEXT key matchups
				{
					int keyxTextpos = oldkey.Find( _T("TEXT") ) - 1; // Position of xTEXT entry in old key or -2 if none
					int newkeyxTextpos = newkey.Find( _T("TEXT") ) - 1; // Position of xTEXT entry in new key or -2 if none
					if ( keyxTextpos > -1 && newkeyxTextpos > -1 ) // Both keys have an xTEXT entry
					{
						// If neither key is an EDITTEXT control then the match is allowed
						if ( oldkey.Mid(keyxTextpos,1).FindOneOf(_T("CLR")) > -1 &&
							 newkey.Mid(newkeyxTextpos,1).FindOneOf(_T("CLR")) > -1 )
							 keymatch = TRUE;
						//
					}
				}
				if ( keymatch ) // Merge size/pos data from matching old RC data key
				{
					// Locate the size/pos data within the line of new RC data and update it
					boost::tmatch mrslt;
					int nlinelen = newRCdata.Find( _T('\n'), nlinepos ) - nlinepos;
					CString sline = newRCdata.Mid( nlinepos, nlinelen ); // Substring to search
					if ( boost::regex_search( sline, mrslt, dsetexpr ) ) // Found it
					{
						CString nval = CString(mrslt[2].first, mrslt[2].length());
						CString nval5 = CString(mrslt[3].first, mrslt[3].length());
						int nmatchpos = nlinepos + mrslt[2].first - sline;
						int nmatchlen = mrslt[2].length();
						CString chk = newRCdata.Mid( nmatchpos, nmatchlen );
						if ( nmatchpos >= nlinepos && nmatchlen > 0 && chk != val )
						{
							newRCdata.Delete( nmatchpos, nmatchlen );
							newRCdata.Insert( nmatchpos, val );
							secitmsupdated++;
						}
					}
					else // Unexpected failure to match size/pos data
					{
						// Log it
						CString msg;
						msg.FormatMessage( IDS_LERRSPFAIL, key.Left(10), sline );
						LogUserMessage( msg );
					}
				}
				else // Key not found in new RC output
				{
					// Log it
					CString msg;
					msg.FormatMessage( IDS_LKEYSKIPPED, key, newRCdata.Mid(nlinepos, key.GetLength()) );
					LogUserMessage( msg );
					secitmsskipped++;
				} // endif: verify key line match
			} // endif: line offset to new RC data is valid
		} // endif: there's a matching section
		secitmsfound++;
		m1++;
	} // endwhile: scan old RC data for size/pos values
	//
	// Report any stats for last section processed
	if ( !seckey.IsEmpty() )
	{
		msg.FormatMessage( IDS_LDIALOGSECFOUND, seckey, secitmsfound, secitmsupdated, secitmsskipped );
		LogUserMessage( msg );
	}
}

void CLocalizeRCDlg::OnBnClickedCreateini()
{
	// Reload InputRC
	if( !OpenInputRC() )
		return;
	// Write/Actualize INI-File
    if( WriteReadIni(true) == OLDFILEFORMAT )
		LogUserMessage( IDS_OLDFILEFORMAT );
}

// extract first caption after *pnPosition
CString CLocalizeRCDlg::ExtractCaption(CString& strText, int* pnPosition, CString strKeyword, CString &strIDC )
{
	CString strCaption, strLine;
	int nStart, nEnd, nStartQuote, nEndQuote;

	nStart = *pnPosition;

	// DLGINIT: Microsoft-Format to store data for comboboxes
	if( strKeyword == _T("DLGINIT") )
	{		
		if( nStart == 0 )
		{
			// remove BEGIN and END
			nStart = FindSeperateWord( strText, _T("BEGIN"), 0 );
			if( nStart != -1 )
				nStart += 7;	
		}

		// FORMAT: <IDC>, 0x403, <LENGTH IN BYTES>, 0
		// FORMAT: <WORD> as Hex Wert chars in LOBYTE und HIGHBYTE
		// comma seperated, null-terminated, ends with comma

		// comma seperated, null-terminated, ends with comma
		CString strHelp;
		LPTSTR pHelp;
		int nWord, nToken = 0, nLength = 0;

		CString strToken = StringTokenize( strText, _T(",\r\n"), &nStart);
		while (nStart != -1)
		{
			strToken.TrimLeft();
			strToken.TrimRight();
			if( strToken.IsEmpty() )
			{
				strToken = StringTokenize( strText, _T(",\r\n"),&nStart);
				continue;
			}

			switch( nToken )
			{
				case 0:		// IDC	oder 0 bei Ende von Liste
					if( strToken == "0" )
					{
						*pnPosition = nStart;
						return strCaption;
					}
					else // IDC
					{
						if( strIDC.IsEmpty() )
						{
							strIDC = strToken; 	// new IDC
						}
						else
						{
							if( strIDC != strToken )
								return strCaption;       
						}
					}

					break;
				case 1:		// 0x403 for Combo-Boxes
					if( strToken != "0x403" )
						return ERR_STR;
					break;
				case 2:		// Length of String
					nLength = _ttoi( strToken );
					nLength = (int)floor((nLength / 2.0)+0.5);
					break;
				case 3:		// always 0
					break;
				default:	// everything above 3 is word values
					if( nToken <= nLength + 3 )
					{
						// convert string to hexadecimal integer
						nWord = _tcstol( strToken, &pHelp, 16 );
						strHelp.Format( _T("%c%c"), LOBYTE(nWord), HIBYTE(nWord) );
						strHelp.Remove( '\0' );
						strCaption += strHelp;
						if( nToken == nLength + 3 )	// last word
						{
							strCaption += ";";
							nToken = -1;
						}
					}
			}
			nToken++;
			*pnPosition = nStart;
			strToken = StringTokenize(strText, _T(",\r\n"), &nStart);
		}
		return ERR_STR;
	}	

	// find end of line
	while( (nEnd = strText.Find( _T("\r\n"), nStart )) != -1 )
	{
		// strLine contents one command line
		strLine += strText.Mid( nStart, nEnd - nStart );
					
		if( strLine.GetLength() <= 0 )
		{
			// next line
			nStart = nEnd+2;
			*pnPosition = nStart;
			continue;
		}
		
		
		int rPos = strLine.GetLength()-1;
		// skip spaces at the end of line
		while (rPos > 0 && strLine[rPos] == ' ')
		{
			rPos--;
		}

		// merge multiline commands to one line
		if( (strLine[rPos] == '|') || 
			(strLine[rPos] == ',') )
		{
			// next line
			nStart = nEnd+2;
			continue;
		}

		TCHAR chHelp;
		if( nStart > 0 )
			chHelp = strText[nStart-1];

		// search for first " (is never within "") so even empty strings ("") are found
		nStartQuote = strLine.Find( _T('"') );
		if ( nStartQuote >= 0 )
			nStartQuote++;

		if( nStartQuote == -1  || !MustBeTranslated(strLine, strKeyword) )	// not found
		{
			strLine = "";
			nStart = *pnPosition = nEnd+2;
			continue;
		}
		// find last " and ignore "" (quotation marks within strings)
		nEndQuote = FindQuote( strLine, nStartQuote );
		if( nEndQuote == -1 )
		{
			strLine = "";
			nStart = *pnPosition = nEnd+2;
			continue;
		}

		nStartQuote--;
		chHelp = strText[nStart+nStartQuote];
		strCaption = strLine.Mid( nStartQuote, nEndQuote-nStartQuote );
		*pnPosition += nEndQuote;
		chHelp = strText[*pnPosition];
		return strCaption;
	}
	return ERR_STR;
}

void CLocalizeRCDlg::OnBnClickedOpenini()
{
	if( m_strLangINI.IsEmpty() )
		LogUserMessage( IDS_ERR_FILENAMEEMPTY );
	else
	{
		// open lang.ini with standard association
		HINSTANCE hInstance = ShellExecute( m_hWnd, _T("open"), m_strLangINI, NULL, NULL, SW_SHOWNORMAL );
		if( (int)hInstance <= 32 )
			ShowError( IDS_ERR_OPENINI, false, (DWORD)hInstance );
	}
}

void CLocalizeRCDlg::OnBnClickedCreateoutput()
{	
	UpdateData( true );
	int nResult = WriteReadIni( false );
	if( nResult == OLDFILEFORMAT )
	{
		AfxMessageBox( IDS_OLDFILEFORMAT );
		return;
	}
	if( nResult <= 0 )
		return; 
	UpdateData( false );

	// create output RC
	if( m_strOutputRC.IsEmpty() )
	{
		AfxMessageBox( IDS_ERR_FILENAMEEMPTY );
		return;
	}
	//
	// If a previous output RC exists then attempt to merge dialog size/pos changes in it with the
	// the newly translated input RC data produced by WriteReadIni (newRCdata)
	CString oldRCdata;

	if ( OpenRCFile(m_strOutputRC, oldRCdata, false) ) // Read contents of old output RC into buffer
	{
		// Preserve any dialog layout changes made in a prior RC file
		MergeOldRCFileDialogLayout( oldRCdata );
		MergeOldRCFileDesignInfo( oldRCdata );
		if( !m_bCopy ) // Remove Res folder references if Res folder isn't being copied
			RemoveNewRCFileRESItems( );
		//
	} // endif: old RC file exists

	try
	{
		CFile File( m_strOutputRC, CFILEFLAG_UNICODEHELPER | CFile::modeWrite|CFile::modeCreate );
	#ifdef UNICODE
		// write 0xFF 0xFE
		BYTE fffe[2] = { 0xFF, 0xFE };
		File.Write(fffe, 2);
	#endif
		File.Write( newRCdata, sizeof(TCHAR) * newRCdata.GetLength() );
		File.Close();
	}
	catch( CFileException* e )
	{
		TCHAR szCause[255];
		e->GetErrorMessage(szCause, 255);
		AfxMessageBox(szCause);
		e->Delete();
	}

	// copy header and RES-Folder
	if( m_bCopy )
	{
		CString strOutputFolder = GetFolder( m_strOutputRC );
		CString strInputFolder = GetFolder( m_strInputRC );
        		
		// copy header
		if( !CopyFile( strInputFolder+_T("resource.h"), strOutputFolder+_T("resource.h"), false ) )
			ShowError( IDS_ERR_FILECOPY, true );
		
		// create folder
		if( !CreateDirectory( strOutputFolder+_T("RES"), NULL ) )
		{
			DWORD dwErrCode = GetLastError();
			if( dwErrCode != ERROR_ALREADY_EXISTS )
			{
				ShowError( IDS_ERR_FOLDERCREATE, false, dwErrCode );
				return;
			}
		}

		CFileFind FFind;
		BOOL bWorking = FFind.FindFile( strInputFolder+_T("RES\\*.*"), 0 );
		while( bWorking ) 
		{
			bWorking = FFind.FindNextFile();

			// skip . and .. files; otherwise, we'd
			// recur infinitely!

			if (FFind.IsDots() || FFind.IsDirectory())
				continue;

			else
			{
				// copy file
				if( !CopyFile( FFind.GetFilePath(), strOutputFolder+_T("RES\\")+FFind.GetFileName(), false ) )
				{
					ShowError( IDS_ERR_FILECOPY, true );
				}
			}
		}
		FFind.Close();
	}
	// Fill Edit with InputRC again
	OpenInputRC();
}


// checks if line contents strings that have to be translated
bool CLocalizeRCDlg::MustBeTranslated(CString strLine, CString strKeyword )
{
	// if it is stringtable -> translate
	if( strKeyword == ::strKeyword[5] )
		return true;

	bool bTranslate = true;

	strLine.TrimLeft(_T(" "));
	strLine.TrimRight(_T(" "));
		
	if(strLine[0] == '#') // Preprocessor line
		bTranslate = false;

	if(strLine[0] == '/') // Comment line
		bTranslate = false;

	//--- Exclude following controls from translation ---------------------
	if(strLine.Find(_T("msctls_updown32"), 0) > 0) // Spin control
		bTranslate = false;

	if(strLine.Find(_T("SysTreeView32"), 0) > 0) // Tree view control
		bTranslate = false;

	if(strLine.Find(_T("msctls_trackbar32"), 0) > 0) // Slider control
		bTranslate = false;

	if(strLine.Find(_T("SysIPAddress32"), 0) > 0) // IP adress
		bTranslate = false;

	if(strLine.Find(_T("msctls_hotkey32"), 0) > 0) // Hot key
		bTranslate = false;

	if(strLine.Find(_T("SysListView32"), 0) > 0) // List view control
		bTranslate = false;

	if(strLine.Find(_T("SysAnimate32"), 0) > 0) // Animate control
		bTranslate = false;

	if(strLine.Find(_T("SysMonthCal32"), 0) > 0) // Month calendar
		bTranslate = false;

	if(strLine.Find(_T("ComboBoxEx32"), 0) > 0) // Extended combo box
		bTranslate = false;

	if( !bTranslate )
		return false;

	bTranslate = false;

	//--- Include following controls into translation --------------- 
	if(strLine.Find(_T("CAPTION"), 0) == 0)
		bTranslate = true;					// Dialog box caption

	if(strLine.Find(_T("POPUP"), 0) == 0)
		bTranslate = true;

	if(strLine.Find(_T("MENUITEM"), 0) == 0)
		bTranslate = true;	

	if(strLine.Find(_T("PUSHBUTTON"), 0) == 0)
		bTranslate = true;

	if(strLine.Find(_T("DEFPUSHBUTTON"), 0) == 0)
		bTranslate = true;

	if(strLine.Find(_T("LTEXT"), 0) == 0)
		bTranslate = true;

	if(strLine.Find(_T("RTEXT"), 0) == 0)
		bTranslate = true;

	if(strLine.Find(_T("CTEXT"), 0) == 0)
		bTranslate = true;

	if(strLine.Find(_T("GROUPBOX"), 0) == 0)
		bTranslate = true;

	if(strLine.Find(_T("CONTROL"), 0) == 0)
	{ 
		if(strLine.Find(_T("BS_AUTORADIOBUTTON"), 0) != -1 )
			bTranslate = true;

		if(strLine.Find(_T("BS_AUTOCHECKBOX"), 0) != -1 )
			bTranslate = true;

		if( strLine.Find(_T("Button"), 0) != -1 )
			bTranslate = true;

		// statics with SS_LEFTNOWORDWRAP are controls
		if(strLine.Find(_T("SS_LEFTNOWORDWRAP"), 0) != -1 )
			bTranslate = true;
	}
	if( strLine.Find( _T("ID"), 0) == 0)	// stringtable
		bTranslate = true;
	
	if(strLine.Find( _T("AFX_IDS_"), 0) == 0)
		bTranslate = true;

	if( strLine[0] == '"' )				// accelerator
		bTranslate = true;

    return bTranslate;
}

// return the position where '"' was found
int CLocalizeRCDlg::FindQuote(CString strLine, int nStartPos)
{
	int nPosition = nStartPos;
	
	// search for '"', that is not a quotation mark inside the text ("")
	do 
	{
		nPosition = strLine.Find( '"', nPosition );
		// no '"' was found
		if( nPosition == -1 )
			return nPosition;

		nPosition++;

		if( nPosition >= strLine.GetLength()-1 )
			return nPosition;
	}
	while( strLine[nPosition++] == '"' );

	return nPosition-1;
}

#define PREFIX_CHANGEDITEM _T("*")

int CLocalizeRCDlg::WriteReadIni(bool bWrite)
{
	BYTE nKeyword;
	
	if( m_strLangINI.IsEmpty() )
	{
		LogUserMessage( IDS_ERR_FILENAMEEMPTY );
		return false;
	}

	if ( bWrite )
		LogUserMessage( IDS_LINIOPENING );
	else
		LogUserMessage( IDS_LINICHECKING );
	//

	// Create or Open INI-File
	CIniEx IniEx;
	if( !IniEx.Open( m_strLangINI ) )
		return false;

	// old sectionnames in INI file ?
	LPCTSTR strOldKeywords[5] =
	{
		_T(" ACCELERATORS "),
		_T(" DIALOG "),
		_T(" DIALOGEX "),
		_T(" MENU "),
		_T(" MENUEX ")
	};

	for( nKeyword = 0; nKeyword < 5; nKeyword++ )
	{
		CString strHelp = strOldKeywords[nKeyword];
		if( IniEx.LookupSection(&strHelp) != -1 )
			return OLDFILEFORMAT;
	}

	CString strCaption, strHelp, strValue, strIDC;
	int nStart, nHelp, nPosition, nSelStart, nSelEnd, nOldLength, nNewLength, nPrevHelp;

	LogUserMessage( IDS_LINIPROCESSING );
	for( nKeyword = 0; nKeyword < NUMKEYWORDS-1; nKeyword++ )
	{
		nPosition = 0;
		// search for keyword section in RC-file
		while( (nPosition = FindSeperateWord( newRCdata, strKeyword[nKeyword], nPosition )) != -1 )
		{
			nStart = nPosition;
			
			// find BEGIN
			nHelp = FindSeperateWord( newRCdata, _T("BEGIN"), nPosition );
			
			// find related END (ignore interlocked BEGINs-ENDs)
			nPosition = nHelp-1;
			do
			{
				nPosition = FindSeperateWord( newRCdata, _T("END"), nPosition+1);
				nHelp = FindSeperateWord( newRCdata, _T("BEGIN"), nHelp+1 );
			}
			while( nHelp < nPosition && nHelp != -1 );
			
			// save SECTION in strHelp
			strHelp = newRCdata.Mid( nStart, nPosition-nStart );
			
			nHelp = nPrevHelp = 0;

			// extract caption
			while( (strCaption = ExtractCaption( strHelp, &nHelp, strKeyword[nKeyword], strIDC )) != ERR_STR )
			{
				if( bWrite )
				{
					// check if key in Lang.INI already exists
					strValue = IniEx.GetValue( strKeyword[nKeyword], strCaption );
					if( strValue.IsEmpty() )
					{
						// insert line in Lang.INI
						IniEx.SetValue( strKeyword[nKeyword], strCaption, PREFIX_CHANGEDITEM+strCaption );
					}
					else
					{
						// insert line in Lang.INI
						IniEx.SetValue( strKeyword[nKeyword], strCaption, PREFIX_CHANGEDITEM+strValue );
					}
				}
				else
				{
					// check if key in Lang.INI exists
					strValue = IniEx.GetValue( strKeyword[nKeyword], strCaption );
					if( !strValue.IsEmpty() )
					{
						// if it is DLGINIT
						if( nKeyword == 6 )
						{
							CString strInsert, strToken, strHelp2;
							WORD wData;
							int n;
							int nTokenPos = 0;

							while( (strToken = StringTokenize( strValue, _T(";"), &nTokenPos )) != "" )
							{
								// first line: header
								strHelp2.Format(_T("\t%s, 0x403, %d, 0\r\n"), strIDC, strToken.GetLength()+1 );
								strInsert += strHelp2;

								// second line: data
								for( n=0; n < strToken.GetLength(); n+=2 )
								{
									if( n == (strToken.GetLength() - 1) )
										wData = MAKEWORD( strToken.GetAt(n), 0 );
									else
										wData = MAKEWORD( strToken.GetAt(n), strToken.GetAt(n+1) );
									strHelp2.Format( _T("0x%04x, "), wData );
									strInsert += strHelp2;
								}

								// eventually add null character
								if( n == strToken.GetLength() )
								{
									strInsert += _T("\"\\000\"");
								}
								strInsert += _T("\r\n");
							}
							// only add 0 if DLGINIT block is at the end
							if( nHelp >= strHelp.GetLength() -1 )
							{
								strInsert += _T("\t0\r\n");
								nSelEnd = strHelp.GetLength();
							}
							else
								nSelEnd = nHelp;

							// find BEGIN
							if( nPrevHelp == 0 )
							{
								nSelStart = FindSeperateWord(strHelp, _T("BEGIN"), 0 );
								nSelStart += 7;
							}
							else
								nSelStart = nPrevHelp;

							nOldLength = nSelEnd-nSelStart;
							strHelp.Delete( nSelStart, nOldLength  );
							nNewLength = strInsert.GetLength();
							strHelp.Insert( nSelStart, strInsert );	
						}
						else
						{
							nSelStart = nHelp-strCaption.GetLength();
							nSelEnd = nHelp;

							nOldLength = nSelEnd-nSelStart;
							strHelp.Delete( nSelStart, nOldLength  );
							nNewLength = strValue.GetLength();
							strHelp.Insert( nSelStart, strValue );	
						}
						nHelp += nNewLength - nOldLength;
						if( nHelp < 0 )
							nHelp = 0;
					}
					nPrevHelp = nHelp;
				}
				strIDC.Empty();
			}
			if( !bWrite )
			{
				newRCdata.Delete( nStart, nPosition-nStart );
				newRCdata.Insert( nStart, strHelp );
				nPosition = nStart + strHelp.GetLength();
			}
		}
	}
#define CODEPAGE _T("code_page")
	// search for codepage
	////////////////////////////////////////////
	nPosition = 0;
	CString strCodepage;
	while( (nPosition = FindSeperateWord( newRCdata, CODEPAGE, nPosition )) != -1 )
	{
		nSelEnd = newRCdata.Find( ')', nPosition );
		nSelStart = newRCdata.Find( '(', nPosition ) + 1;

		strCaption = newRCdata.Mid( nSelStart, nSelEnd-nSelStart );
		
		// check if key in Lang.INI already exists
		strValue = IniEx.GetValue( CODEPAGE, strCaption );

		if( bWrite )
		{
	
			if( strValue.IsEmpty() )
			{
				// insert line in Lang.INI
				IniEx.SetValue( CODEPAGE, strCaption, PREFIX_CHANGEDITEM+strCaption );
			}
			else
			{
				// insert line in Lang.INI
				IniEx.SetValue( CODEPAGE, strCaption, PREFIX_CHANGEDITEM+strValue );
			}
		}
		else
		{
			if( !strValue.IsEmpty() )
			{
				newRCdata.Delete( nSelStart, nSelEnd-nSelStart );
				newRCdata.Insert( nSelStart, strValue );
				nPosition = nSelStart + strValue.GetLength();
			}
		}
		nPosition++;
	}

	if( bWrite )
	{
		// remove unnecessary lines in Lang.INI
		CStringArray strSections, strKeys;
		IniEx.GetSections( strSections );

		// go through every section
		int nSection=0;
		while( nSection < strSections.GetSize() )
		{
			strKeys.RemoveAll();
			IniEx.GetKeysInSection( strSections[nSection], strKeys );
			for( nKeyword = 0; nKeyword < NUMKEYWORDS; nKeyword++ )
			{
				// dont touch unknown sections
				if( _tcscmp(strSections[nSection], strKeyword[nKeyword]) != 0 )
					continue;

				// Go through every key in this section
				int nKey=0;
				while( nKey < strKeys.GetSize() )
				{
					// check if key is necessary for this RC
					strValue = IniEx.GetValue( strSections[nSection], strKeys[nKey] );
					if( strValue[0] == '*' )
					{
						strValue.TrimLeft( _T(" *") );
						IniEx.SetValue( strSections[nSection], strKeys[nKey], strValue );
					}
					else
					{
						// key is obsolete and no longer needed
						switch( m_nObsoleteItems )
						{
							case 0:	// delete item	
								IniEx.RemoveKey( strSections[nSection], strKeys[nKey]);
								break;
							case 1:
								if( strValue[0] != '#' )
									IniEx.SetValue( strSections[nSection], strKeys[nKey], _T("#") + strValue );
								break;
						}
					}
					nKey++;
				}
				break;
			}
			if( nKeyword == NUMKEYWORDS && strSections[nSection] != CODEPAGE  )
				IniEx.SetValue( strSections[nSection], _T("!!! This sectionname isn't recognized by LocalizeRC. Probably you can delete the whole section !!!"), _T("") );

			strKeys.RemoveAll();
			IniEx.GetKeysInSection( strSections[nSection], strKeys );
			
			// remove empty sections
			if( strKeys.GetSize() == 0 )
			{
				if( IniEx.RemoveSection( strSections[nSection] ) )
					strSections.RemoveAt( nSection );
				else
					nSection++;
			}
			else
				nSection++;
		}
		if( !m_bNoSort )
			IniEx.SortIniValues();
		// save changes
		IniEx.WriteFile();
	}
	if ( bWrite )
		LogUserMessage( IDS_LINIFINISHED );
	else
		LogUserMessage( IDS_LINICHECKDONE);
	//
	return true;
}

void CLocalizeRCDlg::OnDestroy()
{
	CDialog::OnDestroy();
	DestroyIcon( hSmallIcon );
	DestroyIcon( hLargeIcon );

	AfxGetApp()->WriteProfileString( SEC_LASTPROJECT, ENT_WORKSPACE, m_strWorkspace );
	SaveWorkspace();
}	

CString CLocalizeRCDlg::GetFolder(CString strPath)
{
	TCHAR path[_MAX_PATH];
	TCHAR drive[_MAX_DRIVE];
	TCHAR dir[_MAX_DIR];

	_tcscpy_s( path, _MAX_PATH, strPath );
	
	// Trenne Pfad von Anwendungsnamen
	_tsplitpath_s( path, drive, _MAX_DRIVE, dir, _MAX_DIR, NULL, 0, NULL, 0 );

	CString strReturn;
	strReturn.Format( _T("%s%s"), drive, dir);
	return strReturn;
}

void CLocalizeRCDlg::OnBnClickedReverseini()
{
	// Create or Open INI-File
	CIniEx IniEx;
	IniEx.Open( m_strLangINI );

	CStringArray strSections, strKeys;
	CString strHelp;
	IniEx.GetSections( strSections );
	int nKey, nSec;
	
	// go through every section
	for( nSec=0; nSec < strSections.GetSize(); nSec++ )
	{
		// go through every key
		IniEx.GetKeysInSection( strSections[nSec], strKeys );
		for( nKey=0; nKey < strKeys.GetSize(); nKey++ )
		{
			strHelp = IniEx.GetValue( strSections[nSec], strKeys[nKey] );	
			if( strHelp != strKeys[nKey] )
			{
				IniEx.SetValue( strSections[nSec], strHelp, strKeys[nKey] );
				IniEx.RemoveKey( strSections[nSec], strKeys[nKey] );
			}
		}
	}
	// save changes
	IniEx.WriteFile();
	LogUserMessage( IDS_LINIKEYSREVERSED );
}

void CLocalizeRCDlg::OnBnClickedNewWorkspace()
{
	if( OpenSaveDialog(false, false, IDS_EXTLWS, IDS_EXTLWSDESCRIPTION, m_strWorkspace, _T("")) )
	{
		m_strLangINI = "";
		m_strInputRC = "";
		m_strOutputRC = "";
		UpdateData( false );
	}
}

void CLocalizeRCDlg::OnBnClickedChngWorkspace()
{
	if( OpenSaveDialog(true, false, IDS_EXTLWS, IDS_EXTLWSDESCRIPTION, m_strWorkspace, _T("")) )
	{
		if( !LoadWorkspace() )
			AfxMessageBox( IDS_ERR_OPENWORKSPACE );
	}
}

BOOL CLocalizeRCDlg::LoadWorkspace(BOOL bShowError)
{
	CIniEx IniEx;
	if( !IniEx.Open( m_strWorkspace, 1, 0 ) )
		return false;

	CString strPath = GetFolder(m_strWorkspace);
	m_strInputRC = GetAbsolutePathFromIni( &IniEx, ENT_INPUTRC, strPath );
	m_strLangINI = GetAbsolutePathFromIni( &IniEx, ENT_LANGINI, strPath );
	m_strOutputRC = GetAbsolutePathFromIni( &IniEx, ENT_OUTPUTRC, strPath );
	m_bCopy = _ttoi( IniEx.GetValue(ENT_COPY) );
	m_nObsoleteItems = _ttoi( IniEx.GetValue(ENT_OBSITEMS) );
	m_bNoSort = _ttoi( IniEx.GetValue(ENT_NOSORT) );
	OpenInputRC(bShowError);

	LogUserMessage( IDS_LWSPACELOADED );
	return true;
}

BOOL CLocalizeRCDlg::SaveWorkspace(void)
{
	CIniEx IniEx;
	if( !IniEx.Open( m_strWorkspace ) )
		return false;

	// save settings to Workspace
	CString strHelp;
	strHelp.Format( _T("%d"), m_bCopy );
	IniEx.SetValue( ENT_COPY, strHelp );
	strHelp.Format( _T("%d"), m_nObsoleteItems );
	IniEx.SetValue( ENT_OBSITEMS, strHelp );
	strHelp.Format( _T("%d"), m_bNoSort );
	IniEx.SetValue( ENT_NOSORT, strHelp );
	
	// save changes
	IniEx.WriteFile();

	return 0;
}

CString CLocalizeRCDlg::GetAbsolutePathFromIni(CIniEx* pIniEx, CString strKey, CString strPath)
{
	CString strHelp = pIniEx->GetValue( strKey );
	CString strValue;
	if( strHelp.IsEmpty() )
		return _T("");

	if( PathIsRelative( strHelp ) )
	{
		TCHAR szPath[MAX_PATH];
		PathCombine( szPath, strPath, strHelp );
		strHelp.Format(_T("%s"), szPath);
		return strHelp;
	}
	return strHelp;
}

void CLocalizeRCDlg::OnCbnSelchangeLanguage()
{
	// Save Changes
	AfxGetApp()->WriteProfileInt( SEC_LASTPROJECT, ENT_LANGID, m_CtrlLanguage.GetItemData(m_CtrlLanguage.GetCurSel()) );
	AfxMessageBox( IDS_RESTARTAPP );
}

#define STR_LEN 64

int CLocalizeRCDlg::AddLanguage(CComboBox* pComboBox, LPCTSTR strLangCode, LANGID SelectedID)
{
	LCID lcID;
	TCHAR szLangName[STR_LEN];
	LPTSTR pHelp;
	int nIndex;

	lcID =  MAKELCID( MAKELANGID(_tcstoul( strLangCode, &pHelp, 16 ), SUBLANG_NEUTRAL ), SORT_DEFAULT );
	GetLocaleInfo( lcID, LOCALE_SNATIVELANGNAME , szLangName, STR_LEN);
	nIndex = pComboBox->AddString( szLangName );
	pComboBox->SetItemData( nIndex, lcID );
	if( lcID == SelectedID )
		pComboBox->SetCurSel( nIndex );
	return nIndex;
}

CString CLocalizeRCDlg::StringTokenize(CString strSource, LPCTSTR pszTokens, int* pnStart)
{
#if _MFC_VER >= 0x0700
	return strSource.Tokenize( pszTokens, *pnStart );
#else
	// original code CString::Tokenize from MFC7
	/////////////////////////////////////////////////

	if( pszTokens == NULL )
	{
		return( strSource );
	}

	// (LPCSTR)(LPCTSTR) hack for VC6.
	LPCTSTR pszPlace = (LPCTSTR)strSource+*pnStart;
	LPCTSTR pszEnd = (LPCTSTR)strSource+strSource.GetLength();
	if( pszPlace < pszEnd )
	{
		int nIncluding = StringSpanIncluding( pszPlace, pszTokens );

		if( (pszPlace+nIncluding) < pszEnd )
		{
			pszPlace += nIncluding;
			int nExcluding = StringSpanExcluding( pszPlace, pszTokens );

			int iFrom = *pnStart+nIncluding;
			int nUntil = nExcluding;
			*pnStart = iFrom+nUntil+1;

			return( strSource.Mid( iFrom, nUntil ) );
		}
	}

	// return empty string, done tokenizing
	*pnStart = -1;

	return( CString( "" ) );
#endif
}

#define SEPERATORS _T(" \r\n,\t()")

int CLocalizeRCDlg::FindSeperateWord(CString strText, LPCTSTR strWord, int nStartPos)
{
	int nFoundPos;
	while( (nFoundPos = strText.Find( strWord, nStartPos )) != -1 )
	{
        CString strSeperator;

		// look for preceding character
		if( nFoundPos > 0 )
		{
			strSeperator = strText[nFoundPos-1];
			// if preceding character isn't a separator, continue search
			if( strSeperator.FindOneOf( SEPERATORS ) == -1 )
			{
//				strSeperator = strText.Mid( nFoundPos-1, nFoundPos + 40 );
				
				nStartPos = nFoundPos+_tcslen(strWord);
//				nStartPos = nFoundPos+1;
				continue;
			}
		}

		// look for successing character
		int nSuccessingPos =  nFoundPos+_tcslen(strWord);
		if( nSuccessingPos < strText.GetLength() )
		{
			strSeperator = strText[nSuccessingPos];

			// if successing character isn't a separator, continue search
			if( strSeperator.FindOneOf( SEPERATORS ) == -1 )
			{
				nStartPos = nSuccessingPos;
//				nStartPos = nFoundPos+1;
				continue;
			}
		}
		break;
		
	}
	return nFoundPos;
}

void CLocalizeRCDlg::OnCbnSelchangeObsItems()
{
	UpdateData( true );
}

void CLocalizeRCDlg::OnBnKillfocusCopy()
{
	UpdateData( true );
}

void CLocalizeRCDlg::OnBnKillfocusNosort()
{
	UpdateData( true );
}

void CLocalizeRCDlg::OnBnClickedChngInputrc()
{
	// Change Input RC
	if( OpenSaveDialog( true, true, IDS_EXTRC, IDS_EXTRCDESCRIPTION, m_strInputRC, ENT_INPUTRC ) )
		OpenInputRC();
}

void CLocalizeRCDlg::OnBnClickedChngLangini()
{
	// Change Lang INI
	OpenSaveDialog( false, true, IDS_EXTINI, IDS_EXTINIDESCRIPTION, m_strLangINI, ENT_LANGINI );	
}

void CLocalizeRCDlg::OnBnClickedChngOutputrc()
{
	// Change Output RC
	OpenSaveDialog( false, true, IDS_EXTRC, IDS_EXTRCDESCRIPTION, m_strOutputRC, ENT_OUTPUTRC );
}

BOOL CLocalizeRCDlg::OpenSaveDialog(BOOL bOpen, BOOL bRelative, UINT nExtID, UINT nExtDescriptionID, CString& strEdit, CString strIniEntry)
{
	CString strExtension, strExtensionInfo;
	strExtension.LoadString( nExtID );
	strExtensionInfo.LoadString( nExtDescriptionID ); 

	DWORD dwFlags = OFN_HIDEREADONLY|OFN_ENABLESIZING|OFN_EXPLORER;
	if( bOpen )
		dwFlags |= OFN_FILEMUSTEXIST;
	else
		dwFlags |= OFN_OVERWRITEPROMPT;

#if _MFC_VER >= 0x0700
	CFileDialog FileDialog( bOpen, strExtension, NULL, dwFlags,	strExtensionInfo, this, 0 );
#else
	CFileDialog FileDialog( bOpen, strExtension, NULL, dwFlags,	strExtensionInfo, this );
#endif
	_tcscpy_s( FileDialog.m_ofn.lpstrFile, _MAX_PATH, strEdit );

	if( FileDialog.DoModal() == IDOK )
	{
		if( bRelative )
		{
			// change to relative path
			TCHAR szOut[MAX_PATH] = _T("");
			if( !PathRelativePathTo( szOut, m_strWorkspace, FILE_ATTRIBUTE_NORMAL, FileDialog.GetPathName(), FILE_ATTRIBUTE_NORMAL ) )
				_tcscpy_s( szOut, MAX_PATH, FileDialog.GetFileName() );
			if( strIniEntry )
			{
				CIniEx IniEx;
				
				if( !IniEx.Open( m_strWorkspace ) )
					return false;
				
				CString strValue = szOut;
				IniEx.SetValue( strIniEntry, strValue );
				IniEx.WriteFile();
			}
		}
		strEdit = FileDialog.GetPathName();

		UpdateData( false );
		return true;
	}
	
	return false;
}
BOOL CLocalizeRCDlg::ShowError(UINT nIDString1, bool bGetLastError, DWORD dwErrCode )
{
	CString strString1, strLastError;
	
	if( dwErrCode == 0 )
		dwErrCode = GetLastError();

	strString1.LoadString( nIDString1 );
	if( bGetLastError )	
	{
		strLastError = ConvertErrorToString( dwErrCode );
		strString1 += _T(" ") + strLastError;
	}
	AfxMessageBox( strString1 );
	return true;
}


#include <lmerr.h>
CString CLocalizeRCDlg::ConvertErrorToString( DWORD dwErrCode )
{	
	HMODULE 	hModule = NULL;		// default to system source
	LPTSTR		MessageBuffer = NULL;
	DWORD		dwBufferLength;
	CString		strError;
 
	// Always start off with an empty string
	strError.Empty();

   	// if error_code is in the network range, load the message source
	if (dwErrCode >= NERR_BASE && dwErrCode <= MAX_NERR) 
	{
	    hModule = ::LoadLibraryEx( _TEXT("netmsg.dll"), NULL, LOAD_LIBRARY_AS_DATAFILE );
	}
 
	
	// call FormatMessage() to allow for message text to be acquired
	// from the system or the supplied module handle
	dwBufferLength = ::FormatMessage( 
	    FORMAT_MESSAGE_ALLOCATE_BUFFER |
	    FORMAT_MESSAGE_IGNORE_INSERTS |
	    FORMAT_MESSAGE_FROM_SYSTEM |		// always consider system table
	    ((hModule != NULL) ? FORMAT_MESSAGE_FROM_HMODULE : 0),
	    hModule,					// module to get message from (NULL == system)
	    dwErrCode,
	    MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),	// default language
	    (LPTSTR) &MessageBuffer, 0, NULL );

    if ( MessageBuffer ) 
	{
	    if ( dwBufferLength )
			strError = (LPCTSTR)MessageBuffer;
 
	    // free the buffer allocated by the system
	    ::LocalFree(MessageBuffer);
	}
 
	// if you loaded a message source, unload it
	if (hModule != NULL)
	    ::FreeLibrary(hModule);

    if ( strError.GetLength() == 0 )
	    strError.Format( IDS_ERR_UNKNOWN, dwErrCode );
	
	return strError;
}
