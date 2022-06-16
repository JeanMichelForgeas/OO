# Microsoft Developer Studio Generated NMAKE File, Format Version 4.20
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Application" 0x0101

!IF "$(CFG)" == ""
CFG=Test_3D - Win32 Debug
!MESSAGE No configuration specified.  Defaulting to Test_3D - Win32 Debug.
!ENDIF 

!IF "$(CFG)" != "Test_3D - Win32 Release" && "$(CFG)" !=\
 "Test_3D - Win32 Debug"
!MESSAGE Invalid configuration "$(CFG)" specified.
!MESSAGE You can specify a configuration when running NMAKE on this makefile
!MESSAGE by defining the macro CFG on the command line.  For example:
!MESSAGE 
!MESSAGE NMAKE /f "Test_3D.mak" CFG="Test_3D - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "Test_3D - Win32 Release" (based on "Win32 (x86) Application")
!MESSAGE "Test_3D - Win32 Debug" (based on "Win32 (x86) Application")
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
# PROP Target_Last_Scanned "Test_3D - Win32 Debug"
MTL=mktyplib.exe
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "Test_3D - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Test_3D_"
# PROP BASE Intermediate_Dir "Test_3D_"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "Release"
# PROP Target_Dir ""
OUTDIR=.\Release
INTDIR=.\Release

ALL : "$(OUTDIR)\Test_3D.exe"

CLEAN : 
	-@erase "$(INTDIR)\Test_3d.obj"
	-@erase "$(OUTDIR)\Test_3D.exe"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /YX /c
# ADD CPP /nologo /G5 /Zp4 /W3 /Gi /O2 /I "..\OOInc" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /YX /c
CPP_PROJ=/nologo /G5 /Zp4 /ML /W3 /Gi /O2 /I "..\OOInc" /D "WIN32" /D "NDEBUG"\
 /D "_WINDOWS" /Fp"$(INTDIR)/Test_3D.pch" /YX /Fo"$(INTDIR)/" /Fd"$(INTDIR)/" /c\
 
CPP_OBJS=.\Release/
CPP_SBRS=.\.
# ADD BASE MTL /nologo /D "NDEBUG" /win32
# ADD MTL /nologo /D "NDEBUG" /win32
MTL_PROJ=/nologo /D "NDEBUG" /win32 
# ADD BASE RSC /l 0x40c /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o"$(OUTDIR)/Test_3D.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /machine:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib winmm.lib ..\OOLib\Release\oo.lib ..\OO3D\Release\oo3d.lib ..\OOSnd\Release\oosnd.lib /nologo /subsystem:windows /machine:I386
# SUBTRACT LINK32 /debug
LINK32_FLAGS=kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib\
 advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib\
 odbccp32.lib winmm.lib ..\OOLib\Release\oo.lib ..\OO3D\Release\oo3d.lib\
 ..\OOSnd\Release\oosnd.lib /nologo /subsystem:windows /incremental:no\
 /pdb:"$(OUTDIR)/Test_3D.pdb" /machine:I386 /out:"$(OUTDIR)/Test_3D.exe" 
LINK32_OBJS= \
	"$(INTDIR)\Test_3d.obj" \
	"..\OO3d\Debug\OO3D.lib" \
	"..\OOLib\Debug\oo.lib" \
	"..\OOSnd\Debug\OOSnd.lib"

"$(OUTDIR)\Test_3D.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "Test_3D - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Test_3D0"
# PROP BASE Intermediate_Dir "Test_3D0"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug"
# PROP Target_Dir ""
OUTDIR=.\Debug
INTDIR=.\Debug

ALL : "$(OUTDIR)\Test_3D.exe" "$(OUTDIR)\Test_3D.bsc"

CLEAN : 
	-@erase "$(INTDIR)\Test_3d.obj"
	-@erase "$(INTDIR)\Test_3d.sbr"
	-@erase "$(INTDIR)\vc40.pdb"
	-@erase "$(OUTDIR)\Test_3D.bsc"
	-@erase "$(OUTDIR)\Test_3D.exe"
	-@erase "$(OUTDIR)\Test_3D.pdb"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

# ADD BASE CPP /nologo /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /YX /c
# ADD CPP /nologo /G5 /Zp4 /W3 /Gi /Zi /Od /I "..\OOInc" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /Fr /YX /c
CPP_PROJ=/nologo /G5 /Zp4 /MLd /W3 /Gi /Zi /Od /I "..\OOInc" /D "WIN32" /D\
 "_DEBUG" /D "_WINDOWS" /Fr"$(INTDIR)/" /Fp"$(INTDIR)/Test_3D.pch" /YX\
 /Fo"$(INTDIR)/" /Fd"$(INTDIR)/" /c 
CPP_OBJS=.\Debug/
CPP_SBRS=.\Debug/
# ADD BASE MTL /nologo /D "_DEBUG" /win32
# ADD MTL /nologo /D "_DEBUG" /win32
MTL_PROJ=/nologo /D "_DEBUG" /win32 
# ADD BASE RSC /l 0x40c /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o"$(OUTDIR)/Test_3D.bsc" 
BSC32_SBRS= \
	"$(INTDIR)\Test_3d.sbr"

"$(OUTDIR)\Test_3D.bsc" : "$(OUTDIR)" $(BSC32_SBRS)
    $(BSC32) @<<
  $(BSC32_FLAGS) $(BSC32_SBRS)
<<

LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /debug /machine:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib winmm.lib ..\OOLib\Debug\oo.lib ..\OO3D\Debug\oo3d.lib ..\OOSnd\Debug\oosnd.lib /nologo /subsystem:windows /incremental:no /debug /machine:I386
LINK32_FLAGS=kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib\
 advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib\
 odbccp32.lib winmm.lib ..\OOLib\Debug\oo.lib ..\OO3D\Debug\oo3d.lib\
 ..\OOSnd\Debug\oosnd.lib /nologo /subsystem:windows /incremental:no\
 /pdb:"$(OUTDIR)/Test_3D.pdb" /debug /machine:I386 /out:"$(OUTDIR)/Test_3D.exe" 
LINK32_OBJS= \
	"$(INTDIR)\Test_3d.obj" \
	"..\OO3d\Debug\OO3D.lib" \
	"..\OOLib\Debug\oo.lib" \
	"..\OOSnd\Debug\OOSnd.lib"

"$(OUTDIR)\Test_3D.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
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

# Name "Test_3D - Win32 Release"
# Name "Test_3D - Win32 Debug"

!IF  "$(CFG)" == "Test_3D - Win32 Release"

!ELSEIF  "$(CFG)" == "Test_3D - Win32 Debug"

!ENDIF 

################################################################################
# Begin Source File

SOURCE=.\Test_3d.c
DEP_CPP_TEST_=\
	"..\OOInc\class_appl.h"\
	"..\OOInc\class_root.h"\
	"..\OOInc\class_sysevt.h"\
	"..\OOInc\oo_3d.h"\
	"..\OOInc\oo_class.h"\
	"..\OOInc\oo_gui.h"\
	"..\OOInc\oo_init.h"\
	"..\OOInc\oo_sound.h"\
	"..\OOInc\oo_tags.h"\
	"..\OOInc\ooprotos.h"\
	"..\OOInc\ootypes.h"\
	".\Data_Obj_Defs.c"\
	".\Data_Obj_Tags.c"\
	

!IF  "$(CFG)" == "Test_3D - Win32 Release"


"$(INTDIR)\Test_3d.obj" : $(SOURCE) $(DEP_CPP_TEST_) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Test_3D - Win32 Debug"


"$(INTDIR)\Test_3d.obj" : $(SOURCE) $(DEP_CPP_TEST_) "$(INTDIR)"

"$(INTDIR)\Test_3d.sbr" : $(SOURCE) $(DEP_CPP_TEST_) "$(INTDIR)"


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=\Oo\OOLib\Debug\oo.lib

!IF  "$(CFG)" == "Test_3D - Win32 Release"

!ELSEIF  "$(CFG)" == "Test_3D - Win32 Debug"

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=\Oo\OO3d\Debug\OO3D.lib

!IF  "$(CFG)" == "Test_3D - Win32 Release"

!ELSEIF  "$(CFG)" == "Test_3D - Win32 Debug"

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=\Oo\OOSnd\Debug\OOSnd.lib

!IF  "$(CFG)" == "Test_3D - Win32 Release"

!ELSEIF  "$(CFG)" == "Test_3D - Win32 Debug"

!ENDIF 

# End Source File
# End Target
# End Project
################################################################################
