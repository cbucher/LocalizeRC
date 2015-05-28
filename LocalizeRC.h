// LocalizeRC.h : main header file for the PROJECT_NAME application
//

#pragma once

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif


// CLocalizeRCApp:
// See LocalizeRC.cpp for the implementation of this class
//

// Workspace / Registry keys
#define SEC_LASTPROJECT _T("Last Project")

#define ENT_WORKSPACE _T("Workspace")
#define ENT_LANGID _T("LanguageID")

#define ENT_OUTPUTRC _T("OutputRC")
#define ENT_LANGINI _T("LangINI")
#define ENT_INPUTRC _T("InputRC")
#define ENT_COPY _T("Copy")
#define ENT_OBSITEMS _T("ObsoleteItems")
#define ENT_NOSORT _T("NoSort")

#define STANDARD_LANGID 0x09	// English

class CLocalizeRCApp : public CWinApp
{
public:
	CLocalizeRCApp();

// Overrides
	public:
	virtual BOOL InitInstance();

// Implementation
private:
	LANGID CurLangID;

	DECLARE_MESSAGE_MAP()
	BOOL LoadLangIDDLL(LANGID LangID);
};

extern CLocalizeRCApp theApp;