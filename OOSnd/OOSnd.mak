# Microsoft Developer Studio Generated NMAKE File, Format Version 4.20
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Static Library" 0x0104

!IF "$(CFG)" == ""
CFG=OOSnd - Win32 Debug
!MESSAGE No configuration specified.  Defaulting to OOSnd - Win32 Debug.
!ENDIF 

!IF "$(CFG)" != "OOSnd - Win32 Release" && "$(CFG)" != "OOSnd - Win32 Debug"
!MESSAGE Invalid configuration "$(CFG)" specified.
!MESSAGE You can specify a configuration when running NMAKE on this makefile
!MESSAGE by defining the macro CFG on the command line.  For example:
!MESSAGE 
!MESSAGE NMAKE /f "OOSnd.mak" CFG="OOSnd - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "OOSnd - Win32 Release" (based on "Win32 (x86) Static Library")
!MESSAGE "OOSnd - Win32 Debug" (based on "Win32 (x86) Static Library")
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
# PROP Target_Last_Scanned "OOSnd - Win32 Debug"
CPP=cl.exe

!IF  "$(CFG)" == "OOSnd - Win32 Release"

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

ALL : "$(OUTDIR)\OOSnd.lib"

CLEAN : 
	-@erase "$(INTDIR)\Class_HandleSound.obj"
	-@erase "$(INTDIR)\Class_Sound.obj"
	-@erase "$(INTDIR)\Class_Sound_Wave.obj"
	-@erase "$(INTDIR)\Init.obj"
	-@erase "$(OUTDIR)\OOSnd.lib"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /YX /c
# ADD CPP /nologo /G5 /Zp4 /W3 /O2 /I "..\OOInc" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /YX /c
CPP_PROJ=/nologo /G5 /Zp4 /ML /W3 /O2 /I "..\OOInc" /D "WIN32" /D "NDEBUG" /D\
 "_WINDOWS" /Fp"$(INTDIR)/OOSnd.pch" /YX /Fo"$(INTDIR)/" /c 
CPP_OBJS=.\Release/
CPP_SBRS=.\.
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o"$(OUTDIR)/OOSnd.bsc" 
BSC32_SBRS= \
	
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo
LIB32_FLAGS=/nologo /out:"$(OUTDIR)/OOSnd.lib" 
LIB32_OBJS= \
	"$(INTDIR)\Class_HandleSound.obj" \
	"$(INTDIR)\Class_Sound.obj" \
	"$(INTDIR)\Class_Sound_Wave.obj" \
	"$(INTDIR)\Init.obj"

"$(OUTDIR)\OOSnd.lib" : "$(OUTDIR)" $(DEF_FILE) $(LIB32_OBJS)
    $(LIB32) @<<
  $(LIB32_FLAGS) $(DEF_FLAGS) $(LIB32_OBJS)
<<

!ELSEIF  "$(CFG)" == "OOSnd - Win32 Debug"

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

ALL : "$(OUTDIR)\OOSnd.lib" "$(OUTDIR)\OOSnd.bsc"

CLEAN : 
	-@erase "$(INTDIR)\Class_HandleSound.obj"
	-@erase "$(INTDIR)\Class_HandleSound.sbr"
	-@erase "$(INTDIR)\Class_Sound.obj"
	-@erase "$(INTDIR)\Class_Sound.sbr"
	-@erase "$(INTDIR)\Class_Sound_Wave.obj"
	-@erase "$(INTDIR)\Class_Sound_Wave.sbr"
	-@erase "$(INTDIR)\Init.obj"
	-@erase "$(INTDIR)\Init.sbr"
	-@erase "$(INTDIR)\vc40.idb"
	-@erase "$(INTDIR)\vc40.pdb"
	-@erase "$(OUTDIR)\OOSnd.bsc"
	-@erase "$(OUTDIR)\OOSnd.lib"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

# ADD BASE CPP /nologo /W3 /GX /Z7 /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /YX /c
# ADD CPP /nologo /G5 /Zp4 /W3 /Gm /Zi /Od /I "..\OOInc" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /FR /YX /c
CPP_PROJ=/nologo /G5 /Zp4 /MLd /W3 /Gm /Zi /Od /I "..\OOInc" /D "WIN32" /D\
 "_DEBUG" /D "_WINDOWS" /FR"$(INTDIR)/" /Fp"$(INTDIR)/OOSnd.pch" /YX\
 /Fo"$(INTDIR)/" /Fd"$(INTDIR)/" /c 
CPP_OBJS=.\Debug/
CPP_SBRS=.\Debug/
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o"$(OUTDIR)/OOSnd.bsc" 
BSC32_SBRS= \
	"$(INTDIR)\Class_HandleSound.sbr" \
	"$(INTDIR)\Class_Sound.sbr" \
	"$(INTDIR)\Class_Sound_Wave.sbr" \
	"$(INTDIR)\Init.sbr"

"$(OUTDIR)\OOSnd.bsc" : "$(OUTDIR)" $(BSC32_SBRS)
    $(BSC32) @<<
  $(BSC32_FLAGS) $(BSC32_SBRS)
<<

LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo
LIB32_FLAGS=/nologo /out:"$(OUTDIR)/OOSnd.lib" 
LIB32_OBJS= \
	"$(INTDIR)\Class_HandleSound.obj" \
	"$(INTDIR)\Class_Sound.obj" \
	"$(INTDIR)\Class_Sound_Wave.obj" \
	"$(INTDIR)\Init.obj"

"$(OUTDIR)\OOSnd.lib" : "$(OUTDIR)" $(DEF_FILE) $(LIB32_OBJS)
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

# Name "OOSnd - Win32 Release"
# Name "OOSnd - Win32 Debug"

!IF  "$(CFG)" == "OOSnd - Win32 Release"

!ELSEIF  "$(CFG)" == "OOSnd - Win32 Debug"

!ENDIF 

################################################################################
# Begin Source File

SOURCE=.\Class_HandleSound.c
DEP_CPP_CLASS=\
	"..\OOInc\class_appl.h"\
	"..\OOInc\class_root.h"\
	"..\OOInc\class_sysevt.h"\
	"..\OOInc\oo_class.h"\
	"..\OOInc\oo_class_prv.h"\
	"..\OOInc\oo_sound.h"\
	"..\OOInc\oo_sound_prv.h"\
	"..\OOInc\oo_tags.h"\
	"..\OOInc\ooprotos.h"\
	"..\OOInc\ootypes.h"\
	

!IF  "$(CFG)" == "OOSnd - Win32 Release"


"$(INTDIR)\Class_HandleSound.obj" : $(SOURCE) $(DEP_CPP_CLASS) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "OOSnd - Win32 Debug"


"$(INTDIR)\Class_HandleSound.obj" : $(SOURCE) $(DEP_CPP_CLASS) "$(INTDIR)"

"$(INTDIR)\Class_HandleSound.sbr" : $(SOURCE) $(DEP_CPP_CLASS) "$(INTDIR)"


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\Class_Sound_Wave.c
DEP_CPP_CLASS_=\
	"..\OOInc\class_root.h"\
	"..\OOInc\oo_class.h"\
	"..\OOInc\oo_class_prv.h"\
	"..\OOInc\oo_sound.h"\
	"..\OOInc\oo_sound_prv.h"\
	"..\OOInc\oo_tags.h"\
	"..\OOInc\ooprotos.h"\
	"..\OOInc\ootypes.h"\
	

!IF  "$(CFG)" == "OOSnd - Win32 Release"


"$(INTDIR)\Class_Sound_Wave.obj" : $(SOURCE) $(DEP_CPP_CLASS_) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "OOSnd - Win32 Debug"


"$(INTDIR)\Class_Sound_Wave.obj" : $(SOURCE) $(DEP_CPP_CLASS_) "$(INTDIR)"

"$(INTDIR)\Class_Sound_Wave.sbr" : $(SOURCE) $(DEP_CPP_CLASS_) "$(INTDIR)"


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\Class_Sound.c
DEP_CPP_CLASS_S=\
	"..\OOInc\class_root.h"\
	"..\OOInc\oo_class.h"\
	"..\OOInc\oo_class_prv.h"\
	"..\OOInc\oo_sound.h"\
	"..\OOInc\oo_sound_prv.h"\
	"..\OOInc\oo_tags.h"\
	"..\OOInc\ooprotos.h"\
	"..\OOInc\ootypes.h"\
	

!IF  "$(CFG)" == "OOSnd - Win32 Release"


"$(INTDIR)\Class_Sound.obj" : $(SOURCE) $(DEP_CPP_CLASS_S) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "OOSnd - Win32 Debug"


"$(INTDIR)\Class_Sound.obj" : $(SOURCE) $(DEP_CPP_CLASS_S) "$(INTDIR)"

"$(INTDIR)\Class_Sound.sbr" : $(SOURCE) $(DEP_CPP_CLASS_S) "$(INTDIR)"


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\Init.c
DEP_CPP_INIT_=\
	"..\OOInc\class_root.h"\
	"..\OOInc\oo_class.h"\
	"..\OOInc\oo_gui.h"\
	"..\OOInc\oo_gui_prv.h"\
	"..\OOInc\oo_init.h"\
	"..\OOInc\oo_sound.h"\
	"..\OOInc\oo_sound_prv.h"\
	"..\OOInc\oo_tags.h"\
	"..\OOInc\ooprotos.h"\
	"..\OOInc\ootypes.h"\
	

!IF  "$(CFG)" == "OOSnd - Win32 Release"


"$(INTDIR)\Init.obj" : $(SOURCE) $(DEP_CPP_INIT_) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "OOSnd - Win32 Debug"


"$(INTDIR)\Init.obj" : $(SOURCE) $(DEP_CPP_INIT_) "$(INTDIR)"

"$(INTDIR)\Init.sbr" : $(SOURCE) $(DEP_CPP_INIT_) "$(INTDIR)"


!ENDIF 

# End Source File
# End Target
# End Project
################################################################################
