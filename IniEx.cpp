// IniEx.cpp: implementation of the CIniEx class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "resource.h"
#include "IniEx.h"
#include "malloc.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

// UNICODE File Reader
////////////////////////////////

CStdioUnicodeFile::CStdioUnicodeFile(CStdioUnicodeFile::FILEENCODING encoding):
	CStdioFile(),
  encoding(encoding),
	newLine("\r\n")
{
}

CStdioUnicodeFile::CStdioUnicodeFile(LPCTSTR lpszFileName, UINT nOpenFlags, CStdioUnicodeFile::FILEENCODING encoding):
	CStdioFile(lpszFileName, nOpenFlags),
  encoding(encoding)
{
}

void CStdioUnicodeFile::WriteBOM()
{
	switch(encoding)
	{
	case CStdioUnicodeFile::FILEENCODING_UTF16LE:
		{
			BYTE bom[2] = { 0xFF, 0xFE };
			Write(bom, 2);
		}
		break;

	case CStdioUnicodeFile::FILEENCODING_UTF8:
	case CStdioUnicodeFile::FILEENCODING_UTF8_WITHOUT_BOM:
		{
			BYTE bom[3] = { 0xEF, 0xBB, 0xBF };
			Write(bom, 3);
		}
		break;
	}
}

CStdioUnicodeFile::FILEENCODING CStdioUnicodeFile::ReadBOM()
{
	BYTE dummy[3] = {0, 0, 0};
	UINT len = Read(dummy, 2);

	// check first two bytes (BOT)
	if (len == 2) 
	{
		if (dummy[0] == 0xFF && dummy[1] == 0xFE)
		{
			return CStdioUnicodeFile::FILEENCODING_UTF16LE;
		}
	}

	// check first three bytes (BOM)
	len += Read(dummy + 2, 1);
	if (len == 3) 
	{
		if (dummy[0] == 0xEF && dummy[1] == 0xBB && dummy[2] == 0xBF)
		{
			return CStdioUnicodeFile::FILEENCODING_UTF8;
		}
	}

	Seek(0, CFile::begin);
	return CStdioUnicodeFile::FILEENCODING_ANSI;
}

// static method to check if file is unicode
CStdioUnicodeFile::FILEENCODING CStdioUnicodeFile::GetFileEncoding(LPCTSTR lpszFileName)
{
	CStdioUnicodeFile File(CStdioUnicodeFile::FILEENCODING_UNKNOWN);
	if( !File.Open( lpszFileName, CFile::modeRead | CFile::typeBinary ) )
		return CStdioUnicodeFile::FILEENCODING_UNKNOWN;

	CStdioUnicodeFile::FILEENCODING result = File.ReadBOM();

	if( result == CStdioUnicodeFile::FILEENCODING_ANSI )
	{
		// UTF-8 without BOM ?
		unsigned char buf[8];
		while( !feof(File.m_pStream) )
		{
			if( fread(buf, 1, 1, File.m_pStream) == 0 ) break;
			size_t bytes = 0;
			if( buf[0] < 0x80 )
			{
				// 0xxxxxxx  ASCII < 0x80 (128)
				continue;
			}
			else
			{
				if( (buf[0] & 0xE0) == 0xC0 )
				{
					// 110xxxxx 10xxxxxx  2-byte
					bytes = 1;
				}
				else if( (buf[0] & 0xF0) == 0xE0 )
				{
					// 1110xxxx 10xxxxxx 10xxxxxx  3-byte
					bytes = 2;
				}
				else if( (buf[0] & 0xF8) == 0xF0 )
				{
					// 11110xxx 10xxxxxx 10xxxxxx 10xxxxxx  4-byte
					bytes = 3;
				}

				if( fread(buf, 1, bytes, File.m_pStream) != bytes ) goto ansi;
				for( size_t i = 0; i < bytes; ++i )
					if( (buf[i] & 0xC0) != 0x80 ) goto ansi;
			}
		}

		result = CStdioUnicodeFile::FILEENCODING_UTF8_WITHOUT_BOM;
	}

	ansi:

	File.Close();
	return result;
}

BOOL CStdioUnicodeFile::ReadString(CString& rString)
{
	ASSERT_VALID(this);
	rString = _T("");    // empty string without deallocating
	const int nMaxSize = 128;
	BOOL result = FALSE;

	switch(encoding)
	{
#ifdef UNICODE
	case CStdioUnicodeFile::FILEENCODING_UTF8:
	case CStdioUnicodeFile::FILEENCODING_UTF8_WITHOUT_BOM:
		{
			char * utf8 = nullptr;
			size_t size = 1;
			size_t len = 0;
			for(;;)
			{
				char * p = static_cast<char*>(realloc(utf8, size += nMaxSize));
				if(p == nullptr)
				{
					free(utf8);
					AfxThrowMemoryException();
				}
				utf8 = p;
				p = fgets(utf8 + len, nMaxSize + 1, m_pStream);
				// handle error/eof case
				if(p == nullptr && !feof(m_pStream))
				{
					free(utf8);
					clearerr(m_pStream);
					AfxThrowFileException(CFileException::genericException, _doserrno,
										  m_strFileName);
				}

				// if string is read completely or EOF
				if(p == nullptr)
				{
					return FALSE;
				}

				if(strlen(utf8 + len) < nMaxSize ||
				   utf8[len + nMaxSize - 1] == '\n')
				{
					break;
				}

				len += nMaxSize;
			}

			len = strlen(utf8);
			int rc = ::MultiByteToWideChar(
				CP_UTF8, 0,
				utf8, len,
				nullptr, 0);

			if(rc > 0)
			{
				wchar_t * lpsz = rString.GetBuffer(rc);
				rc = ::MultiByteToWideChar(
					CP_UTF8, 0,
					utf8, len,
					lpsz, rc + 1);
				lpsz[rc] = 0;
				rString.ReleaseBuffer();

				result = TRUE;
			}

			free(utf8);
		}
		break;
#endif
	default:
		{
			LPTSTR lpsz = rString.GetBuffer(nMaxSize);
			LPTSTR lpszResult;
			int nLen = 0;
			for(;;)
			{
				lpszResult = _fgetts(lpsz, nMaxSize + 1, m_pStream);
				rString.ReleaseBuffer();
				// handle error/eof case
				if(lpszResult == NULL && !feof(m_pStream))
				{
					clearerr(m_pStream);
					AfxThrowFileException(CFileException::genericException, _doserrno,
										  m_strFileName);
				}
				// if string is read completely or EOF
				if(lpszResult == NULL ||
				   (nLen = lstrlen(lpsz)) < nMaxSize ||
				   lpsz[nLen - 1] == '\n')
				   break;
				nLen = rString.GetLength();
				lpsz = rString.GetBuffer(nMaxSize + nLen) + nLen;
			}
			result = lpszResult != NULL;
		}
	}

	// remove '\n' from end of string if present
	LPTSTR lpsz = rString.GetBuffer(0);
	int nLen = rString.GetLength();
	if (nLen != 0 && lpsz[nLen-1] == '\n') {
		rString.GetBufferSetLength(nLen-1);
		nLen = rString.GetLength();
		if( nLen != 0 && lpsz[nLen - 1] == '\r' )
		{
			newLine = "\r\n";
			rString.GetBufferSetLength(nLen - 1);
		}
		else
		{
			newLine = "\n";
		}
	}

	return result;
}

void CStdioUnicodeFile::WriteString(LPCTSTR lpsz)
{
	switch(encoding)
	{
#ifdef UNICODE
	case CStdioUnicodeFile::FILEENCODING_UTF8:
	case CStdioUnicodeFile::FILEENCODING_UTF8_WITHOUT_BOM:
		{
			int len = static_cast<int>(wcslen(lpsz));
			int rc = ::WideCharToMultiByte(
				CP_UTF8, 0,
				lpsz, len,
				nullptr, 0,
				nullptr, nullptr);
			if(rc > 0)
			{
				char * utf8 = static_cast<char*>(malloc(rc + 1));
				if(utf8)
				{
					rc = ::WideCharToMultiByte(
						CP_UTF8, 0,
						lpsz, len,
						utf8, rc,
						nullptr, nullptr);

					Write(utf8, rc);
				}
			}
		}
		break;
#endif
	default:
		CStdioFile::WriteString(lpsz);
	}
}

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

//GrowSize for Dynmiz Section Allocation
CIniEx::CIniEx(int GrowSize/*=4*/)
{
	m_GrowSize=GrowSize;

	m_SectionNo=0;
	m_writeWhenChange=FALSE;
	m_makeBackup=FALSE;
	m_NoCaseSensitive=TRUE;
	m_Changed=FALSE;
	m_Keys=NULL;
	m_Values=NULL;
	m_allocatedObjectCount=0;

	m_NewLine = "\r\n";
}

CIniEx::~CIniEx()
{
	if (m_writeWhenChange)
		WriteFile(m_makeBackup);
	ResetContent();
}

void CIniEx::ParseLine (const CString& line, CString& key, CString& val)
{
	int iStart = 0, iEnd;
	bool quoted = false;

	//Clear params
	key.Empty();
	val.Empty();

	//First eat whitespaces
	while ( line[iStart] && _istspace(line[iStart]) )
		iStart++;
	
	// return if empty input string
	if ( line[iStart] == _T('\0') )
		return;

	//Save starting position and continue
	iEnd = iStart;

	// First character is " -> everything is quoted (always)
	if ( line[iStart] == _T('\"') )
	{
		quoted = true;
		iEnd++;
	}

	//Walk until first quote skipping pairs
	while ( line[iEnd] )
	{
		if ( !quoted && (line[iEnd] == _T('=')) )
		{
			break;
		}
		else if ( quoted && line[iEnd] == _T('\"') )
		{
			iEnd++;
			if ( line[iEnd] != _T('\"') )
			{
				break;
			}
		}
		iEnd++;
	}

	//We have our key
	key = line.Mid(iStart, iEnd - iStart);

	//Remove spaces only from right
	key.TrimRight();

	//Find the equal.. we should NOT have any remaining chars here...
	//but we could if we have something like:
	//"key" extra = value
	//In that case we discard the extra data
	while ( line[iEnd] && line[iEnd] != _T('=') )
		iEnd++;

	if ( !line[iEnd] )
		return;

	//Onto the value
	iStart = iEnd + 1;

	//Eat whitespaces again
	while ( line[iStart] && _istspace(line[iStart]) )
		iStart++;

	val = line.Mid(iStart);
	val.TrimRight();
}


BOOL CIniEx::OpenAtExeDirectory(LPCTSTR pFileName,
							BOOL writeWhenChange,/*=TRUE*/
							BOOL createIfNotExist/*=TRUE*/,
							BOOL noCaseSensitive /*=TRUE*/,
							BOOL makeBackup      /*=FALSE*/)
{

	CString filePath;
//if it's a dll argv will be NULL and it may cause memory leak	
#ifndef _USRDLL
	CString tmpFilePath;
	int nPlace=0;
	tmpFilePath=__argv[0];
	nPlace=tmpFilePath.ReverseFind('\\');
	
	
	if (nPlace!=-1)
	{
		filePath=tmpFilePath.Left(nPlace);
	}
	else
	{
		TCHAR curDir[MAX_PATH];
		GetCurrentDirectory(MAX_PATH,curDir);
		filePath=curDir;
	}
#else
	//it must be safe for dll's
	TCHAR curDir[MAX_PATH];
	GetCurrentDirectory(MAX_PATH,curDir);
	filePath=curDir;
#endif
	filePath+="\\";
	filePath+=pFileName;
	return Open(filePath,writeWhenChange,createIfNotExist,noCaseSensitive,makeBackup);
}

BOOL CIniEx::Open(LPCTSTR pFileName,
				  BOOL writeWhenChange,/*=FALSE*/
				  BOOL createIfNotExist/*=TRUE*/,
				  BOOL noCaseSensitive /*=FALSE*/,
				  BOOL makeBackup      /*=FALSE*/)
{

	CFileException e;
	BOOL bRet;
	CString Line;
	CString sectionStr;
	//int nPlace;
	UINT mode=CFile::modeReadWrite;

	//if it's second ini file for this instance
	//we have to save it and delete member variables contents
	if (!m_FileName.IsEmpty()) 
	{
		if( m_writeWhenChange )
			WriteFile();
		ResetContent();
	}

	m_NoCaseSensitive=noCaseSensitive;
	m_writeWhenChange=writeWhenChange;
	m_makeBackup=makeBackup;

	CString tmpKey;
	CString tmpValue;
	if (createIfNotExist)
		mode= mode | CFile::modeCreate | CFile::modeNoTruncate;

	CStdioUnicodeFile::FILEENCODING encoding = CStdioUnicodeFile::GetFileEncoding( pFileName );

	try
	{
		m_FileName=pFileName;

		//Grow the arrays as GrowSize(which given constructor)
		m_allocatedObjectCount = m_GrowSize;
		m_Keys=(CStringArray **)malloc( m_allocatedObjectCount * sizeof(CStringArray *) );
		m_Values=(CStringArray **)malloc( m_allocatedObjectCount * sizeof(CStringArray *) );
		for (int i=0;i<m_GrowSize;i++)
		{
			m_Keys[m_SectionNo+i]=NULL;
			m_Values[m_SectionNo+i]=NULL;
		}		

		CStdioUnicodeFile file(encoding);

#ifdef UNICODE
		switch(encoding)
		{
		case CStdioUnicodeFile::FILEENCODING_UTF16LE:
		case CStdioUnicodeFile::FILEENCODING_UTF8:
		case CStdioUnicodeFile::FILEENCODING_UTF8_WITHOUT_BOM:
			if(!file.Open(pFileName, mode | CFILEFLAG_UNICODEHELPER, &e))
			{
				return false;
			}
			
			file.ReadBOM();
			break;

		default:
			if(!file.Open(pFileName, mode, &e))
				return false;
		}
#else
      switch( encoding )
			{
      case CStdioUnicodeFile::FILEENCODING_UTF16LE:
      case CStdioUnicodeFile::FILEENCODING_UTF8:
				CString strErr;
				strErr.Format( IDS_UNICODEFILE, pFileName );
				AfxMessageBox( strErr );
				return false;	// cancel

      default:
				if( !file.Open( pFileName, mode, &e ) )
					return false;
			}
#endif

		for(;;)
		{
			//Read one line from given ini file
			bRet=file.ReadString(Line);
			if (!bRet) 
				break;
			Line.TrimRight();
			if (Line=="") 
				continue;
					
			//if line's first character = '[' 
			//and last character = ']' it's section 
			if (Line.Left(1)=="[" && Line.Right(1)=="]")
			{
				m_Keys[m_SectionNo]=new CStringArray;
				m_Values[m_SectionNo]=new CStringArray;
				m_SectionNo++;		
				GrowIfNecessary();
				
				sectionStr=Line.Mid(1,Line.GetLength()-2);
				m_Sections.SetAtGrow(m_SectionNo-1,sectionStr);
				continue;
			}
			
			ParseLine( Line, tmpKey, tmpValue );

			/*nPlace=Line.Find(_T("="));
			if (nPlace==-1)
			{
				tmpKey=Line;
				tmpValue="";
			}
			else
			{
				tmpKey=Line.Left(nPlace);
				tmpValue=Line.Mid(nPlace+1);
			}*/

			// create Section of no one exists in file
			if( m_SectionNo == 0 )
			{	
				m_Keys[m_SectionNo]=new CStringArray;
				m_Values[m_SectionNo]=new CStringArray;
				m_SectionNo++;
			}
			m_Keys[m_SectionNo-1]->Add(tmpKey);
			m_Values[m_SectionNo-1]->Add(tmpValue);
			m_Sections.SetAtGrow(m_SectionNo-1,sectionStr);
		}

		m_NewLine = file.GetNewLine();
		file.Close();
	}
	catch (CFileException *e)
	{
		m_ErrStr.Format( _T("%d"), e->m_cause );
	}
	

	return TRUE;
}

CString CIniEx::GetValue(CString Key)
{
	return GetValue(_T(""),Key);
}

//if Section Name="" -> looking up key for witout section
CString CIniEx::GetValue(CString Section,CString Key,CString DefaultValue/*=""*/)
{
	int nIndex=LookupSection(&Section);
	if (nIndex==-1) return DefaultValue;
	int nRet;
	CString retStr;
	for (INT_PTR i=m_Keys[nIndex]->GetUpperBound();i>=0;i--)
	{
		nRet=CompareStrings(&(m_Keys[nIndex]->GetAt(i)),&Key);
		if (nRet==0)
		{
			retStr=m_Values[nIndex]->GetAt(i);
			/*int nPlace=retStr.ReverseFind(';');
			if (nPlace!=-1) 
				retStr.Delete(nPlace,retStr.GetLength()-nPlace);*/
			return retStr;
		}
	}
	return DefaultValue;
}



//returns index of key for given section
//if no result returns -1
int CIniEx::LookupKey(int nSectionIndex,CString *Key)
{
	ASSERT(nSectionIndex<=m_SectionNo);
	int nRet;
	for (INT_PTR i=m_Keys[nSectionIndex]->GetUpperBound();i>=0;i--)
	{
		nRet=CompareStrings(&m_Keys[nSectionIndex]->GetAt(i),Key);
		if (nRet==0) 
			return (int)i;
	}
	return -1;
}

//return given sections index in array
int CIniEx::LookupSection(CString *Section)
{
	int nRet;
	for (int i=0;i<m_Sections.GetSize();i++)
	{
		nRet=CompareStrings(&m_Sections.GetAt(i),Section);
		if (nRet==0) return i;
	}
	return -1;
}

//Sets for Key=Value for without section
void CIniEx::SetValue(CString Key,CString Value)
{
	SetValue(_T(""),Key,Value);
}

//writes Key=value given section
void CIniEx::SetValue(CString Section,CString Key,CString Value)
{
	//file opened?
	ASSERT(!m_FileName.IsEmpty());

	//if given key already existing, overwrite it
	int nIndex=LookupSection(&Section);
	int nKeyIndex;
	if (nIndex==-1)
	{
		//if key not exist grow arrays (if necessary)
		m_Changed=TRUE;
		m_SectionNo++;
		GrowIfNecessary();
		m_Keys[m_SectionNo-1]=new CStringArray;
		m_Values[m_SectionNo-1]=new CStringArray;
		nIndex=m_SectionNo-1;
		m_Sections.SetAtGrow(m_SectionNo-1,Section);
	}

	
	//looking up keys for section
	nKeyIndex=LookupKey(nIndex,&Key);
	
	//if key exist -> overwrite it
	//if not add to end of array
	if (nKeyIndex!=-1) 
	{
		if (CompareStrings(&m_Values[nIndex]->GetAt(nKeyIndex),&Value)!=0)
			m_Changed=TRUE;
		m_Values[nIndex]->SetAt(nKeyIndex,Value);
	}
	else	//if not exist
	{
		m_Changed=TRUE;
		m_Keys[nIndex]->Add(Key);
		m_Values[nIndex]->Add(Value);
	}
}


//returns backup file name
//if you didn't want backup (when openning file) it returns ""
CString CIniEx::WriteFile(BOOL makeBackup/*=FALSE*/)
{
	if (!m_Changed) 
		return _T("");
	CString tmpFileName=m_FileName;

	if (makeBackup)
	{
		if (m_BackupFileName.IsEmpty())
		{
			FindBackupFile();
		}
		CopyFile(m_FileName,m_BackupFileName,FALSE);
	}

	
	CStdioUnicodeFile file(CStdioUnicodeFile::FILEENCODING_UTF8);
	if (!file.Open(m_FileName, CFILEFLAG_UNICODEHELPER | CFile::modeCreate | CFile::modeWrite)) 
	{
		#ifdef _DEBUG
			afxDump << "ERROR!!!!: The file could not open for writing\n";
		#endif
		return _T("");
	}

	file.WriteBOM();

	CString tmpLine;
	for (int i=0;i<m_Sections.GetSize();i++)
	{
		if (!m_Sections.GetAt(i).IsEmpty())
		{
			tmpLine.Format(_T("[%s]%s"),m_Sections.GetAt(i), m_NewLine );
			file.WriteString(tmpLine);
		}
		if (!m_Keys[i]) continue;
		for (int j=0;j<=m_Keys[i]->GetUpperBound();j++)
		{
			//if key is empts we don't write "="
			tmpLine.Format(_T("%s%s%s%s"),m_Keys[i]->GetAt(j),
							m_Keys[i]->GetAt(j).IsEmpty()?"":"=",
							   m_Values[i]->GetAt(j), m_NewLine );
		
			file.WriteString(tmpLine);
 
		}
	}
	file.Close();
	return m_BackupFileName;
}

void CIniEx::WriteFileXliff()
{
	CStdioUnicodeFile file(CStdioUnicodeFile::FILEENCODING_UTF8);
	if( !file.Open(m_FileName.Left(m_FileName.GetLength() - 4) + CString(".xlf"), CFILEFLAG_UNICODEHELPER | CFile::modeCreate | CFile::modeWrite) )
	{
		#ifdef _DEBUG
			afxDump << "ERROR!!!!: The file could not open for writing\n";
		#endif
		return;
	}

	file.WriteBOM();

	file.WriteString(CString("<?xml version=\"1.0\" encoding=\"utf-8\"?>\n"));
	file.WriteString(CString("<xliff version=\"1.2\" xmlns=\"urn:oasis:names:tc:xliff:document:1.2\" xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\" xsi:schemaLocation=\"urn:oasis:names:tc:xliff:document:1.2 http://docs.oasis-open.org/xliff/v1.2/os/xliff-core-1.2-strict.xsd\">\n"));
	file.WriteString(CString("\t<file source-language=\"en\" target-language=\"en\" datatype=\"winres\" original=\"") + m_FileName + CString("\">\n"));
	file.WriteString(CString("\t\t<body>\n"));

	CString tmpLine;
	for (int i=0;i<m_Sections.GetSize();i++)
	{
		if( m_Sections.GetAt(i).IsEmpty() )
		{
			file.WriteString(CString("\t\t\t<group>\n"));
		}
		else
		{
			tmpLine.Format(_T("\t\t\t<group id=\"%s\">\n"),m_Sections.GetAt(i));
			file.WriteString(tmpLine);
		}

		if( m_Keys[i] )
		{
			for( int j = 0; j <= m_Keys[i]->GetUpperBound(); j++ )
			{
				const CString& ref1 = m_Keys[i]->GetAt(j);
				CString xmlencodedSource = ref1.Mid(1, ref1.GetLength() - 2);
				xmlencodedSource.Replace(_T("&"), _T("&amp;"));
				xmlencodedSource.Replace(_T("'"), _T("&apos;"));
				xmlencodedSource.Replace(_T("\""), _T("&quot;"));
				xmlencodedSource.Replace(_T("<"), _T("&lt;"));
				xmlencodedSource.Replace(_T(">"), _T("&gt;"));

				const CString& ref2 = m_Values[i]->GetAt(j);
				CString xmlencodedTarget = ref2.Mid(1, ref2.GetLength() - 2);
				xmlencodedTarget.Replace(_T("&"), _T("&amp;"));
				xmlencodedTarget.Replace(_T("'"), _T("&apos;"));
				xmlencodedTarget.Replace(_T("\""), _T("&quot;"));
				xmlencodedTarget.Replace(_T("<"), _T("&lt;"));
				xmlencodedTarget.Replace(_T(">"), _T("&gt;"));

				tmpLine.Format(_T("\t\t\t\t<trans-unit id=\"%d\" resname=\"%d\" datatype=\"plaintext\">\n\t\t\t\t\t<source>%s</source>\n\t\t\t\t\t<target>%s</target>\n\t\t\t\t</trans-unit>\n"),
											 i * 10000 + j,
											 i * 10000 + j,
											 xmlencodedSource,
											 xmlencodedTarget);

				file.WriteString(tmpLine);
			}
		}

		file.WriteString(CString("\t\t\t</group>\n"));
	}

	file.WriteString(CString("\t\t</body>\n"));
	file.WriteString(CString("\t</file>\n"));
	file.WriteString(CString("</xliff>"));

	file.Close();
}

BOOL CIniEx::GetWriteWhenChange(void)
{
	return m_writeWhenChange;
}


void CIniEx::SetWriteWhenChange(BOOL WriteWhenChange)
{
	m_writeWhenChange=WriteWhenChange;
}


void CIniEx::SetBackupFileName(CString &backupFile)
{
	m_BackupFileName=backupFile;
}


void CIniEx::FindBackupFile(void)
{
	WIN32_FIND_DATA ffData;
	BOOL bContinue=TRUE;
	CString filePath(m_FileName);
	CString ext;
	int nPlace=filePath.ReverseFind('.');
	filePath.Delete(nPlace,filePath.GetLength()-nPlace);
	filePath+="*.*";
	int extNo=0;
	LPTSTR p;
	HANDLE handle=FindFirstFile(filePath,&ffData);
	while (bContinue)
	{
		bContinue=FindNextFile(handle,&ffData);
		p=ffData.cFileName;
		p+=_tcslen(ffData.cFileName)-3;
		if (_ttoi(p)>extNo) extNo=_ttoi(p);
	}
	m_BackupFileName.Format(_T("%s.%.3d"),m_FileName,extNo+1);

}


void CIniEx::ResetContent()
{
	if (!m_Keys) return;
	if ( m_Keys)
	{
		for (int i=0;i<m_allocatedObjectCount;i++)
		{
			if (m_Keys[i])
				delete m_Keys[i];
			if (m_Values[i])
				delete m_Values[i];
		}
		free(m_Keys);
		free(m_Values);
	}
	m_Keys=NULL;
	m_Values=NULL;
	m_Sections.RemoveAll();
	m_SectionNo=0;
	m_FileName="";
	m_Changed=FALSE;

}


//Removes key and it's value from given section
BOOL CIniEx::RemoveKey(CString Section,CString Key)
{
	int nIndex=LookupSection(&Section);
	if (nIndex==-1) return FALSE;

	int nKeyIndex=LookupKey(nIndex,&Key);
	if (nKeyIndex==-1) return FALSE;

	m_Keys[nIndex]->RemoveAt(nKeyIndex);
	m_Values[nIndex]->RemoveAt(nKeyIndex);
	m_Changed=TRUE;
	return TRUE;
}

//Removes key and it's value 
BOOL CIniEx::RemoveKey(CString Key)
{
	return RemoveKey(_T(""),Key);
}


//Removes given section(including all keys and values)
//return FALSE when given section not found
//It won't couse memory leak because when deleting object
//the msize (malloc size) checking
BOOL CIniEx::RemoveSection(CString Section)
{
	int nIndex=LookupSection(&Section);
	if (nIndex==-1) return FALSE;

	m_Keys[nIndex]->RemoveAll();
	m_Values[nIndex]->RemoveAll();

	delete m_Keys[nIndex];
	delete m_Values[nIndex];

	m_Keys[nIndex]=NULL;
	m_Values[nIndex]=NULL;

	m_Sections.RemoveAt(nIndex);
	m_SectionNo--;
	m_Changed=TRUE;
	return TRUE;
}

int CIniEx::CompareStrings(const CString *str1, CString *str2)
{
	if (m_NoCaseSensitive)
		return str1->CompareNoCase(*str2);
	else
		return str1->Compare(*str2);
}

void CIniEx::GrowIfNecessary(void)
{
	//for first gives GrowSize
	if (m_SectionNo>=m_allocatedObjectCount)
	{
		//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
		//realloc for GrowSize
		m_allocatedObjectCount += m_GrowSize;
		m_Keys=(CStringArray **)realloc(m_Keys,sizeof(CStringArray*) * (m_allocatedObjectCount) );
		m_Values=(CStringArray **)realloc(m_Values,sizeof(CStringArray*) * (m_allocatedObjectCount) );
		//allocated + GrowSize
		//zero memory for new allocation
		for (int i=0;i<m_GrowSize;i++)
		{
			m_Keys[m_SectionNo+i]=NULL;
			m_Values[m_SectionNo+i]=NULL;
		}
	}
}

//copy each string (section name) because 
//if sections parametter be a pointer it may clear content of member
void CIniEx::GetSections(CStringArray &sections)
{
	for (int i=0;i<m_Sections.GetSize();i++)
		sections.Add(m_Sections.GetAt(i));

}

void CIniEx::GetKeysInSection(CString section,CStringArray &keys)
{
	int nIndex=LookupSection(&section);
	if (nIndex==-1) return;

	for (int i=0;i<m_Keys[nIndex]->GetSize();i++)
	{
		keys.Add(m_Keys[nIndex]->GetAt(i));
	}
}

void CIniEx::SortIniValues()
{
	for (int i=0;i<m_Sections.GetSize();i++)
	{
		if (!m_Keys[i]) 
			continue;

		// Quicksort
		QuickSortRecursive( i, 0, m_Keys[i]->GetUpperBound(), true );
	}
}

int CIniEx::CompareItems( CString str1, CString str2 )
{
	return str1.CompareNoCase(str2);
}

bool CIniEx::Swap( int nSection, int nLeftIndex, int nRightIndex )
{
	// Ignore unnecessary swaps
	if( nLeftIndex == nRightIndex ) return false;
	if( nRightIndex == (nLeftIndex + 1) && CompareItems(m_Keys[nSection]->GetAt(nLeftIndex), m_Keys[nSection]->GetAt(nRightIndex)) == 0 ) return false;

	CString strHelp = m_Keys[nSection]->GetAt(nLeftIndex);
	m_Keys[nSection]->SetAt(nLeftIndex, m_Keys[nSection]->GetAt(nRightIndex) );
	m_Keys[nSection]->SetAt(nRightIndex, strHelp );

	strHelp = m_Values[nSection]->GetAt(nLeftIndex);
	m_Values[nSection]->SetAt(nLeftIndex, m_Values[nSection]->GetAt(nRightIndex));
	m_Values[nSection]->SetAt(nRightIndex, strHelp);

	return true;
}

void CIniEx::QuickSortRecursive(int nSection, int iLow, int iHigh, bool bAscending)
{
	// Params renamed for easier comparison with literature
	int iLeft = iLow;
	int iRight = iHigh;

	// Important: Save Pivot-Element on Stack, instead of using GetAt(iPivot) in 
	// "while('compare')-Loop". Original implementation by Attila Hajdrik used 
	// GetAt(iPivot) in within the Loop
	// int iPivot = (iLow+iHigh) / 2;
	CString Pivot = m_Keys[nSection]->GetAt((iLow+iHigh) / 2); 

	do
	{
		if( bAscending )
		{
			while( CompareItems(m_Keys[nSection]->GetAt(iLeft), Pivot) < 0 ) iLeft++;
			while( CompareItems(Pivot, m_Keys[nSection]->GetAt(iRight)) < 0 ) iRight--;
		}
		else
		{
			while( CompareItems(m_Keys[nSection]->GetAt(iLeft), Pivot) > 0 ) iLeft++;
			while( CompareItems(Pivot, m_Keys[nSection]->GetAt(iRight)) > 0 ) iRight--;
		}

		if( iLeft <= iRight )
		{
			Swap(nSection, iLeft, iRight);

			iLeft++;
			iRight--;
		}
	}
	while( iLeft <= iRight );

	if( iLow < iRight )
		QuickSortRecursive(nSection, iLow, iRight, bAscending);

	if( iLeft < iHigh )
		QuickSortRecursive(nSection, iLeft, iHigh, bAscending);
}
