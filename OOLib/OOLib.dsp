# Microsoft Developer Studio Project File - Name="OOLib" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Static Library" 0x0104

CFG=OOLib - Win32 Release
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "OOLib.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "OOLib.mak" CFG="OOLib - Win32 Release"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "OOLib - Win32 Release" (based on "Win32 (x86) Static Library")
!MESSAGE "OOLib - Win32 Debug" (based on "Win32 (x86) Static Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "OOLib - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir ".\Release"
# PROP BASE Intermediate_Dir ".\Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir ".\Release"
# PROP Intermediate_Dir ".\Release"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /YX /c
# ADD CPP /nologo /G5 /W3 /Gi /O2 /I "..\OOInc" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /YX /FD /c
# ADD BASE RSC /l 0x40c
# ADD RSC /l 0x40c
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo /out:".\Release\oo.lib"

!ELSEIF  "$(CFG)" == "OOLib - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir ".\Debug"
# PROP BASE Intermediate_Dir ".\Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir ".\Debug"
# PROP Intermediate_Dir ".\Debug"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /Z7 /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /YX /c
# ADD CPP /nologo /G5 /W3 /Gi /ZI /Od /I "..\OOInc" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /Fr /YX /FD /c
# ADD BASE RSC /l 0x40c
# ADD RSC /l 0x40c
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo /out:".\Debug\oo.lib"

!ENDIF 

# Begin Target

# Name "OOLib - Win32 Release"
# Name "OOLib - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;hpj;bat;for;f90"
# Begin Source File

SOURCE=.\Array.c
# End Source File
# Begin Source File

SOURCE=.\Buf.c
# End Source File
# Begin Source File

SOURCE=.\Class_Appl.c
# End Source File
# Begin Source File

SOURCE=.\Class_Area.c
# End Source File
# Begin Source File

SOURCE=.\Class_AreaGroup.c
# End Source File
# Begin Source File

SOURCE=.\Class_AreaGroupBox.c
# End Source File
# Begin Source File

SOURCE=.\Class_AreaScreen_Win32.c
# End Source File
# Begin Source File

SOURCE=.\Class_AreaWindow_Win32.c
# End Source File
# Begin Source File

SOURCE=.\Class_Ctrl.c
# End Source File
# Begin Source File

SOURCE=.\Class_CtrlButton.c
# End Source File
# Begin Source File

SOURCE=.\Class_CtrlChecker.c
# End Source File
# Begin Source File

SOURCE=.\Class_CtrlDisplayImage.c
# End Source File
# Begin Source File

SOURCE=.\Class_CtrlDisplayNum.c
# End Source File
# Begin Source File

SOURCE=.\Class_CtrlDisplayText.c
# End Source File
# Begin Source File

SOURCE=.\Class_CtrlEditNum.c
# End Source File
# Begin Source File

SOURCE=.\Class_CtrlEditText.c
# End Source File
# Begin Source File

SOURCE=.\Class_CtrlList.c
# End Source File
# Begin Source File

SOURCE=.\Class_CtrlRadio.c
# End Source File
# Begin Source File

SOURCE=.\Class_CtrlScroller.c
# End Source File
# Begin Source File

SOURCE=.\Class_HandleGUI.c
# End Source File
# Begin Source File

SOURCE=.\Class_HandleTimer.c
# End Source File
# Begin Source File

SOURCE=.\Class_Root.c
# End Source File
# Begin Source File

SOURCE=.\Class_SysEvt.c
# End Source File
# Begin Source File

SOURCE=.\Classes.c
# End Source File
# Begin Source File

SOURCE=.\Data.c
# End Source File
# Begin Source File

SOURCE=.\Gfx.c
# End Source File
# Begin Source File

SOURCE=.\Gfx_BMap_Win32.c
# End Source File
# Begin Source File

SOURCE=.\Gfx_Draw_Win32.c
# End Source File
# Begin Source File

SOURCE=.\Gfx_Draw_Win32_Hand.c
# End Source File
# Begin Source File

SOURCE=.\Gfx_Draw_Win32_Sys.c
# End Source File
# Begin Source File

SOURCE=.\Gfx_File_Win32.c
# End Source File
# Begin Source File

SOURCE=.\Gfx_Font_Win32.c
# End Source File
# Begin Source File

SOURCE=.\Init.c
# End Source File
# Begin Source File

SOURCE=.\List.c
# End Source File
# Begin Source File

SOURCE=.\Pictures.c
# End Source File
# Begin Source File

SOURCE=.\Pool.c
# End Source File
# Begin Source File

SOURCE=.\PoolI.c
# End Source File
# Begin Source File

SOURCE=..\OO3d\Sqrt.c
# End Source File
# Begin Source File

SOURCE=.\Tags.c
# End Source File
# Begin Source File

SOURCE=.\Text.c
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl;fi;fd"
# Begin Source File

SOURCE=..\OOInc\oo_array.h
# End Source File
# Begin Source File

SOURCE=..\OOInc\oo_array_prv.h
# End Source File
# Begin Source File

SOURCE=..\OOInc\oo_buf.h
# End Source File
# Begin Source File

SOURCE=..\OOInc\oo_buf_prv.h
# End Source File
# Begin Source File

SOURCE=..\OOInc\Oo_class.h
# End Source File
# Begin Source File

SOURCE=..\OOInc\oo_class_prv.h
# End Source File
# Begin Source File

SOURCE=..\OOInc\oo_gfx.h
# End Source File
# Begin Source File

SOURCE=..\OOInc\oo_gfx_prv.h
# End Source File
# Begin Source File

SOURCE=..\OOInc\oo_gui.h
# End Source File
# Begin Source File

SOURCE=..\OOInc\oo_gui_prv.h
# End Source File
# Begin Source File

SOURCE=..\OOInc\oo_init.h
# End Source File
# Begin Source File

SOURCE=..\OOInc\oo_list.h
# End Source File
# Begin Source File

SOURCE=..\OOInc\oo_list_prv.h
# End Source File
# Begin Source File

SOURCE=..\OOInc\oo_pictures.h
# End Source File
# Begin Source File

SOURCE=..\OOInc\oo_pool.h
# End Source File
# Begin Source File

SOURCE=..\OOInc\oo_pool_prv.h
# End Source File
# Begin Source File

SOURCE=..\OOInc\oo_pooli.h
# End Source File
# Begin Source File

SOURCE=..\OOInc\oo_pooli_prv.h
# End Source File
# Begin Source File

SOURCE=..\OOInc\oo_tags.h
# End Source File
# Begin Source File

SOURCE=..\OOInc\oo_tags_prv.h
# End Source File
# Begin Source File

SOURCE=..\OOInc\oo_text.h
# End Source File
# Begin Source File

SOURCE=..\OOInc\oo_text_prv.h
# End Source File
# Begin Source File

SOURCE=..\OOInc\ooprotos.h
# End Source File
# Begin Source File

SOURCE=..\OOInc\Ootypes.h
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;cnt;rtf;gif;jpg;jpeg;jpe"
# End Group
# End Target
# End Project
