# Microsoft Developer Studio Project File - Name="The SGADE" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) External Target" 0x0106

CFG=The SGADE - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "The SGADE.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "The SGADE.mak" CFG="The SGADE - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "The SGADE - Win32 Release" (based on "Win32 (x86) External Target")
!MESSAGE "The SGADE - Win32 Debug" (based on "Win32 (x86) External Target")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""

!IF  "$(CFG)" == "The SGADE - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Cmd_Line "NMAKE /f "The SGADE.mak""
# PROP BASE Rebuild_Opt "/a"
# PROP BASE Target_File "The SGADE.exe"
# PROP BASE Bsc_Name "The SGADE.bsc"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "Release"
# PROP Cmd_Line "make --file ../Makefile"
# PROP Rebuild_Opt "clean"
# PROP Target_File "socrates.a"
# PROP Bsc_Name ""
# PROP Target_Dir ""

!ELSEIF  "$(CFG)" == "The SGADE - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Cmd_Line "NMAKE /f "The SGADE.mak""
# PROP BASE Rebuild_Opt "/a"
# PROP BASE Target_File "The SGADE.exe"
# PROP BASE Bsc_Name "The SGADE.bsc"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug"
# PROP Cmd_Line "make --file ../Makefile"
# PROP Rebuild_Opt "clean"
# PROP Target_File "socrates.a"
# PROP Bsc_Name ""
# PROP Target_Dir ""

!ENDIF 

# Begin Target

# Name "The SGADE - Win32 Release"
# Name "The SGADE - Win32 Debug"

!IF  "$(CFG)" == "The SGADE - Win32 Release"

!ELSEIF  "$(CFG)" == "The SGADE - Win32 Debug"

!ENDIF 

# Begin Group "Build"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\Makefile
# End Source File
# End Group
# Begin Group "Include"

# PROP Default_Filter "h"
# Begin Source File

SOURCE=..\..\include\SoAssembly.h
# End Source File
# Begin Source File

SOURCE=..\..\include\SoBkg.h
# End Source File
# Begin Source File

SOURCE=..\..\include\SoBkgFont.h
# End Source File
# Begin Source File

SOURCE=..\..\include\SoBkgManager.h
# End Source File
# Begin Source File

SOURCE=..\..\include\SoBkgMap.h
# End Source File
# Begin Source File

SOURCE=..\..\include\SoBkgMemManager.h
# End Source File
# Begin Source File

SOURCE=..\..\include\SoCamera.h
# End Source File
# Begin Source File

SOURCE=..\..\include\Socrates.h
# End Source File
# Begin Source File

SOURCE=..\..\include\SoDebug.h
# End Source File
# Begin Source File

SOURCE=..\..\include\SoDisplay.h
# End Source File
# Begin Source File

SOURCE=..\..\include\SoDMA.h
# End Source File
# Begin Source File

SOURCE=..\..\include\SoEffects.h
# End Source File
# Begin Source File

SOURCE=..\..\include\SoFlashMem.h
# End Source File
# Begin Source File

SOURCE=..\..\include\SoFont.h
# End Source File
# Begin Source File

SOURCE=..\..\include\SoImage.h
# End Source File
# Begin Source File

SOURCE=..\..\include\SoIntManager.h
# End Source File
# Begin Source File

SOURCE=..\..\include\SoKeys.h
# End Source File
# Begin Source File

SOURCE=..\..\include\SoMath.h
# End Source File
# Begin Source File

SOURCE=..\..\include\SoMatrix.h
# End Source File
# Begin Source File

SOURCE=..\..\include\SoMemManager.h
# End Source File
# Begin Source File

SOURCE=..\..\include\SoMesh.h
# End Source File
# Begin Source File

SOURCE=..\..\include\SoMode4PolygonRasterizer.h
# End Source File
# Begin Source File

SOURCE=..\..\include\SoMode4Renderer.h
# End Source File
# Begin Source File

SOURCE=..\..\include\SoMultiPlayer.h
# End Source File
# Begin Source File

SOURCE=..\..\include\SoPalette.h
# End Source File
# Begin Source File

SOURCE=..\..\include\SoPolygon.h
# End Source File
# Begin Source File

SOURCE=..\..\include\SoSound.h
# End Source File
# Begin Source File

SOURCE=..\..\include\SoSprite.h
# End Source File
# Begin Source File

SOURCE=..\..\include\SoSpriteAnimation.h
# End Source File
# Begin Source File

SOURCE=..\..\include\SoSpriteManager.h
# End Source File
# Begin Source File

SOURCE=..\..\include\SoSpriteMemManager.h
# End Source File
# Begin Source File

SOURCE=..\..\include\SoSystem.h
# End Source File
# Begin Source File

SOURCE=..\..\include\SoTables.h
# End Source File
# Begin Source File

SOURCE=..\..\include\SoTileSet.h
# End Source File
# Begin Source File

SOURCE=..\..\include\SoTimer.h
# End Source File
# Begin Source File

SOURCE=..\..\include\SoTransform.h
# End Source File
# Begin Source File

SOURCE=..\..\include\SoVector.h
# End Source File
# Begin Source File

SOURCE=..\..\include\SoWindow.h
# End Source File
# End Group
# Begin Group "Source"

# PROP Default_Filter ""
# Begin Group "C"

# PROP Default_Filter "c"
# Begin Source File

SOURCE=..\..\source\SoBkg.c
# End Source File
# Begin Source File

SOURCE=..\..\source\SoBkgManager.c
# End Source File
# Begin Source File

SOURCE=..\..\source\SoBkgMap.c
# End Source File
# Begin Source File

SOURCE=..\..\source\SoBkgMemManager.c
# End Source File
# Begin Source File

SOURCE=..\..\source\SoCamera.c
# End Source File
# Begin Source File

SOURCE=..\..\source\SoDebug.c
# End Source File
# Begin Source File

SOURCE=..\..\source\SoDisplay.c
# End Source File
# Begin Source File

SOURCE=..\..\source\SoDMA.c
# End Source File
# Begin Source File

SOURCE=..\..\source\SoEffects.c
# End Source File
# Begin Source File

SOURCE=..\..\source\SoFlashMem.c
# End Source File
# Begin Source File

SOURCE=..\..\source\SoFont.c
# End Source File
# Begin Source File

SOURCE=..\..\source\SoImage.c
# End Source File
# Begin Source File

SOURCE=..\..\source\SoIntManager.c
# End Source File
# Begin Source File

SOURCE=..\..\source\SoKeys.c
# End Source File
# Begin Source File

SOURCE=..\..\source\SoMath.c
# End Source File
# Begin Source File

SOURCE=..\..\source\SoMatrix.c
# End Source File
# Begin Source File

SOURCE=..\..\source\SoMemManager.c
# End Source File
# Begin Source File

SOURCE=..\..\source\SoMesh.c
# End Source File
# Begin Source File

SOURCE=..\..\source\SoMeshCube.c
# End Source File
# Begin Source File

SOURCE=..\..\source\SoMode4PolygonRasterizer.c
# End Source File
# Begin Source File

SOURCE=..\..\source\SoMode4Renderer.c
# End Source File
# Begin Source File

SOURCE=..\..\source\SoMultiPlayer.c
# End Source File
# Begin Source File

SOURCE=..\..\source\SoPalette.c
# End Source File
# Begin Source File

SOURCE=..\..\source\SoPolygon.c
# End Source File
# Begin Source File

SOURCE=..\..\source\SoSound.c
# End Source File
# Begin Source File

SOURCE=..\..\source\SoSprite.c
# End Source File
# Begin Source File

SOURCE=..\..\source\SoSpriteAnimation.c
# End Source File
# Begin Source File

SOURCE=..\..\source\SoSpriteManager.c
# End Source File
# Begin Source File

SOURCE=..\..\source\SoSpriteMemManager.c
# End Source File
# Begin Source File

SOURCE=..\..\source\SoSystem.c
# End Source File
# Begin Source File

SOURCE=..\..\source\SoTables.c
# End Source File
# Begin Source File

SOURCE=..\..\source\SoTileSet.c
# End Source File
# Begin Source File

SOURCE=..\..\source\SoTimer.c
# End Source File
# Begin Source File

SOURCE=..\..\source\SoTransform.c
# End Source File
# Begin Source File

SOURCE=..\..\source\SoVector.c
# End Source File
# Begin Source File

SOURCE=..\..\source\SoWindow.c
# End Source File
# End Group
# Begin Group "S"

# PROP Default_Filter "s"
# Begin Source File

SOURCE=..\..\source\SoIntManagerIntHandler.S
# End Source File
# Begin Source File

SOURCE=..\..\source\SoMathDivide.s
# End Source File
# Begin Source File

SOURCE=..\..\source\SoMode4PolygonRasterizerSolidTriangle.s
# End Source File
# Begin Source File

SOURCE=..\..\source\SoMode4RendererClear.s
# End Source File
# End Group
# End Group
# Begin Group "Documentation"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\documentation\SoDoxygenConfigFile
# End Source File
# Begin Source File

SOURCE=..\..\documentation\SoDoxygenHTMLFooter.html
# End Source File
# Begin Source File

SOURCE=..\..\documentation\SoDoxygenHTMLHeader.html
# End Source File
# Begin Source File

SOURCE=..\..\documentation\SoDoxygenHTMLStyleSheet.css
# End Source File
# End Group
# Begin Source File

SOURCE=..\..\SoReadme.txt
# End Source File
# Begin Source File

SOURCE=..\..\SoRevision.txt
# End Source File
# End Target
# End Project
