// IniEx.h: interface for the CIniEx class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_INIEX_H__36888C4C_12D3_4F65_A78B_2F3C3576B5B8__INCLUDED_)
#define AFX_INIEX_H__36888C4C_12D3_4F65_A78B_2F3C3576B5B8__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifdef UNICODE
	#define CFILEFLAG_UNICODEHELPER	(CFile::typeBinary)
#else
	#define CFILEFLAG_UNICODEHELPER	(0)
#endif

class CStdioUnicodeFile: public CStdioFile
{
public:
	enum FILEENCODING
	{
		FILEENCODING_UNKNOWN,
		FILEENCODING_ANSI,
		FILEENCODING_UTF16LE,
		FILEENCODING_UTF8,
		FILEENCODING_UTF8_WITHOUT_BOM,
	};

	CStdioUnicodeFile(FILEENCODING encoding);
	CStdioUnicodeFile(LPCTSTR lpszFileName, UINT nOpenFlags, FILEENCODING encoding);
	BOOL ReadString(CString& cstr);
	void WriteString(LPCTSTR lpsz);

	void WriteBOM();
	FILEENCODING ReadBOM();

	static FILEENCODING GetFileEncoding(LPCTSTR lpszFileName);

	inline const CString& GetNewLine(void) const { return newLine;  }

private:
  FILEENCODING encoding;
	CString      newLine;
};

#define MAX_SECTION_COUNT	512
class CIniEx  
{
private:
	//Functions
	int		LookupKey		(int nSectionIndex,CString *Key);
	
	int		CompareStrings	(const CString *str1,CString *str2);
	void    GrowIfNecessary	(void);
	void	FindBackupFile	(void);
private:
	//Variables
	//CMapStringToString	*m_tmpLines;
	CString				m_ErrStr;
	CStringArray    	**m_Keys;
	CStringArray    	**m_Values;
	CStringArray		m_Sections;

	int		m_SectionNo;
	int		m_GrowSize;
	int		m_allocatedObjectCount;
	BOOL	m_NoCaseSensitive;
	BOOL	m_writeWhenChange;
	CString	m_BackupFileName;
	CString m_FileName;
	BOOL	m_makeBackup;
	BOOL	m_Changed;

	CString	m_NewLine;

public:
	int		LookupSection	(CString *Section);
	void GetKeysInSection(CString section,CStringArray &keys);
	void GetSections(CStringArray &sections);

	CIniEx(int GrowSize=4);
	virtual ~CIniEx();

	void ParseLine (const CString& line, CString& key, CString& val);

	BOOL Open(LPCTSTR pFileName,
			  BOOL writeWhenChange=FALSE,
			  BOOL createIfNotExist=TRUE,
			  BOOL noCaseSensitive=FALSE,
			  BOOL makeBackup=FALSE);
	BOOL OpenAtExeDirectory(LPCTSTR pFileName,
			  BOOL writeWhenChange=FALSE,
			  BOOL createIfNotExist=TRUE,
			  BOOL noCaseSensitive=FALSE,
			  BOOL makeBackup=FALSE);
	void ResetContent();

	CString WriteFile(BOOL makeBackup=FALSE);
	void WriteFileXliff();

	CString GetValue(CString Section,CString Key,CString DefaultValue=_T(""));
	CString GetValue(CString Key);

	void SetValue(CString Key,CString Value);
	void SetValue(CString Section,CString Key,CString Value);

	BOOL RemoveKey(CString Key);
	BOOL RemoveKey(CString Section,CString Key);

	BOOL RemoveSection(CString Section);

	BOOL GetWriteWhenChange();
	void SetWriteWhenChange(BOOL WriteWhenChange);

	void SetBackupFileName(CString &backupFile);

	void SortIniValues();
	int CompareItems( CString str1, CString str2 );
	bool Swap( int nSection, int nLeftIndex, int nRightIndex );
	void QuickSortRecursive(int nSection, int iLow, int iHigh, bool bAscending);
};

#endif // !defined(AFX_INIEX_H__36888C4C_12D3_4F65_A78B_2F3C3576B5B8__INCLUDED_)

