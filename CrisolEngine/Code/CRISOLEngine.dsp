# Microsoft Developer Studio Project File - Name="CRISOLEngine" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Application" 0x0101

CFG=CRISOLEngine - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "CRISOLEngine.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "CRISOLEngine.mak" CFG="CRISOLEngine - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "CRISOLEngine - Win32 Release" (based on "Win32 (x86) Application")
!MESSAGE "CRISOLEngine - Win32 Debug" (based on "Win32 (x86) Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 1
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "CRISOLEngine - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "Release"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /YX /FD /c
# ADD CPP /nologo /W3 /GX /O2 /D "_NDEBUG" /D "WIN32" /D "_WINDOWS" /D "_MBCS" /D "_CRISOLENGINE" /D "ENGINE_TRACE" /D "_SYSASSERT" /Fr /YX /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0xc0a /d "NDEBUG"
# ADD RSC /l 0xc0a /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /machine:I386
# ADD LINK32 winmm.lib dxguid.lib dinput.lib ddraw.lib dsound.lib dx7wrapper.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /machine:I386

!ELSEIF  "$(CFG)" == "CRISOLEngine - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /YX /FD /GZ /c
# ADD CPP /nologo /W3 /Gm /GX /ZI /D "_DEBUG" /D "WIN32" /D "_WINDOWS" /D "_MBCS" /D "_CRISOLENGINE" /D "_SYSASSERT" /D "ENGINE_TRACE" /YX /FD /GZ /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0xc0a /d "_DEBUG"
# ADD RSC /l 0xc0a /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /debug /machine:I386 /pdbtype:sept
# ADD LINK32 winmm.lib dxguid.lib ddraw.lib dinput.lib dsound.lib dx7wrapper.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /debug /machine:I386 /nodefaultlib:"LIBC" /pdbtype:sept
# SUBTRACT LINK32 /pdb:none

!ENDIF 

# Begin Target

# Name "CRISOLEngine - Win32 Release"
# Name "CRISOLEngine - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\CAlarmManager.cpp
# End Source File
# Begin Source File

SOURCE=.\CAnimCmd.cpp
# End Source File
# Begin Source File

SOURCE=.\CAnimTemplate.cpp
# End Source File
# Begin Source File

SOURCE=.\CAnimTemplateData.cpp
# End Source File
# Begin Source File

SOURCE=.\CArea.cpp
# End Source File
# Begin Source File

SOURCE=.\CAudioSystem.cpp
# End Source File
# Begin Source File

SOURCE=.\CBitmap.cpp
# End Source File
# Begin Source File

SOURCE=.\CCBTEngineParser.cpp
# End Source File
# Begin Source File

SOURCE=.\CCBTParser.cpp
# End Source File
# Begin Source File

SOURCE=.\CCharFont.cpp
# End Source File
# Begin Source File

SOURCE=.\CCinemaCurtain.cpp
# End Source File
# Begin Source File

SOURCE=.\CCinemaCurtainCmd.cpp
# End Source File
# Begin Source File

SOURCE=.\CCloseCinemaCurtainCmd.cpp
# End Source File
# Begin Source File

SOURCE=.\CCombatSystem.cpp
# End Source File
# Begin Source File

SOURCE=.\CCommand.cpp
# End Source File
# Begin Source File

SOURCE=.\CCommandManager.cpp
# End Source File
# Begin Source File

SOURCE=.\CCPAKFile.cpp
# End Source File
# Begin Source File

SOURCE=.\CCriature.cpp
# End Source File
# Begin Source File

SOURCE=.\CCRISOLEngine.cpp
# End Source File
# Begin Source File

SOURCE=.\CCyclicFadeCmd.cpp
# End Source File
# Begin Source File

SOURCE=.\CEntity.cpp
# End Source File
# Begin Source File

SOURCE=.\CEntitySelector.cpp
# End Source File
# Begin Source File

SOURCE=.\CEquipmentSlots.cpp
# End Source File
# Begin Source File

SOURCE=.\CFadeCmd.cpp
# End Source File
# Begin Source File

SOURCE=.\CFileParser.cpp
# End Source File
# Begin Source File

SOURCE=.\CFileSystem.cpp
# End Source File
# Begin Source File

SOURCE=.\CFloor.cpp
# End Source File
# Begin Source File

SOURCE=.\CFont.cpp
# End Source File
# Begin Source File

SOURCE=.\CFontSystem.cpp
# End Source File
# Begin Source File

SOURCE=.\CFXBack.cpp
# End Source File
# Begin Source File

SOURCE=.\CGameDataBase.cpp
# End Source File
# Begin Source File

SOURCE=.\CGameState.cpp
# End Source File
# Begin Source File

SOURCE=.\CGFX.cpp
# End Source File
# Begin Source File

SOURCE=.\CGFXManager.cpp
# End Source File
# Begin Source File

SOURCE=.\CGraphicLoader.cpp
# End Source File
# Begin Source File

SOURCE=.\CGraphicSystem.cpp
# End Source File
# Begin Source File

SOURCE=.\CGUICAnimBitmap.cpp
# End Source File
# Begin Source File

SOURCE=.\CGUICBitmap.cpp
# End Source File
# Begin Source File

SOURCE=.\CGUICBlockText.cpp
# End Source File
# Begin Source File

SOURCE=.\CGUICButton.cpp
# End Source File
# Begin Source File

SOURCE=.\CGUICInput.cpp
# End Source File
# Begin Source File

SOURCE=.\CGUICJustifyText.cpp
# End Source File
# Begin Source File

SOURCE=.\CGUICLineText.cpp
# End Source File
# Begin Source File

SOURCE=.\CGUIComponent.cpp
# End Source File
# Begin Source File

SOURCE=.\CGUICPercentageBar.cpp
# End Source File
# Begin Source File

SOURCE=.\CGUICSingleText.cpp
# End Source File
# Begin Source File

SOURCE=.\CGUICSpriteSelector.cpp
# End Source File
# Begin Source File

SOURCE=.\CGUIManager.cpp
# End Source File
# Begin Source File

SOURCE=.\CGUIWBaseItemSelector.cpp
# End Source File
# Begin Source File

SOURCE=.\CGUIWBaseTextSelector.cpp
# End Source File
# Begin Source File

SOURCE=.\CGUIWBaseWindowContainer.cpp
# End Source File
# Begin Source File

SOURCE=.\CGUIWCharacterCreator.cpp
# End Source File
# Begin Source File

SOURCE=.\CGUIWCombat.cpp
# End Source File
# Begin Source File

SOURCE=.\CGUIWConversator.cpp
# End Source File
# Begin Source File

SOURCE=.\CGUIWDialog.cpp
# End Source File
# Begin Source File

SOURCE=.\CGUIWEntityInteract.cpp
# End Source File
# Begin Source File

SOURCE=.\CGUIWGameLoading.cpp
# End Source File
# Begin Source File

SOURCE=.\CGUIWGameMenu.cpp
# End Source File
# Begin Source File

SOURCE=.\CGUIWindow.cpp
# End Source File
# Begin Source File

SOURCE=.\CGUIWItemSelectToUse.cpp
# End Source File
# Begin Source File

SOURCE=.\CGUIWMainInterfaz.cpp
# End Source File
# Begin Source File

SOURCE=.\CGUIWMainMenu.cpp
# End Source File
# Begin Source File

SOURCE=.\CGUIWPlayerProfile.cpp
# End Source File
# Begin Source File

SOURCE=.\CGUIWPresentation.cpp
# End Source File
# Begin Source File

SOURCE=.\CGUIWSavedFileManager.cpp
# End Source File
# Begin Source File

SOURCE=.\CGUIWTextOptionsSelector.cpp
# End Source File
# Begin Source File

SOURCE=.\CGUIWTextReader.cpp
# End Source File
# Begin Source File

SOURCE=.\CGUIWTrade.cpp
# End Source File
# Begin Source File

SOURCE=.\CImage.cpp
# End Source File
# Begin Source File

SOURCE=.\CInputManager.cpp
# End Source File
# Begin Source File

SOURCE=.\CIsoEngine.cpp
# End Source File
# Begin Source File

SOURCE=.\CIsoTravellingCmd.cpp
# End Source File
# Begin Source File

SOURCE=.\CItem.cpp
# End Source File
# Begin Source File

SOURCE=.\CItemContainer.cpp
# End Source File
# Begin Source File

SOURCE=.\CItemContainerIt.cpp
# End Source File
# Begin Source File

SOURCE=.\CLinearInterpolation.cpp
# End Source File
# Begin Source File

SOURCE=.\CLogger.cpp
# End Source File
# Begin Source File

SOURCE=.\CMathUtil.cpp
# End Source File
# Begin Source File

SOURCE=.\CMemoryPool.cpp
# End Source File
# Begin Source File

SOURCE=.\CMoveCmd.cpp
# End Source File
# Begin Source File

SOURCE=.\CMovIsoEngineCmd.cpp
# End Source File
# Begin Source File

SOURCE=.\COpenCinemaCurtainCmd.cpp
# End Source File
# Begin Source File

SOURCE=.\CPath.cpp
# End Source File
# Begin Source File

SOURCE=.\CPlayer.cpp
# End Source File
# Begin Source File

SOURCE=.\CQuadForm.cpp
# End Source File
# Begin Source File

SOURCE=.\CRecycleNodePool.cpp
# End Source File
# Begin Source File

SOURCE=.\CResourceManager.cpp
# End Source File
# Begin Source File

SOURCE=.\CRISOLEngine.cpp
# End Source File
# Begin Source File

SOURCE=.\CRoof.cpp
# End Source File
# Begin Source File

SOURCE=.\CRulesDataBase.cpp
# End Source File
# Begin Source File

SOURCE=.\CSceneObj.cpp
# End Source File
# Begin Source File

SOURCE=.\CScript.cpp
# End Source File
# Begin Source File

SOURCE=.\CScriptInstructions.cpp
# End Source File
# Begin Source File

SOURCE=.\CScriptStackValue.cpp
# End Source File
# Begin Source File

SOURCE=.\CShadow.cpp
# End Source File
# Begin Source File

SOURCE=.\CSprite.cpp
# End Source File
# Begin Source File

SOURCE=.\CTimer.cpp
# End Source File
# Begin Source File

SOURCE=.\CVirtualMachine.cpp
# End Source File
# Begin Source File

SOURCE=.\CWall.cpp
# End Source File
# Begin Source File

SOURCE=.\CWorld.cpp
# End Source File
# Begin Source File

SOURCE=.\CWorldEntity.cpp
# End Source File
# Begin Source File

SOURCE=.\StdAfx.cpp
# End Source File
# Begin Source File

SOURCE=.\SYSAssert.cpp
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=.\CAlarmManager.h
# End Source File
# Begin Source File

SOURCE=.\CAnimCmd.h
# End Source File
# Begin Source File

SOURCE=.\CAnimTemplate.h
# End Source File
# Begin Source File

SOURCE=.\CAnimTemplateData.h
# End Source File
# Begin Source File

SOURCE=.\CArea.h
# End Source File
# Begin Source File

SOURCE=.\CAudioSystem.h
# End Source File
# Begin Source File

SOURCE=.\CBitmap.h
# End Source File
# Begin Source File

SOURCE=.\CCBTEngineParser.h
# End Source File
# Begin Source File

SOURCE=.\CCBTParser.h
# End Source File
# Begin Source File

SOURCE=.\CCharFont.h
# End Source File
# Begin Source File

SOURCE=.\CCinemaCurtain.h
# End Source File
# Begin Source File

SOURCE=.\CCinemaCurtainCmd.h
# End Source File
# Begin Source File

SOURCE=.\CCloseCinemaCurtainCmd.h
# End Source File
# Begin Source File

SOURCE=.\CCombatSystem.h
# End Source File
# Begin Source File

SOURCE=.\CCommand.h
# End Source File
# Begin Source File

SOURCE=.\CCommandManager.h
# End Source File
# Begin Source File

SOURCE=.\CCPAKFile.h
# End Source File
# Begin Source File

SOURCE=.\CCriature.h
# End Source File
# Begin Source File

SOURCE=.\CCRISOLEngine.h
# End Source File
# Begin Source File

SOURCE=.\CCyclicFadeCmd.h
# End Source File
# Begin Source File

SOURCE=.\CDrawable.h
# End Source File
# Begin Source File

SOURCE=.\CEntity.h
# End Source File
# Begin Source File

SOURCE=.\CEntitySelector.h
# End Source File
# Begin Source File

SOURCE=.\CEquipmentSlots.h
# End Source File
# Begin Source File

SOURCE=.\CFadeCmd.h
# End Source File
# Begin Source File

SOURCE=.\CFileParser.h
# End Source File
# Begin Source File

SOURCE=.\CFileSystem.h
# End Source File
# Begin Source File

SOURCE=.\CFloor.h
# End Source File
# Begin Source File

SOURCE=.\CFont.h
# End Source File
# Begin Source File

SOURCE=.\CFontSystem.h
# End Source File
# Begin Source File

SOURCE=.\CFXBack.h
# End Source File
# Begin Source File

SOURCE=.\CGameDataBase.h
# End Source File
# Begin Source File

SOURCE=.\CGameState.h
# End Source File
# Begin Source File

SOURCE=.\CGFX.h
# End Source File
# Begin Source File

SOURCE=.\CGFXManager.h
# End Source File
# Begin Source File

SOURCE=.\CGraphicLoader.h
# End Source File
# Begin Source File

SOURCE=.\CGraphicSystem.h
# End Source File
# Begin Source File

SOURCE=.\CGUICAnimBitmap.h
# End Source File
# Begin Source File

SOURCE=.\CGUICBitmap.h
# End Source File
# Begin Source File

SOURCE=.\CGUICBlockText.h
# End Source File
# Begin Source File

SOURCE=.\CGUICButton.h
# End Source File
# Begin Source File

SOURCE=.\CGUICInput.h
# End Source File
# Begin Source File

SOURCE=.\CGUICJustifyText.h
# End Source File
# Begin Source File

SOURCE=.\CGUICLineText.h
# End Source File
# Begin Source File

SOURCE=.\CGUIComponent.h
# End Source File
# Begin Source File

SOURCE=.\CGUICPercentageBar.h
# End Source File
# Begin Source File

SOURCE=.\CGUICSingleText.h
# End Source File
# Begin Source File

SOURCE=.\CGUICSpriteSelector.h
# End Source File
# Begin Source File

SOURCE=.\CGUIManager.h
# End Source File
# Begin Source File

SOURCE=.\CGUIWBaseItemSelector.h
# End Source File
# Begin Source File

SOURCE=.\CGUIWBaseTextSelector.h
# End Source File
# Begin Source File

SOURCE=.\CGUIWBaseWindowContainer.h
# End Source File
# Begin Source File

SOURCE=.\CGUIWCharacterCreator.h
# End Source File
# Begin Source File

SOURCE=.\CGUIWCombat.h
# End Source File
# Begin Source File

SOURCE=.\CGUIWConversator.h
# End Source File
# Begin Source File

SOURCE=.\CGUIWDialog.h
# End Source File
# Begin Source File

SOURCE=.\CGUIWEntityInteract.h
# End Source File
# Begin Source File

SOURCE=.\CGUIWGameLoading.h
# End Source File
# Begin Source File

SOURCE=.\CGUIWGameMenu.h
# End Source File
# Begin Source File

SOURCE=.\CGUIWindow.h
# End Source File
# Begin Source File

SOURCE=.\CGUIWItemSelectToUse.h
# End Source File
# Begin Source File

SOURCE=.\CGUIWMainInterfaz.h
# End Source File
# Begin Source File

SOURCE=.\CGUIWMainMenu.h
# End Source File
# Begin Source File

SOURCE=.\CGUIWPlayerProfile.h
# End Source File
# Begin Source File

SOURCE=.\CGUIWPresentation.h
# End Source File
# Begin Source File

SOURCE=.\CGUIWSavedFileManager.h
# End Source File
# Begin Source File

SOURCE=.\CGUIWTextOptionsSelector.h
# End Source File
# Begin Source File

SOURCE=.\CGUIWTextReader.h
# End Source File
# Begin Source File

SOURCE=.\CGUIWTrade.h
# End Source File
# Begin Source File

SOURCE=.\CImage.h
# End Source File
# Begin Source File

SOURCE=.\CInputManager.h
# End Source File
# Begin Source File

SOURCE=.\CIsoEngine.h
# End Source File
# Begin Source File

SOURCE=.\CIsoTravellingCmd.h
# End Source File
# Begin Source File

SOURCE=.\CItem.h
# End Source File
# Begin Source File

SOURCE=.\CItemContainer.h
# End Source File
# Begin Source File

SOURCE=.\CItemContainerIt.h
# End Source File
# Begin Source File

SOURCE=.\CLinearInterpolation.h
# End Source File
# Begin Source File

SOURCE=.\CLogger.h
# End Source File
# Begin Source File

SOURCE=.\CMathUtil.h
# End Source File
# Begin Source File

SOURCE=.\CMemoryPool.h
# End Source File
# Begin Source File

SOURCE=.\CMoveCmd.h
# End Source File
# Begin Source File

SOURCE=.\CMovIsoEngineCmd.h
# End Source File
# Begin Source File

SOURCE=.\COpenCinemaCurtainCmd.h
# End Source File
# Begin Source File

SOURCE=.\CPath.h
# End Source File
# Begin Source File

SOURCE=.\CPlayer.h
# End Source File
# Begin Source File

SOURCE=.\CQuadForm.h
# End Source File
# Begin Source File

SOURCE=.\CRecycleNodePool.h
# End Source File
# Begin Source File

SOURCE=.\CResourceManager.h
# End Source File
# Begin Source File

SOURCE=.\CRoof.h
# End Source File
# Begin Source File

SOURCE=.\CRulesDataBase.h
# End Source File
# Begin Source File

SOURCE=.\CSceneObj.h
# End Source File
# Begin Source File

SOURCE=.\CScript.h
# End Source File
# Begin Source File

SOURCE=.\CScriptInstructions.h
# End Source File
# Begin Source File

SOURCE=.\CScriptStackValue.h
# End Source File
# Begin Source File

SOURCE=.\CShadow.h
# End Source File
# Begin Source File

SOURCE=.\CSprite.h
# End Source File
# Begin Source File

SOURCE=.\CTimer.h
# End Source File
# Begin Source File

SOURCE=.\CVirtualMachine.h
# End Source File
# Begin Source File

SOURCE=.\CWall.h
# End Source File
# Begin Source File

SOURCE=.\CWorld.h
# End Source File
# Begin Source File

SOURCE=.\CWorldEntity.h
# End Source File
# Begin Source File

SOURCE=.\StdAfx.h
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# End Group
# Begin Group "Defs"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\AlarmDefs.h
# End Source File
# Begin Source File

SOURCE=.\AnimTemplateDefs.h
# End Source File
# Begin Source File

SOURCE=.\AnimTemplateObserverDefs.h
# End Source File
# Begin Source File

SOURCE=.\AreaDefs.h
# End Source File
# Begin Source File

SOURCE=.\AudioDefs.h
# End Source File
# Begin Source File

SOURCE=.\CombatSystemDefs.h
# End Source File
# Begin Source File

SOURCE=.\CommandDefs.h
# End Source File
# Begin Source File

SOURCE=.\CriatureDefs.h
# End Source File
# Begin Source File

SOURCE=.\CriatureObserverDefs.h
# End Source File
# Begin Source File

SOURCE=.\EquipmentSlotsDefs.h
# End Source File
# Begin Source File

SOURCE=.\FileDefs.h
# End Source File
# Begin Source File

SOURCE=.\FontDefs.h
# End Source File
# Begin Source File

SOURCE=.\GameDataBaseDefs.h
# End Source File
# Begin Source File

SOURCE=.\GameStateDefs.h
# End Source File
# Begin Source File

SOURCE=.\GFXManagerDefs.h
# End Source File
# Begin Source File

SOURCE=.\GraphDefs.h
# End Source File
# Begin Source File

SOURCE=.\GUIDefs.h
# End Source File
# Begin Source File

SOURCE=.\GUIManagerDefs.h
# End Source File
# Begin Source File

SOURCE=.\InputDefs.h
# End Source File
# Begin Source File

SOURCE=.\IsoDefs.h
# End Source File
# Begin Source File

SOURCE=.\ItemContainerDefs.h
# End Source File
# Begin Source File

SOURCE=.\ResDefs.h
# End Source File
# Begin Source File

SOURCE=.\RulesDefs.h
# End Source File
# Begin Source File

SOURCE=.\ScriptClientDefs.h
# End Source File
# Begin Source File

SOURCE=.\ScriptDefs.h
# End Source File
# Begin Source File

SOURCE=.\SYSAssert.h
# End Source File
# Begin Source File

SOURCE=.\SYSDefs.h
# End Source File
# Begin Source File

SOURCE=.\SYSEngine.h
# End Source File
# Begin Source File

SOURCE=.\TimerDefs.h
# End Source File
# Begin Source File

SOURCE=.\WorldDefs.h
# End Source File
# Begin Source File

SOURCE=.\WorldObserverDefs.h
# End Source File
# End Group
# Begin Group "Interfaces"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\iCAlarmClient.h
# End Source File
# Begin Source File

SOURCE=.\iCAlarmManager.h
# End Source File
# Begin Source File

SOURCE=.\iCAnimTemplate.h
# End Source File
# Begin Source File

SOURCE=.\iCAnimTemplateData.h
# End Source File
# Begin Source File

SOURCE=.\iCAnimTemplateObserver.h
# End Source File
# Begin Source File

SOURCE=.\iCAudioSystem.h
# End Source File
# Begin Source File

SOURCE=.\iCCombatSystem.h
# End Source File
# Begin Source File

SOURCE=.\iCCommandClient.h
# End Source File
# Begin Source File

SOURCE=.\iCCommandManager.h
# End Source File
# Begin Source File

SOURCE=.\iCCriatureObserver.h
# End Source File
# Begin Source File

SOURCE=.\iCDrawable.h
# End Source File
# Begin Source File

SOURCE=.\iCEntitySelector.h
# End Source File
# Begin Source File

SOURCE=.\iCEquipmentSlots.h
# End Source File
# Begin Source File

SOURCE=.\iCFileSystem.h
# End Source File
# Begin Source File

SOURCE=.\iCFont.h
# End Source File
# Begin Source File

SOURCE=.\iCFontSystem.h
# End Source File
# Begin Source File

SOURCE=.\iCGameDataBase.h
# End Source File
# Begin Source File

SOURCE=.\iCGFXManager.h
# End Source File
# Begin Source File

SOURCE=.\iCGraphicLoader.h
# End Source File
# Begin Source File

SOURCE=.\iCGraphicSystem.h
# End Source File
# Begin Source File

SOURCE=.\iCGUIManager.h
# End Source File
# Begin Source File

SOURCE=.\iCGUIWindow.h
# End Source File
# Begin Source File

SOURCE=.\iCGUIWindowClient.h
# End Source File
# Begin Source File

SOURCE=.\iCInputClient.h
# End Source File
# Begin Source File

SOURCE=.\iCInputManager.h
# End Source File
# Begin Source File

SOURCE=.\iCItemContainer.h
# End Source File
# Begin Source File

SOURCE=.\iCLogger.h
# End Source File
# Begin Source File

SOURCE=.\iCMathUtil.h
# End Source File
# Begin Source File

SOURCE=.\iCResourceManager.h
# End Source File
# Begin Source File

SOURCE=.\iCRulesDataBase.h
# End Source File
# Begin Source File

SOURCE=.\iCScript.h
# End Source File
# Begin Source File

SOURCE=.\iCScriptClient.h
# End Source File
# Begin Source File

SOURCE=.\iCTimer.h
# End Source File
# Begin Source File

SOURCE=.\iCVirtualMachine.h
# End Source File
# Begin Source File

SOURCE=.\iCWorld.h
# End Source File
# Begin Source File

SOURCE=.\iCWorldObserver.h
# End Source File
# End Group
# End Target
# End Project
