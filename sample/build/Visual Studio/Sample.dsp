# Microsoft Developer Studio Project File - Name="Sample" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) External Target" 0x0106

CFG=Sample - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "Sample.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "Sample.mak" CFG="Sample - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "Sample - Win32 Release" (based on "Win32 (x86) External Target")
!MESSAGE "Sample - Win32 Debug" (based on "Win32 (x86) External Target")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""

!IF  "$(CFG)" == "Sample - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Cmd_Line "NMAKE /f Sample.mak"
# PROP BASE Rebuild_Opt "/a"
# PROP BASE Target_File "Sample.exe"
# PROP BASE Bsc_Name "Sample.bsc"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "Release"
# PROP Cmd_Line "make --file ../Makefile"
# PROP Rebuild_Opt ""
# PROP Target_File "Sample.gba"
# PROP Bsc_Name ""
# PROP Target_Dir ""

!ELSEIF  "$(CFG)" == "Sample - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Cmd_Line "NMAKE /f Sample.mak"
# PROP BASE Rebuild_Opt "/a"
# PROP BASE Target_File "Sample.exe"
# PROP BASE Bsc_Name "Sample.bsc"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug"
# PROP Cmd_Line "make --file ../Makefile"
# PROP Rebuild_Opt ""
# PROP Target_File "Sample.gba"
# PROP Bsc_Name ""
# PROP Target_Dir ""

!ENDIF 

# Begin Target

# Name "Sample - Win32 Release"
# Name "Sample - Win32 Debug"

!IF  "$(CFG)" == "Sample - Win32 Release"

!ELSEIF  "$(CFG)" == "Sample - Win32 Debug"

!ENDIF 

# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=..\..\source\Sample.c
# End Source File
# Begin Source File

SOURCE=..\..\source\SampleEffect3DScene.c
# End Source File
# Begin Source File

SOURCE=..\..\source\SampleEffectCredits.c
# End Source File
# Begin Source File

SOURCE=..\..\source\SampleEffectMultiPlayer.c
# End Source File
# Begin Source File

SOURCE=..\..\source\SampleEffectSprites.c
# End Source File
# Begin Source File

SOURCE=..\..\source\SampleEffectTiles.c
# End Source File
# Begin Source File

SOURCE=..\..\source\SampleEffectTriangle.c
# End Source File
# Begin Source File

SOURCE=..\..\source\SampleEffectWelcome.c
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=..\..\include\SampleEffects.h
# End Source File
# End Group
# Begin Group "Build"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\crt0.S
# End Source File
# Begin Source File

SOURCE=..\lnkscript
# End Source File
# Begin Source File

SOURCE=..\Makefile
# End Source File
# End Group
# End Target
# End Project
