# Microsoft Developer Studio Project File - Name="German" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Dynamic-Link Library" 0x0102

CFG=German - Win32 Release
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "German.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "German.mak" CFG="German - Win32 Release"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "German - Win32 Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "German - Win32 Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "German - Win32 UNICODE Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "German - Win32 UNICODE Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "German - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir ".\..\Release"
# PROP BASE Intermediate_Dir ".\Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir ".\..\Release"
# PROP Intermediate_Dir ".\Release"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MT /Zi /W3 /Ob1 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_USRDLL" /D "GERMAN_EXPORTS" /D "_MBCS" /GF /Gy /Fp".\Release/German.pch" /Fo".\Release/" /Fd".\Release/" /c PRECOMP_VC7_TOBEREMOVED /GX 
# ADD CPP /nologo /MT /Zi /W3 /Ob1 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_USRDLL" /D "GERMAN_EXPORTS" /D "_MBCS" /GF /Gy /Fp".\Release/German.pch" /Fo".\Release/" /Fd".\Release/" /c PRECOMP_VC7_TOBEREMOVED /GX 
# ADD BASE MTL /nologo /tlb".\..\Release\German.tlb" /win32 
# ADD MTL /nologo /tlb".\..\Release\German.tlb" /win32 
# ADD BASE RSC /l 1033 
# ADD RSC /l 1033 
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo 
# ADD BSC32 /nologo 
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib odbc32.lib odbccp32.lib shlwapi.lib /nologo /dll /out:"..\Release\LocalizeRC07.dll" /incremental:no /debug /pdb:"..\Release\LocalizeRC07.pdb" /pdbtype:sept /subsystem:windows /opt:ref /opt:icf /noentry /implib:"$(OutDir)/German.lib" 
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib odbc32.lib odbccp32.lib shlwapi.lib /nologo /dll /out:"..\Release\LocalizeRC07.dll" /incremental:no /debug /pdb:"..\Release\LocalizeRC07.pdb" /pdbtype:sept /subsystem:windows /opt:ref /opt:icf /noentry /implib:"$(OutDir)/German.lib" 

!ELSEIF  "$(CFG)" == "German - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir ".\..\Debug"
# PROP BASE Intermediate_Dir ".\Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir ".\..\Debug"
# PROP Intermediate_Dir ".\Debug"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MTd /ZI /W3 /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_USRDLL" /D "GERMAN_EXPORTS" /D "_MBCS" /Fp".\Debug/German.pch" /Fo".\Debug/" /Fd".\Debug/" /GZ /c PRECOMP_VC7_TOBEREMOVED /GX 
# ADD CPP /nologo /MTd /ZI /W3 /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_USRDLL" /D "GERMAN_EXPORTS" /D "_MBCS" /Fp".\Debug/German.pch" /Fo".\Debug/" /Fd".\Debug/" /GZ /c PRECOMP_VC7_TOBEREMOVED /GX 
# ADD BASE MTL /nologo /tlb".\..\Debug\German.tlb" /win32 
# ADD MTL /nologo /tlb".\..\Debug\German.tlb" /win32 
# ADD BASE RSC /l 1033 
# ADD RSC /l 1033 
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo 
# ADD BSC32 /nologo 
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib odbc32.lib odbccp32.lib shlwapi.lib /nologo /dll /out:"..\Debug\LocalizeRC07.dll" /incremental:yes /debug /pdb:"..\Debug\LocalizeRC07.pdb" /pdbtype:sept /subsystem:windows /noentry /implib:"$(OutDir)/German.lib" 
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib odbc32.lib odbccp32.lib shlwapi.lib /nologo /dll /out:"..\Debug\LocalizeRC07.dll" /incremental:yes /debug /pdb:"..\Debug\LocalizeRC07.pdb" /pdbtype:sept /subsystem:windows /noentry /implib:"$(OutDir)/German.lib" 

!ELSEIF  "$(CFG)" == "German - Win32 UNICODE Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir ".\..\UNICODE_Debug"
# PROP BASE Intermediate_Dir ".\UNICODE_Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir ".\..\UNICODE_Debug"
# PROP Intermediate_Dir ".\UNICODE_Debug"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MTd /ZI /W3 /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_USRDLL" /D "GERMAN_EXPORTS" /D "UNICODE" /D "_UNICODE" /D "_UNICODE" /Fp".\UNICODE_Debug/German.pch" /Fo".\UNICODE_Debug/" /Fd".\UNICODE_Debug/" /GZ /c PRECOMP_VC7_TOBEREMOVED /GX 
# ADD CPP /nologo /MTd /ZI /W3 /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_USRDLL" /D "GERMAN_EXPORTS" /D "UNICODE" /D "_UNICODE" /D "_UNICODE" /Fp".\UNICODE_Debug/German.pch" /Fo".\UNICODE_Debug/" /Fd".\UNICODE_Debug/" /GZ /c PRECOMP_VC7_TOBEREMOVED /GX 
# ADD BASE MTL /nologo /tlb".\..\Debug\German.tlb" /win32 
# ADD MTL /nologo /tlb".\..\Debug\German.tlb" /win32 
# ADD BASE RSC /l 1033 
# ADD RSC /l 1033 
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo 
# ADD BSC32 /nologo 
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /out:"..\UNICODE_Debug\LocalizeRC07.dll" /incremental:yes /debug /pdb:"..\UNICODE_Debug\LocalizeRC07.pdb" /pdbtype:sept /subsystem:windows /noentry /implib:"$(OutDir)/German.lib" /machine:ix86 
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /out:"..\UNICODE_Debug\LocalizeRC07.dll" /incremental:yes /debug /pdb:"..\UNICODE_Debug\LocalizeRC07.pdb" /pdbtype:sept /subsystem:windows /noentry /implib:"$(OutDir)/German.lib" /machine:ix86 

!ELSEIF  "$(CFG)" == "German - Win32 UNICODE Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir ".\..\UNICODE_Release"
# PROP BASE Intermediate_Dir ".\UNICODE_Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir ".\..\UNICODE_Release"
# PROP Intermediate_Dir ".\UNICODE_Release"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MT /Zi /W3 /Ob1 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_USRDLL" /D "GERMAN_EXPORTS" /D "_UNICODE" /D "UNICODE" /D "_MBCS" /GF /Gy /Fp".\UNICODE_Release/German.pch" /Fo".\UNICODE_Release/" /Fd".\UNICODE_Release/" /c PRECOMP_VC7_TOBEREMOVED /GX 
# ADD CPP /nologo /MT /Zi /W3 /Ob1 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_USRDLL" /D "GERMAN_EXPORTS" /D "_UNICODE" /D "UNICODE" /D "_MBCS" /GF /Gy /Fp".\UNICODE_Release/German.pch" /Fo".\UNICODE_Release/" /Fd".\UNICODE_Release/" /c PRECOMP_VC7_TOBEREMOVED /GX 
# ADD BASE MTL /nologo /tlb".\..\Release\German.tlb" /win32 
# ADD MTL /nologo /tlb".\..\Release\German.tlb" /win32 
# ADD BASE RSC /l 1033 
# ADD RSC /l 1033 
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo 
# ADD BSC32 /nologo 
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib odbc32.lib odbccp32.lib shlwapi.lib /nologo /dll /out:"..\UNICODE_Release\LocalizeRC07.dll" /incremental:no /debug /pdb:".\..\UNICODE_Release\LocalizeRC07.pdb" /pdbtype:sept /subsystem:windows /opt:ref /opt:icf /noentry /implib:"$(OutDir)/German.lib" 
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib odbc32.lib odbccp32.lib shlwapi.lib /nologo /dll /out:"..\UNICODE_Release\LocalizeRC07.dll" /incremental:no /debug /pdb:".\..\UNICODE_Release\LocalizeRC07.pdb" /pdbtype:sept /subsystem:windows /opt:ref /opt:icf /noentry /implib:"$(OutDir)/German.lib" 

!ENDIF

# Begin Target

# Name "German - Win32 Release"
# Name "German - Win32 Debug"
# Name "German - Win32 UNICODE Debug"
# Name "German - Win32 UNICODE Release"
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl;inc"
# Begin Source File

SOURCE=.\resource.h
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "rc;ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# Begin Source File

SOURCE=.\German.rc
# End Source File
# Begin Source File

SOURCE=.\res\LocalizeRC.ico
# End Source File
# End Group
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;def;odl;idl;hpj;bat;asm"
# End Group
# Begin Source File

SOURCE=.\res\LocalizeRC.manifest
# End Source File
# End Target
# End Project

