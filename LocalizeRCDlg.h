// LocalizeRCDlg.h : header file
//

#pragma once
#include <tchar.h>
#include "afxcmn.h"
#include "afxwin.h"

// CLocalizeRCDlg dialog
class CLocalizeRCDlg : public CDialog
{
// Construction
public:
	CLocalizeRCDlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
	enum { IDD = IDD_LOCALIZERC_DIALOG };

	LANGID LangID;

protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support


// Implementation
private:
	HICON hLargeIcon, hSmallIcon;

	// Generated message map functions
	virtual BOOL OnInitDialog();

	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg void OnBnClickedNewWorkspace();
	afx_msg void OnBnClickedChngWorkspace();
	afx_msg void OnBnClickedReverseini();
	afx_msg void OnCbnSelchangeLanguage();
	afx_msg void OnDestroy();
	afx_msg void OnBnClickedCreateini();
	afx_msg void OnBnClickedOpenini();
	afx_msg void OnBnClickedCreateoutput();
	afx_msg void OnCbnSelchangeObsItems();
	afx_msg void OnBnKillfocusCopy();
	afx_msg void OnBnKillfocusNosort();
	afx_msg void OnBnClickedChngInputrc();
	afx_msg void OnBnClickedChngLangini();
	afx_msg void OnBnClickedChngOutputrc();

	DECLARE_MESSAGE_MAP()

	// content of rich edit control
	CString m_strEdit;

	// absolute filenames
	/////////////////////

	// filename of resource-file with source language 
	CString m_strInputRC;
	// file of the INI File with IDC and text-strings
	CString m_strLangINI;
	// filename of the generated RC
	CString m_strOutputRC;

	CString newRCdata; // Working buffer for new RC data output
	
	BOOL m_bCopy;
	
	int m_nObsoleteItems;
	CString m_strWorkspace;
	CComboBox m_CtrlLanguage;

	CString m_strTextmode;
	CString m_strAbout;
	BOOL m_bNoSort;	

	// checks if line contents strings that have to be translated
	bool MustBeTranslated(CString strLine, CString strKeyword);
	// search for '"' that is not a quotation mark inside the text ("")
	int FindQuote(CString strLine, int nStartPos=0);
	int WriteReadIni(bool bWrite);
	CString ExtractCaption(CString& strText, int* pnPosition, CString strKeyword, CString &strIDC);
	static CString GetFolder(CString strPath);

	BOOL OpenRCFile( CString filename, CString &strbuf, BOOL bShowError );
	BOOL OpenInputRC(BOOL bShowError = true);
	BOOL LoadWorkspace(BOOL bShowError = true);
	BOOL SaveWorkspace(void);
	CString GetAbsolutePathFromIni(CIniEx* pIniEx, CString strKey, CString strPath);
	void LogUserMessage( int strID ); // Log message to status window
	void LogUserMessage( CString msg );
	void MergeOldRCFileDialogLayout( CString &oldRCdata );
	void MergeOldRCFileDesignInfo( CString &oldRCdata );
	void RemoveNewRCFileRESItems( );

	static int AddLanguage(CComboBox* pComboBox, LPCTSTR strLangCode, LANGID SelectedID);
	static CString StringTokenize(CString strSource, LPCTSTR strDelimiters, int* pnStart);
	static int FindSeperateWord(CString strText, LPCTSTR strWord, int nStartPos);

	static int StringSpanIncluding( LPCTSTR pszBlock, LPCTSTR pszSet ) throw()
	{
		return (int)_tcsspn( ( pszBlock ), ( pszSet ) );

	}

	static int StringSpanExcluding( LPCTSTR pszBlock, LPCTSTR pszSet ) throw()
	{
		return (int)_tcscspn( ( pszBlock ), ( pszSet ) );
	}

	BOOL OpenSaveDialog(BOOL bOpen, BOOL bRelative, UINT nExtID, UINT nExtDescriptionID, CString& strEdit, CString strIniEntry);
	BOOL ShowError(UINT nIDString1, bool bGetLastError = false, DWORD dwErrCode = 0);

	static CString ConvertErrorToString( DWORD dwErrCode );
	
};
