# Microsoft Developer Studio Generated NMAKE File, Format Version 4.20
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Static Library" 0x0104

!IF "$(CFG)" == ""
CFG=OOLib - Win32 Debug
!MESSAGE No configuration specified.  Defaulting to OOLib - Win32 Debug.
!ENDIF 

!IF "$(CFG)" != "OOLib - Win32 Release" && "$(CFG)" != "OOLib - Win32 Debug"
!MESSAGE Invalid configuration "$(CFG)" specified.
!MESSAGE You can specify a configuration when running NMAKE on this makefile
!MESSAGE by defining the macro CFG on the command line.  For example:
!MESSAGE 
!MESSAGE NMAKE /f "OOLib.mak" CFG="OOLib - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "OOLib - Win32 Release" (based on "Win32 (x86) Static Library")
!MESSAGE "OOLib - Win32 Debug" (based on "Win32 (x86) Static Library")
!MESSAGE 
!ERROR An invalid configuration is specified.
!ENDIF 

!IF "$(OS)" == "Windows_NT"
NULL=
!ELSE 
NULL=nul
!ENDIF 
################################################################################
# Begin Project
# PROP Target_Last_Scanned "OOLib - Win32 Debug"
CPP=cl.exe

!IF  "$(CFG)" == "OOLib - Win32 Release"

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
OUTDIR=.\Release
INTDIR=.\Release

ALL : "$(OUTDIR)\oo.lib"

CLEAN : 
	-@erase "$(INTDIR)\Array.obj"
	-@erase "$(INTDIR)\Buf.obj"
	-@erase "$(INTDIR)\Class_Appl.obj"
	-@erase "$(INTDIR)\Class_Area.obj"
	-@erase "$(INTDIR)\Class_AreaGroup.obj"
	-@erase "$(INTDIR)\Class_AreaScreen_Win32.obj"
	-@erase "$(INTDIR)\Class_AreaWindow_Win32.obj"
	-@erase "$(INTDIR)\Class_HdlGUI.obj"
	-@erase "$(INTDIR)\Class_Root.obj"
	-@erase "$(INTDIR)\Class_SysEvt.obj"
	-@erase "$(INTDIR)\Classes.obj"
	-@erase "$(INTDIR)\Gfx.obj"
	-@erase "$(INTDIR)\Init.obj"
	-@erase "$(INTDIR)\List.obj"
	-@erase "$(INTDIR)\Pool.obj"
	-@erase "$(INTDIR)\PoolI.obj"
	-@erase "$(INTDIR)\Tags.obj"
	-@erase "$(INTDIR)\Text.obj"
	-@erase "$(OUTDIR)\oo.lib"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /YX /c
# ADD CPP /nologo /G5 /Zp4 /W3 /Gi /O2 /I "..\OOInc" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /YX /c
CPP_PROJ=/nologo /G5 /Zp4 /ML /W3 /Gi /O2 /I "..\OOInc" /D "WIN32" /D "NDEBUG"\
 /D "_WINDOWS" /Fp"$(INTDIR)/OOLib.pch" /YX /Fo"$(INTDIR)/" /Fd"$(INTDIR)/" /c 
CPP_OBJS=.\Release/
CPP_SBRS=.\.
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o"$(OUTDIR)/OOLib.bsc" 
BSC32_SBRS= \
	
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo /out:"Release\oo.lib"
LIB32_FLAGS=/nologo /out:"$(OUTDIR)/oo.lib" 
LIB32_OBJS= \
	"$(INTDIR)\Array.obj" \
	"$(INTDIR)\Buf.obj" \
	"$(INTDIR)\Class_Appl.obj" \
	"$(INTDIR)\Class_Area.obj" \
	"$(INTDIR)\Class_AreaGroup.obj" \
	"$(INTDIR)\Class_AreaScreen_Win32.obj" \
	"$(INTDIR)\Class_AreaWindow_Win32.obj" \
	"$(INTDIR)\Class_HdlGUI.obj" \
	"$(INTDIR)\Class_Root.obj" \
	"$(INTDIR)\Class_SysEvt.obj" \
	"$(INTDIR)\Classes.obj" \
	"$(INTDIR)\Gfx.obj" \
	"$(INTDIR)\Init.obj" \
	"$(INTDIR)\List.obj" \
	"$(INTDIR)\Pool.obj" \
	"$(INTDIR)\PoolI.obj" \
	"$(INTDIR)\Tags.obj" \
	"$(INTDIR)\Text.obj"

"$(OUTDIR)\oo.lib" : "$(OUTDIR)" $(DEF_FILE) $(LIB32_OBJS)
    $(LIB32) @<<
  $(LIB32_FLAGS) $(DEF_FLAGS) $(LIB32_OBJS)
<<

!ELSEIF  "$(CFG)" == "OOLib - Win32 Debug"

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
OUTDIR=.\Debug
INTDIR=.\Debug

ALL : "$(OUTDIR)\oo.lib"

CLEAN : 
	-@erase "$(INTDIR)\Array.obj"
	-@erase "$(INTDIR)\Buf.obj"
	-@erase "$(INTDIR)\Class_Appl.obj"
	-@erase "$(INTDIR)\Class_Area.obj"
	-@erase "$(INTDIR)\Class_AreaGroup.obj"
	-@erase "$(INTDIR)\Class_AreaScreen_Win32.obj"
	-@erase "$(INTDIR)\Class_AreaWindow_Win32.obj"
	-@erase "$(INTDIR)\Class_HdlGUI.obj"
	-@erase "$(INTDIR)\Class_Root.obj"
	-@erase "$(INTDIR)\Class_SysEvt.obj"
	-@erase "$(INTDIR)\Classes.obj"
	-@erase "$(INTDIR)\Gfx.obj"
	-@erase "$(INTDIR)\Init.obj"
	-@erase "$(INTDIR)\List.obj"
	-@erase "$(INTDIR)\Pool.obj"
	-@erase "$(INTDIR)\PoolI.obj"
	-@erase "$(INTDIR)\Tags.obj"
	-@erase "$(INTDIR)\Text.obj"
	-@erase "$(INTDIR)\vc40.pdb"
	-@erase "$(OUTDIR)\oo.lib"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

# ADD BASE CPP /nologo /W3 /GX /Z7 /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /YX /c
# ADD CPP /nologo /G5 /Zp4 /W3 /Gi /Zi /Od /I "..\OOInc" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /YX /c
# SUBTRACT CPP /Fr
CPP_PROJ=/nologo /G5 /Zp4 /MLd /W3 /Gi /Zi /Od /I "..\OOInc" /D "WIN32" /D\
 "_DEBUG" /D "_WINDOWS" /Fp"$(INTDIR)/OOLib.pch" /YX /Fo"$(INTDIR)/"\
 /Fd"$(INTDIR)/" /c 
CPP_OBJS=.\Debug/
CPP_SBRS=.\.
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o"$(OUTDIR)/OOLib.bsc" 
BSC32_SBRS= \
	
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo /out:"Debug\oo.lib"
LIB32_FLAGS=/nologo /out:"$(OUTDIR)/oo.lib" 
LIB32_OBJS= \
	"$(INTDIR)\Array.obj" \
	"$(INTDIR)\Buf.obj" \
	"$(INTDIR)\Class_Appl.obj" \
	"$(INTDIR)\Class_Area.obj" \
	"$(INTDIR)\Class_AreaGroup.obj" \
	"$(INTDIR)\Class_AreaScreen_Win32.obj" \
	"$(INTDIR)\Class_AreaWindow_Win32.obj" \
	"$(INTDIR)\Class_HdlGUI.obj" \
	"$(INTDIR)\Class_Root.obj" \
	"$(INTDIR)\Class_SysEvt.obj" \
	"$(INTDIR)\Classes.obj" \
	"$(INTDIR)\Gfx.obj" \
	"$(INTDIR)\Init.obj" \
	"$(INTDIR)\List.obj" \
	"$(INTDIR)\Pool.obj" \
	"$(INTDIR)\PoolI.obj" \
	"$(INTDIR)\Tags.obj" \
	"$(INTDIR)\Text.obj"

"$(OUTDIR)\oo.lib" : "$(OUTDIR)" $(DEF_FILE) $(LIB32_OBJS)
    $(LIB32) @<<
  $(LIB32_FLAGS) $(DEF_FLAGS) $(LIB32_OBJS)
<<

!ENDIF 

.c{$(CPP_OBJS)}.obj:
   $(CPP) $(CPP_PROJ) $<  

.cpp{$(CPP_OBJS)}.obj:
   $(CPP) $(CPP_PROJ) $<  

.cxx{$(CPP_OBJS)}.obj:
   $(CPP) $(CPP_PROJ) $<  

.c{$(CPP_SBRS)}.sbr:
   $(CPP) $(CPP_PROJ) $<  

.cpp{$(CPP_SBRS)}.sbr:
   $(CPP) $(CPP_PROJ) $<  

.cxx{$(CPP_SBRS)}.sbr:
   $(CPP) $(CPP_PROJ) $<  

################################################################################
# Begin Target

# Name "OOLib - Win32 Release"
# Name "OOLib - Win32 Debug"

!IF  "$(CFG)" == "OOLib - Win32 Release"

!ELSEIF  "$(CFG)" == "OOLib - Win32 Debug"

!ENDIF 

################################################################################
# Begin Source File

SOURCE=.\Array.c
DEP_CPP_ARRAY=\
	"..\OOInc\oo_array.h"\
	"..\OOInc\oo_array_prv.h"\
	"..\OOInc\oo_list.h"\
	"..\OOInc\oo_pool.h"\
	"..\OOInc\oo_pool_prv.h"\
	"..\OOInc\ooprotos.h"\
	"..\OOInc\ootypes.h"\
	

"$(INTDIR)\Array.obj" : $(SOURCE) $(DEP_CPP_ARRAY) "$(INTDIR)"


# End Source File
################################################################################
# Begin Source File

SOURCE=.\Text.c
DEP_CPP_TEXT_=\
	"..\OOInc\oo_pool.h"\
	"..\OOInc\oo_text.h"\
	"..\OOInc\oo_text_prv.h"\
	"..\OOInc\ooprotos.h"\
	"..\OOInc\ootypes.h"\
	

"$(INTDIR)\Text.obj" : $(SOURCE) $(DEP_CPP_TEXT_) "$(INTDIR)"


# End Source File
################################################################################
# Begin Source File

SOURCE=.\List.c
DEP_CPP_LIST_=\
	"..\OOInc\oo_list.h"\
	"..\OOInc\oo_list_prv.h"\
	"..\OOInc\ooprotos.h"\
	"..\OOInc\ootypes.h"\
	

"$(INTDIR)\List.obj" : $(SOURCE) $(DEP_CPP_LIST_) "$(INTDIR)"


# End Source File
################################################################################
# Begin Source File

SOURCE=.\Tags.c
DEP_CPP_TAGS_=\
	"..\OOInc\oo_pool.h"\
	"..\OOInc\oo_tags.h"\
	"..\OOInc\oo_tags_prv.h"\
	"..\OOInc\ooprotos.h"\
	"..\OOInc\ootypes.h"\
	

"$(INTDIR)\Tags.obj" : $(SOURCE) $(DEP_CPP_TAGS_) "$(INTDIR)"


# End Source File
################################################################################
# Begin Source File

SOURCE=.\Buf.c
DEP_CPP_BUF_C=\
	"..\OOInc\oo_buf.h"\
	"..\OOInc\oo_buf_prv.h"\
	"..\OOInc\oo_list.h"\
	"..\OOInc\oo_pool.h"\
	"..\OOInc\oo_pool_prv.h"\
	"..\OOInc\oo_text.h"\
	"..\OOInc\ooprotos.h"\
	"..\OOInc\ootypes.h"\
	

"$(INTDIR)\Buf.obj" : $(SOURCE) $(DEP_CPP_BUF_C) "$(INTDIR)"


# End Source File
################################################################################
# Begin Source File

SOURCE=.\Pool.c
DEP_CPP_POOL_=\
	"..\OOInc\oo_array.h"\
	"..\OOInc\oo_list.h"\
	"..\OOInc\oo_pool.h"\
	"..\OOInc\oo_pool_prv.h"\
	"..\OOInc\ooprotos.h"\
	"..\OOInc\ootypes.h"\
	

"$(INTDIR)\Pool.obj" : $(SOURCE) $(DEP_CPP_POOL_) "$(INTDIR)"


# End Source File
################################################################################
# Begin Source File

SOURCE=.\PoolI.c
DEP_CPP_POOLI=\
	"..\OOInc\oo_array.h"\
	"..\OOInc\oo_list.h"\
	"..\OOInc\oo_pooli.h"\
	"..\OOInc\oo_pooli_prv.h"\
	"..\OOInc\ooprotos.h"\
	"..\OOInc\ootypes.h"\
	

"$(INTDIR)\PoolI.obj" : $(SOURCE) $(DEP_CPP_POOLI) "$(INTDIR)"


# End Source File
################################################################################
# Begin Source File

SOURCE=.\Classes.c
DEP_CPP_CLASS=\
	"..\OOInc\class_root.h"\
	"..\OOInc\oo_class.h"\
	"..\OOInc\oo_class_prv.h"\
	"..\OOInc\oo_gui.h"\
	"..\OOInc\oo_pool.h"\
	"..\OOInc\oo_tags.h"\
	"..\OOInc\ooprotos.h"\
	"..\OOInc\ootypes.h"\
	

"$(INTDIR)\Classes.obj" : $(SOURCE) $(DEP_CPP_CLASS) "$(INTDIR)"


# End Source File
################################################################################
# Begin Source File

SOURCE=.\Class_Root.c

!IF  "$(CFG)" == "OOLib - Win32 Release"

DEP_CPP_CLASS_=\
	"..\OOInc\class_root.h"\
	"..\OOInc\oo_class.h"\
	"..\OOInc\oo_class_prv.h"\
	"..\OOInc\oo_pool.h"\
	"..\OOInc\oo_tags.h"\
	"..\OOInc\ooprotos.h"\
	"..\OOInc\ootypes.h"\
	

"$(INTDIR)\Class_Root.obj" : $(SOURCE) $(DEP_CPP_CLASS_) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "OOLib - Win32 Debug"

DEP_CPP_CLASS_=\
	"..\OOInc\class_root.h"\
	"..\OOInc\oo_class.h"\
	"..\OOInc\oo_class_prv.h"\
	"..\OOInc\oo_pool.h"\
	"..\OOInc\oo_tags.h"\
	"..\OOInc\ooprotos.h"\
	"..\OOInc\ootypes.h"\
	

"$(INTDIR)\Class_Root.obj" : $(SOURCE) $(DEP_CPP_CLASS_) "$(INTDIR)"


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\Init.c
DEP_CPP_INIT_=\
	"..\OOInc\class_root.h"\
	"..\OOInc\oo_class.h"\
	"..\OOInc\oo_class_prv.h"\
	"..\OOInc\oo_gfx.h"\
	"..\OOInc\oo_gui.h"\
	"..\OOInc\oo_init.h"\
	"..\OOInc\oo_pool.h"\
	"..\OOInc\oo_tags.h"\
	"..\OOInc\ooprotos.h"\
	"..\OOInc\ootypes.h"\
	

"$(INTDIR)\Init.obj" : $(SOURCE) $(DEP_CPP_INIT_) "$(INTDIR)"


# End Source File
################################################################################
# Begin Source File

SOURCE=.\Class_Appl.c
DEP_CPP_CLASS_A=\
	"..\OOInc\class_appl.h"\
	"..\OOInc\class_appl_prv.h"\
	"..\OOInc\class_root.h"\
	"..\OOInc\class_sysevt.h"\
	"..\OOInc\oo_class.h"\
	"..\OOInc\oo_class_prv.h"\
	"..\OOInc\oo_tags.h"\
	"..\OOInc\ooprotos.h"\
	"..\OOInc\ootypes.h"\
	

"$(INTDIR)\Class_Appl.obj" : $(SOURCE) $(DEP_CPP_CLASS_A) "$(INTDIR)"


# End Source File
################################################################################
# Begin Source File

SOURCE=.\Class_SysEvt.c
DEP_CPP_CLASS_S=\
	"..\OOInc\class_appl.h"\
	"..\OOInc\class_appl_prv.h"\
	"..\OOInc\class_root.h"\
	"..\OOInc\class_sysevt.h"\
	"..\OOInc\class_sysevt_prv.h"\
	"..\OOInc\oo_class.h"\
	"..\OOInc\oo_class_prv.h"\
	"..\OOInc\oo_tags.h"\
	"..\OOInc\ooprotos.h"\
	"..\OOInc\ootypes.h"\
	

"$(INTDIR)\Class_SysEvt.obj" : $(SOURCE) $(DEP_CPP_CLASS_S) "$(INTDIR)"


# End Source File
################################################################################
# Begin Source File

SOURCE=.\Class_HdlGUI.c

!IF  "$(CFG)" == "OOLib - Win32 Release"

DEP_CPP_CLASS_H=\
	"..\OOInc\class_appl.h"\
	"..\OOInc\class_appl_prv.h"\
	"..\OOInc\class_root.h"\
	"..\OOInc\class_sysevt.h"\
	"..\OOInc\class_sysevt_prv.h"\
	"..\OOInc\oo_class.h"\
	"..\OOInc\oo_class_prv.h"\
	"..\OOInc\oo_gui.h"\
	"..\OOInc\oo_gui_prv.h"\
	"..\OOInc\oo_pool.h"\
	"..\OOInc\oo_tags.h"\
	"..\OOInc\ooprotos.h"\
	"..\OOInc\ootypes.h"\
	

"$(INTDIR)\Class_HdlGUI.obj" : $(SOURCE) $(DEP_CPP_CLASS_H) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "OOLib - Win32 Debug"

DEP_CPP_CLASS_H=\
	"..\OOInc\class_appl.h"\
	"..\OOInc\class_appl_prv.h"\
	"..\OOInc\class_root.h"\
	"..\OOInc\class_sysevt.h"\
	"..\OOInc\class_sysevt_prv.h"\
	"..\OOInc\oo_class.h"\
	"..\OOInc\oo_class_prv.h"\
	"..\OOInc\oo_gui.h"\
	"..\OOInc\oo_gui_prv.h"\
	"..\OOInc\oo_pool.h"\
	"..\OOInc\oo_tags.h"\
	"..\OOInc\ooprotos.h"\
	"..\OOInc\ootypes.h"\
	

"$(INTDIR)\Class_HdlGUI.obj" : $(SOURCE) $(DEP_CPP_CLASS_H) "$(INTDIR)"


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\Gfx.c
DEP_CPP_GFX_C=\
	"..\OOInc\class_root.h"\
	"..\OOInc\oo_class.h"\
	"..\OOInc\oo_gfx.h"\
	"..\OOInc\oo_gui.h"\
	"..\OOInc\oo_gui_prv.h"\
	"..\OOInc\oo_pool.h"\
	"..\OOInc\oo_tags.h"\
	"..\OOInc\ooprotos.h"\
	"..\OOInc\ootypes.h"\
	

"$(INTDIR)\Gfx.obj" : $(SOURCE) $(DEP_CPP_GFX_C) "$(INTDIR)"


# End Source File
################################################################################
# Begin Source File

SOURCE=.\Class_AreaGroup.c

!IF  "$(CFG)" == "OOLib - Win32 Release"

DEP_CPP_CLASS_AR=\
	"..\OOInc\class_root.h"\
	"..\OOInc\oo_buf.h"\
	"..\OOInc\oo_class.h"\
	"..\OOInc\oo_class_prv.h"\
	"..\OOInc\oo_gui.h"\
	"..\OOInc\oo_gui_prv.h"\
	"..\OOInc\oo_pool.h"\
	"..\OOInc\oo_tags.h"\
	"..\OOInc\ooprotos.h"\
	"..\OOInc\ootypes.h"\
	

"$(INTDIR)\Class_AreaGroup.obj" : $(SOURCE) $(DEP_CPP_CLASS_AR) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "OOLib - Win32 Debug"

DEP_CPP_CLASS_AR=\
	"..\OOInc\class_root.h"\
	"..\OOInc\oo_buf.h"\
	"..\OOInc\oo_class.h"\
	"..\OOInc\oo_class_prv.h"\
	"..\OOInc\oo_gui.h"\
	"..\OOInc\oo_gui_prv.h"\
	"..\OOInc\oo_pool.h"\
	"..\OOInc\oo_tags.h"\
	"..\OOInc\ooprotos.h"\
	"..\OOInc\ootypes.h"\
	

"$(INTDIR)\Class_AreaGroup.obj" : $(SOURCE) $(DEP_CPP_CLASS_AR) "$(INTDIR)"


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\Class_AreaScreen_Win32.c

!IF  "$(CFG)" == "OOLib - Win32 Release"

DEP_CPP_CLASS_ARE=\
	"..\OOInc\class_root.h"\
	"..\OOInc\oo_class.h"\
	"..\OOInc\oo_class_prv.h"\
	"..\OOInc\oo_gui.h"\
	"..\OOInc\oo_gui_prv.h"\
	"..\OOInc\oo_tags.h"\
	"..\OOInc\ooprotos.h"\
	"..\OOInc\ootypes.h"\
	

"$(INTDIR)\Class_AreaScreen_Win32.obj" : $(SOURCE) $(DEP_CPP_CLASS_ARE)\
 "$(INTDIR)"


!ELSEIF  "$(CFG)" == "OOLib - Win32 Debug"

DEP_CPP_CLASS_ARE=\
	"..\OOInc\class_root.h"\
	"..\OOInc\oo_class.h"\
	"..\OOInc\oo_class_prv.h"\
	"..\OOInc\oo_gui.h"\
	"..\OOInc\oo_gui_prv.h"\
	"..\OOInc\oo_tags.h"\
	"..\OOInc\ooprotos.h"\
	"..\OOInc\ootypes.h"\
	

"$(INTDIR)\Class_AreaScreen_Win32.obj" : $(SOURCE) $(DEP_CPP_CLASS_ARE)\
 "$(INTDIR)"


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\Class_AreaWindow_Win32.c

!IF  "$(CFG)" == "OOLib - Win32 Release"

DEP_CPP_CLASS_AREA=\
	"..\OOInc\class_root.h"\
	"..\OOInc\class_sysevt.h"\
	"..\OOInc\oo_class.h"\
	"..\OOInc\oo_class_prv.h"\
	"..\OOInc\oo_gui.h"\
	"..\OOInc\oo_gui_prv.h"\
	"..\OOInc\oo_pool.h"\
	"..\OOInc\oo_tags.h"\
	"..\OOInc\ooprotos.h"\
	"..\OOInc\ootypes.h"\
	

"$(INTDIR)\Class_AreaWindow_Win32.obj" : $(SOURCE) $(DEP_CPP_CLASS_AREA)\
 "$(INTDIR)"


!ELSEIF  "$(CFG)" == "OOLib - Win32 Debug"

DEP_CPP_CLASS_AREA=\
	"..\OOInc\class_root.h"\
	"..\OOInc\class_sysevt.h"\
	"..\OOInc\oo_class.h"\
	"..\OOInc\oo_class_prv.h"\
	"..\OOInc\oo_gui.h"\
	"..\OOInc\oo_gui_prv.h"\
	"..\OOInc\oo_pool.h"\
	"..\OOInc\oo_tags.h"\
	"..\OOInc\ooprotos.h"\
	"..\OOInc\ootypes.h"\
	

"$(INTDIR)\Class_AreaWindow_Win32.obj" : $(SOURCE) $(DEP_CPP_CLASS_AREA)\
 "$(INTDIR)"


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\Class_Area.c

!IF  "$(CFG)" == "OOLib - Win32 Release"

DEP_CPP_CLASS_AREA_=\
	"..\OOInc\class_root.h"\
	"..\OOInc\oo_class.h"\
	"..\OOInc\oo_class_prv.h"\
	"..\OOInc\oo_gui.h"\
	"..\OOInc\oo_gui_prv.h"\
	"..\OOInc\oo_pool.h"\
	"..\OOInc\oo_tags.h"\
	"..\OOInc\ooprotos.h"\
	"..\OOInc\ootypes.h"\
	

"$(INTDIR)\Class_Area.obj" : $(SOURCE) $(DEP_CPP_CLASS_AREA_) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "OOLib - Win32 Debug"

DEP_CPP_CLASS_AREA_=\
	"..\OOInc\class_root.h"\
	"..\OOInc\oo_class.h"\
	"..\OOInc\oo_class_prv.h"\
	"..\OOInc\oo_gui.h"\
	"..\OOInc\oo_gui_prv.h"\
	"..\OOInc\oo_pool.h"\
	"..\OOInc\oo_tags.h"\
	"..\OOInc\ooprotos.h"\
	"..\OOInc\ootypes.h"\
	

"$(INTDIR)\Class_Area.obj" : $(SOURCE) $(DEP_CPP_CLASS_AREA_) "$(INTDIR)"


!ENDIF 

# End Source File
# End Target
# End Project
################################################################################
