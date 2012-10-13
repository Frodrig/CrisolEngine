# Microsoft Developer Studio Project File - Name="DX7Wrapper" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Static Library" 0x0104

CFG=DX7Wrapper - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "DX7Wrapper.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "DX7Wrapper.mak" CFG="DX7Wrapper - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "DX7Wrapper - Win32 Release" (based on "Win32 (x86) Static Library")
!MESSAGE "DX7Wrapper - Win32 Debug" (based on "Win32 (x86) Static Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "DX7Wrapper - Win32 Release"

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
# ADD CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /YX /FD /c
# ADD BASE RSC /l 0xc0a /d "NDEBUG"
# ADD RSC /l 0xc0a /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo

!ELSEIF  "$(CFG)" == "DX7Wrapper - Win32 Debug"

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
# ADD CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /YX /FD /GZ /c
# ADD BASE RSC /l 0xc0a /d "_DEBUG"
# ADD RSC /l 0xc0a /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo

!ENDIF 

# Begin Target

# Name "DX7Wrapper - Win32 Release"
# Name "DX7Wrapper - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\DXD3D.cpp
# End Source File
# Begin Source File

SOURCE=.\DXD3DDevice.cpp
# End Source File
# Begin Source File

SOURCE=.\DXD3DError.cpp
# End Source File
# Begin Source File

SOURCE=.\DXD3DQuad.cpp
# End Source File
# Begin Source File

SOURCE=.\DXDDClipper.cpp
# End Source File
# Begin Source File

SOURCE=.\DXDDError.cpp
# End Source File
# Begin Source File

SOURCE=.\DXDDPalette.cpp
# End Source File
# Begin Source File

SOURCE=.\DXDDSurface.cpp
# End Source File
# Begin Source File

SOURCE=.\DXDDSurfaceBack.cpp
# End Source File
# Begin Source File

SOURCE=.\DXDDSurfaceScreen.cpp
# End Source File
# Begin Source File

SOURCE=.\DXDDSurfaceTexture.cpp
# End Source File
# Begin Source File

SOURCE=.\DXDIError.cpp
# End Source File
# Begin Source File

SOURCE=.\DXDIKeyboard.cpp
# End Source File
# Begin Source File

SOURCE=.\DXDIMouse.cpp
# End Source File
# Begin Source File

SOURCE=.\DXDMError.cpp
# End Source File
# Begin Source File

SOURCE=.\DXDMManager.cpp
# End Source File
# Begin Source File

SOURCE=.\DXDMSegment.cpp
# End Source File
# Begin Source File

SOURCE=.\DXDraw.cpp
# End Source File
# Begin Source File

SOURCE=.\DXDSError.cpp
# End Source File
# Begin Source File

SOURCE=.\DXDSWAVBuffer.cpp
# End Source File
# Begin Source File

SOURCE=.\DXDSWAVSound.cpp
# End Source File
# Begin Source File

SOURCE=.\DXInput.cpp
# End Source File
# Begin Source File

SOURCE=.\DXSound.cpp
# End Source File
# Begin Source File

SOURCE=.\DXWrapper.cpp
# End Source File
# Begin Source File

SOURCE=.\StdAfx.cpp
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=.\DXD3D.h
# End Source File
# Begin Source File

SOURCE=.\DXD3DDefs.h
# End Source File
# Begin Source File

SOURCE=.\DXD3DDevice.h
# End Source File
# Begin Source File

SOURCE=.\DXD3DError.h
# End Source File
# Begin Source File

SOURCE=.\DXD3DQuad.h
# End Source File
# Begin Source File

SOURCE=.\DXDDClipper.h
# End Source File
# Begin Source File

SOURCE=.\DXDDDefs.h
# End Source File
# Begin Source File

SOURCE=.\DXDDError.h
# End Source File
# Begin Source File

SOURCE=.\DXDDPalette.h
# End Source File
# Begin Source File

SOURCE=.\DXDDSurface.h
# End Source File
# Begin Source File

SOURCE=.\DXDDSurfaceBack.h
# End Source File
# Begin Source File

SOURCE=.\DXDDSurfaceScreen.h
# End Source File
# Begin Source File

SOURCE=.\DXDDSurfaceTexture.h
# End Source File
# Begin Source File

SOURCE=.\DXDIDefs.h
# End Source File
# Begin Source File

SOURCE=.\DXDIError.h
# End Source File
# Begin Source File

SOURCE=.\DXDIKeyboard.h
# End Source File
# Begin Source File

SOURCE=.\DXDIMouse.h
# End Source File
# Begin Source File

SOURCE=.\DXDMDefs.h
# End Source File
# Begin Source File

SOURCE=.\DXDMError.h
# End Source File
# Begin Source File

SOURCE=.\DXDMManager.h
# End Source File
# Begin Source File

SOURCE=.\DXDMSegment.h
# End Source File
# Begin Source File

SOURCE=.\DXDraw.h
# End Source File
# Begin Source File

SOURCE=.\DXDSDefs.h
# End Source File
# Begin Source File

SOURCE=.\DXDSError.h
# End Source File
# Begin Source File

SOURCE=.\DXDSWAVBuffer.h
# End Source File
# Begin Source File

SOURCE=.\DXDSWAVSound.h
# End Source File
# Begin Source File

SOURCE=.\DXInput.h
# End Source File
# Begin Source File

SOURCE=.\DXSound.h
# End Source File
# Begin Source File

SOURCE=.\DXWrapper.h
# End Source File
# Begin Source File

SOURCE=.\StdAfx.h
# End Source File
# End Group
# End Target
# End Project
