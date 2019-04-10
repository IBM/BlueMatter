# Microsoft Developer Studio Project File - Name="blrudp" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Static Library" 0x0104

CFG=blrudp - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "blrudp.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "blrudp.mak" CFG="blrudp - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "blrudp - Win32 Release" (based on "Win32 (x86) Static Library")
!MESSAGE "blrudp - Win32 Debug" (based on "Win32 (x86) Static Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "blrudp - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "Release"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /YX /FD /c
# ADD CPP /nologo /W3 /GX /O2 /I "../include" /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /YX /FD /c
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo

!ELSEIF  "$(CFG)" == "blrudp - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /YX /FD /GZ /c
# ADD CPP /nologo /W3 /Gm /GX /ZI /Od /I "../include" /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /FR /YX /FD /GZ /c
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo

!ENDIF 

# Begin Target

# Name "blrudp - Win32 Release"
# Name "blrudp - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\blmap.c
# End Source File
# Begin Source File

SOURCE=.\blrudp.c
# End Source File
# Begin Source File

SOURCE=.\blrudp_common.c
# End Source File
# Begin Source File

SOURCE=.\blrudp_io.c
# End Source File
# Begin Source File

SOURCE=.\blrudp_iohpk.c
# End Source File
# Begin Source File

SOURCE=.\blrudp_pktpool.c
# End Source File
# Begin Source File

SOURCE=.\blrudp_pktstatus.c
# End Source File
# Begin Source File

SOURCE=.\blrudp_sapmgr.c
# End Source File
# Begin Source File

SOURCE=.\blrudp_state.c
# End Source File
# Begin Source File

SOURCE=.\blrudppkt.c
# End Source File
# Begin Source File

SOURCE=.\blrudppkt_help.c
# End Source File
# Begin Source File

SOURCE=.\blrudpwork.c
# End Source File
# Begin Source File

SOURCE=.\bltimemgr.c
# End Source File
# Begin Source File

SOURCE=..\common\dbgPrint.c
# End Source File
# Begin Source File

SOURCE=..\common\dbgtimelog.c
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=.\bldefs.h
# End Source File
# Begin Source File

SOURCE=..\include\BlErrors.h
# End Source File
# Begin Source File

SOURCE=.\blmap.h
# End Source File
# Begin Source File

SOURCE=.\blrudp.h
# End Source File
# Begin Source File

SOURCE=.\blrudp_common.h
# End Source File
# Begin Source File

SOURCE=.\blrudp_io.h
# End Source File
# Begin Source File

SOURCE=.\blrudp_memory.h
# End Source File
# Begin Source File

SOURCE=.\blrudp_pktpool.h
# End Source File
# Begin Source File

SOURCE=.\blrudp_pktstatus.h
# End Source File
# Begin Source File

SOURCE=.\blrudp_sapmgr.h
# End Source File
# Begin Source File

SOURCE=.\blrudp_st.h
# End Source File
# Begin Source File

SOURCE=.\blrudp_state.h
# End Source File
# Begin Source File

SOURCE=.\blrudppkt.h
# End Source File
# Begin Source File

SOURCE=.\blrudppkt_help.h
# End Source File
# Begin Source File

SOURCE=.\blrudpwork.h
# End Source File
# Begin Source File

SOURCE=.\bltimemgr.h
# End Source File
# Begin Source File

SOURCE=..\include\dbgPrint.h
# End Source File
# Begin Source File

SOURCE=..\include\dbgtimelog.h
# End Source File
# Begin Source File

SOURCE=..\include\gdb_stdint.h
# End Source File
# Begin Source File

SOURCE=..\include\hostcfg.h
# End Source File
# Begin Source File

SOURCE=.\qlink.h
# End Source File
# End Group
# End Target
# End Project
