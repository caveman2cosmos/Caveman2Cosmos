#### Civilization 4 SDK Makefile 2.5 ####
####  Copyright 2015 Nightinggale    ####
#########################################
#### Civilization 4 SDK Makefile 2.0 ####
####  Copyright 2013 Nightinggale    ####
#########################################
#### Civilization 4 SDK Makefile 1.0 ####
####  Copyright 2010 Danny Daemonic  ####
#########################################


## User setup
##
## Any setting here can be overwritten by assigning a new value to the variables in Makefile.settings
##  Makefile.settings is made automatically if it is missing whenever the project is build or cleaned
##
## Not touching the makefile itself can be beneficial if the makefile is committed to svn/git
## Makefile.settings can also accept CUSTOM_CFLAGS and CUSTOM_LDFLAGS if you want to ADD additional flags
##   the custom flags will not overwrite any flags the makefile sets itself
##   An example could be you want to define ENABLE_MY_DEBUG_OUTPUT in C+, then you write
##     CUSTOM_CFLAGS -DENABLE_MY_DEBUG_OUTPUT
##     in Makefile.settings and it will be defined for you without the risk of committing active debug output to other people
##
##     You can write whatever you want after -D and it will work like you write #DEFINE in C++
## 


#### Paths ####
#
# Note: $(PROGRAMFILES) converts to "C:\Program Files", "C:\Program Files (x86)" or whatever fits your system.
# run "nmake.exe /P" in cmd.exe to see what it is on your system
TOOLKIT=..\Build\deps\Microsoft Visual C++ Toolkit 2003
PSDK=..\Build\deps\Microsoft SDKs\Windows\v6.0
## Uncomment to have newly compiled dlls copied to your mod's Assets directory
YOURMOD=..

## Civ install path
## Path to the directory where boost and python is stored
## Overwritten by enviroment variable CIV4_LIB_INSTALL_PATH
# CIV4_PATH=$(PROGRAMFILES)\2K Games\Firaxis Games\Sid Meier's Civilization IV Colonization\CvGameCoreDLL

#### Tools ####
CC="$(TOOLKIT)\bin\cl.exe"
CPP="$(TOOLKIT)\bin\cl.exe"
LD="$(TOOLKIT)\bin\link.exe"
RC="$(PSDK)\bin\rc.exe"
## Uncomment to build dependencies using fastdep
## Overwritten by enviroment variable FASTDEP_FULL_PATH
FD="..\Build\deps\fastdep\fastdep.exe"

#### DEFINES ####
## copy the following lines into "Additional options" in NMAKE properties
## This will make VC++ aware of what is defined and what isn't, which affects the help popup and red lines
## Use Release_DEFINES for all targets except debug
## Assert adds /DFASSERT_ENABLE, which should be added to additional options too, if you care.
## However you will only be able to tell a difference if you do something with asserts other than FAssert() or FAssertMsg()

Debug_DEFINES=/DWIN32 /D_WINDOWS /D_DEBUG /DBUILD_TARGET=$(TARGET)
Release_DEFINES=/DWIN32 /D_WINDOWS /DNDEBUG /DBUILD_TARGET=$(TARGET)

#### You shouldn't need to modify anything beyond this point ####
#################################################################

CUSTOM_CFLAGS =
CUSTOM_LDFLAGS =

# !IF [IF NOT EXIST Makefile.settings EXIT 1] != 0
# !IF [ECHO CUSTOM_CFLAGS = > Makefile.settings]
# !ENDIF
# !ENDIF
# !include Makefile.settings

!IFDEF C2C_VERSION
VERSION_DEFINE=/DC2C_VERSION="\"$(C2C_VERSION)\""
!ELSE
VERSION_DEFINE=
!ENDIF

# /nologo /MD /Z7 /O2 /Oy /Oi /G7 /DWIN32 /D_WINDOWS /DCVGAMECOREDLL_EXPORTS /DNDEBUG /DFINAL_RELEASE /GR /Gy /W3 /EHsc /Gd /Gm- /Fd"TARGET_DIR_files\Profile\CvGameCoreDLL.pdb"   /Z7 /Yu"CvGameCoreDLL.h" /Fp"temp_files\Profile\CvGameCoreDLL.pch" /I"deps/Boost-1.32.0/include" /I"deps/Python24/include" /I"./include" /I"deps\Microsoft Visual C++ Toolkit 2003/include" /I"deps\Microsoft SDKs\Windows\v6.0/Include" /I"deps\Microsoft SDKs\Windows\v6.0/Include/mfc" /YcCvGameCoreDLL.h /Fo"temp_files\Profile\_precompile.obj" /c .\_precompile.cpp
PRECOMPILE_DEPENDENCIES = 
PROJECT_CFLAGS = /arch:SSE2 /Zm200 /DXERCES_STATIC_LIBRARY /D_MOD_SENTRY /D_MOD_FRACTRADE /D_MOD_GOVWORKERS /DLOG_AI $(VERSION_DEFINE)
PROJECT_LDFLAGS = /NODEFAULTLIB:msvcprtd.lib /DEF:CvGameCoreDLL.def /PDBALTPATH:%_PDB%
SOURCE_DIR = .


SUBDIRPATH1 = ../
SUBDIRPATH2 = ../
SUBDIRPATH3 = ../
SUBDIRPATH4 = ../
SUBDIRPATH5 = ../
SUBDIRPATH6 = ../
SUBDIRPATH7 = ../
SUBDIRPATH8 = ../
SUBDIRPATH9 = ../
SUBDIRPATH10 = ../
SUBDIRPATH11 = ../
SUBDIRPATH12 = ../
SUBDIRPATH13 = ../
SUBDIRPATH14 = ../
SUBDIRPATH15 = ../

# !IF [IF NOT EXIST Makefile.project EXIT 1] != 0
# !IF [ECHO SOURCE_DIR = .>> Makefile.project]
# !ENDIF
# !IF [ECHO PROJECT_CFLAGS = >> Makefile.project]
# !ENDIF
# !IF [ECHO PROJECT_LDFLAGS = >> Makefile.project]
# !ENDIF
# !IF [ECHO PRECOMPILE_DEPENDENCIES = >> Makefile.project]
# !ENDIF
# !ENDIF
# !include Makefile.project

SUBDIRS = . $(SUBDIR1) $(SUBDIR2) $(SUBDIR3) $(SUBDIR4) $(SUBDIR5) $(SUBDIR6) $(SUBDIR7) $(SUBDIR8) $(SUBDIR9) $(SUBDIR10) $(SUBDIR11) $(SUBDIR12) $(SUBDIR13) $(SUBDIR14) $(SUBDIR15)

BUILD_DIR =..\Build
TARGET_DIR = $(BUILD_DIR)\$(TARGET)

## Environmental path overwrite
!IFDEF CIV4_LIB_INSTALL_PATH
CIV4_PATH=$(CIV4_LIB_INSTALL_PATH)
!ENDIF

!IFDEF FASTDEP_FULL_PATH
FD=$(FASTDEP_FULL_PATH)
!ENDIF

#### Target Files ####
Target_BIN=$(TARGET_DIR)\CvGameCoreDLL.dll

!IF [IF NOT EXIST CvGameCoreDLL.rc EXIT 1] == 0
Target_RESOURCE=$(TARGET_DIR)\CvGameCoreDLL.res
!ENDIF

Target_STATICLIB=$(TARGET_DIR)\CvGameCoreDLL.lib

Target_LIBDEF=$(TARGET_DIR)\CvGameCoreDLL.def

Target_PCH=$(TARGET_DIR)\CvGameCoreDLL.pch

Target_PDB=$(TARGET_DIR)\CvGameCoreDLL.pdb

Target_OTHER=$(TARGET_DIR)\CvGameCoreDLL.exp

!IFDEF IGNORE_CUSTOM
CUSTOM_CFLAGS  =
CUSTOM_LDFLAGS =
!ENDIF

FILE_DEPENDS = $(TARGET_DIR)\depends

#### CFLAGS ####
GLOBAL_CFLAGS=/GR /Gy /W3 /EHsc /Gd /Gm- /Fd"$(Target_PDB)" $(PROJECT_CFLAGS)
Debug_CFLAGS=/MD /Z7 /Od /RTC1 $(Debug_DEFINES) $(GLOBAL_CFLAGS)
Release_CFLAGS=/MD /Z7 /O2 /Oy /Oi /G7 $(Release_DEFINES) $(GLOBAL_CFLAGS)
FinalRelease_CFLAGS=/GL $(Release_CFLAGS)

PRECOMPILE_CFLAGS1 = /Yu"
PRECOMPILE_CFLAGS2 = CvGameCoreDLL.h" /Fp"$(Target_PCH)"

#### LDFLAGS ####
GLOBAL_LDFLAGS=/LTCG /DLL /NOLOGO /SUBSYSTEM:WINDOWS /LARGEADDRESSAWARE /TLBID:1 /PDB:"$(Target_PDB)" $(CUSTOM_LDFLAGS) $(PROJECT_LDFLAGS)
Debug_LDFLAGS=/INCREMENTAL:NO /DEBUG /IMPLIB:"$(Target_STATICLIB)" $(GLOBAL_LDFLAGS)
Release_LDFLAGS=/INCREMENTAL:NO /DEBUG /OPT:REF /OPT:ICF $(GLOBAL_LDFLAGS)
FinalRelease_LDFLAGS= $(Release_LDFLAGS)

#### INCLUDES ####
GLOBAL_INCS=/I"$(TOOLKIT)/include" /I"$(PSDK)/Include" /I"$(PSDK)/Include/mfc"

#### boost and python ####

BOOST_LIB_PATH = ..\\Build\deps/
!IF [IF NOT EXIST "$(BOOST_LIB_PATH)Boost-1.32.0\libs\boost_python-vc71-mt-gd-1_32.lib" EXIT 1] != 0
BOOST_LIB_PATH = $(SOURCE_DIR)
!IF [IF NOT EXIST "$(BOOST_LIB_PATH)Boost-1.32.0\libs\boost_python-vc71-mt-gd-1_32.lib" EXIT 1] != 0
BOOST_LIB_PATH=$(CIV4_PATH)/
!IF [IF NOT EXIST "$(BOOST_LIB_PATH)Boost-1.32.0\libs\boost_python-vc71-mt-gd-1_32.lib" EXIT 1] != 0
BOOST_LIB_PATH=$(CIV4_LIB_INSTALL_PATH)/
!IF [IF NOT EXIST "$(BOOST_LIB_PATH)Boost-1.32.0\libs\boost_python-vc71-mt-gd-1_32.lib" EXIT 1] != 0
!MESSAGE FATAL ERROR: Failed to locate boost and python
!ENDIF
!ENDIF
!ENDIF
!ENDIF

PROJECT_INCS=/I"$(BOOST_LIB_PATH)Boost-1.32.0/include" /I"$(BOOST_LIB_PATH)Python24/include" /I"./include"
PROJECT_LIBS=/LIBPATH:"$(BOOST_LIB_PATH)Python24/libs" /LIBPATH:"$(BOOST_LIB_PATH)boost-1.32.0/libs/" /LIBPATH:"./lib"  boost_python-vc71-mt-1_32.lib XercesLib-3.1.1-x86-windows-vc-7.1.lib

INCS = $(PROJECT_INCS) $(GLOBAL_INCS)

#### LIBS ####
GLOBAL_LIBS=/LIBPATH:"$(TOOLKIT)/lib" /LIBPATH:"$(PSDK)/Lib" winmm.lib user32.lib psapi.lib advapi32.lib
LIBS = $(PROJECT_LIBS) $(GLOBAL_LIBS)

!IF "$(TARGET)" == "Debug"
CFLAGS  = $(Debug_CFLAGS) /DFASSERT_ENABLE
LDFLAGS = $(Debug_LDFLAGS)
LIBS    = $(LIBS) msvcprt.lib
!ENDIF
!IF "$(TARGET)" == "Release"
CFLAGS  = $(Release_CFLAGS)
LDFLAGS = $(Release_LDFLAGS)
!ENDIF
!IF "$(TARGET)" == "FinalRelease"
CFLAGS  = $(FinalRelease_CFLAGS) /DFINAL_RELEASE
LDFLAGS = $(FinalRelease_LDFLAGS)
!ENDIF
!IF "$(TARGET)" == "Assert"
CFLAGS  = $(Release_CFLAGS) /DFASSERT_ENABLE
LDFLAGS = $(Release_LDFLAGS)
!ENDIF
!IF "$(TARGET)" == "Testing"
CFLAGS  = $(Release_CFLAGS) /DFASSERT_ENABLE /DFASSERT_LOGGING
LDFLAGS = $(Release_LDFLAGS)
!ENDIF
!IF "$(TARGET)" == "Profile"
CFLAGS  = $(Release_CFLAGS) /DFP_PROFILE_ENABLE /DUSE_INTERNAL_PROFILER
LDFLAGS = $(Release_LDFLAGS)
!ENDIF

!IFNDEF CFLAGS
!ERROR Target "$(TARGET)" not supported. Supported targets: Debug Release Assert Profile Testing
!ENDIF

!IF EXIST ($(FILE_DEPENDS))
!INCLUDE $(FILE_DEPENDS)
!ENDIF

#### Targets ####
#################

.PHONY: all clean build precompile Make_Dir fastdep source_list

all: Precompile build

dll: all

clean:
	@IF EXIST "version.cpp" DEL "version.cpp"
	@IF EXIST "$(TARGET_DIR)" RMDIR /s /q "$(TARGET_DIR)"
	@FOR %i IN ($(Target_BIN) $(Target_STATICLIB) $(Target_LIBDEF) $(Target_RESOURCE) \
		$(Target_PCH) $(Target_PDB) $(Target_OTHER)) DO @IF EXIST "%i" DEL "%i"

build: $(Target_BIN)

stage: $(Target_BIN)
!IFDEF YOURMOD
	-COPY "$(Target_BIN)" "$(YOURMOD)\Assets\."
	-COPY "$(Target_PDB)" "$(YOURMOD)\Assets\."
!ENDIF

precompile: Make_Dir Target_unfinished $(Target_PCH)

source_list: Make_Dir
	@ECHO Building source list
	@ECHO SOURCES= \> $(FILE_DEPENDS)
	@FOR %j in ($(SUBDIRS)) DO @(FOR %i IN ($(SOURCE_DIR)\%j\*.cpp) DO @ECHO %i \>> $(FILE_DEPENDS))
	@ECHO.>> $(FILE_DEPENDS)
	@ECHO OBJS= \>> $(FILE_DEPENDS)
	@FOR %j in ($(SUBDIRS)) DO @(FOR /F "delims=." %i IN ('dir /b $(SOURCE_DIR)\%j\*.cpp') DO @ECHO. $(TARGET_DIR)\%j\%i.obj \>> $(FILE_DEPENDS))
	@ECHO.>> $(FILE_DEPENDS)

fastdep:
	@ECHO Running fastdep
	@$(FD) --objectextension=pch -q --removepath=$(SOURCE_DIR)\ -O $(TARGET_DIR) $(SOURCE_DIR)\CvGameCoreDLL.cpp >> $(FILE_DEPENDS)
	@$(FD) --objectextension=obj -q --removepath=$(SOURCE_DIR)\ -O $(TARGET_DIR) $(SOURCES) >> $(FILE_DEPENDS)

Make_Dir:
	@FOR %j in ($(SUBDIRS)) DO @(IF NOT EXIST "$(TARGET_DIR)\%j\." MKDIR "$(TARGET_DIR)\%j")
	@IF NOT EXIST "$(TARGET_DIR)\." MKDIR $(TARGET_DIR)

Target_unfinished:
	@ECHO.>$(TARGET_DIR)\unfinished.@
	@FOR /F "delims=@" %i IN ('dir /b $(TARGET_DIR)\*.@') DO \
		@IF EXIST "$(TARGET_DIR)\%i" DEL "$(TARGET_DIR)\%i"
	@FOR /F %i IN ('dir /b $(TARGET_DIR)\*.@') DO \
		@IF EXIST "$(TARGET_DIR)\%i" DEL "$(TARGET_DIR)\%i"

$(Target_BIN): $(OBJS) $(Target_RESOURCE)
	@ECHO.Linking DLL
	$(LD) /out:$(Target_BIN) $(LDFLAGS) $(LIBS) $(OBJS) $(Target_RESOURCE)

{$(SOURCE_DIR)\.}.cpp{$(TARGET_DIR)\.}.obj:
	@ECHO.>"$*.obj.@"
	$(CPP) /nologo $(CFLAGS) $(PRECOMPILE_CFLAGS1)$(PRECOMPILE_CFLAGS2) $(INCS) /Fo$*.obj /c $<
	@DEL "$*.obj.@"
	
{$(SOURCE_DIR)\$(SUBDIR1)}.cpp{$(TARGET_DIR)\$(SUBDIR1)}.obj:
	@ECHO.>"$*.obj.@"
	$(CPP) /nologo $(CFLAGS) $(PRECOMPILE_CFLAGS1)$(SUBDIRPATH1)$(PRECOMPILE_CFLAGS2) $(INCS) /Fo$*.obj /c $<
	@DEL "$*.obj.@"

{$(SOURCE_DIR)\$(SUBDIR2)}.cpp{$(TARGET_DIR)\$(SUBDIR2)}.obj:
	@ECHO.>"$*.obj.@"
	$(CPP) /nologo $(CFLAGS) $(PRECOMPILE_CFLAGS1)$(SUBDIRPATH2)$(PRECOMPILE_CFLAGS2) $(INCS) /Fo$*.obj /c $<
	@DEL "$*.obj.@"

{$(SOURCE_DIR)\$(SUBDIR3)}.cpp{$(TARGET_DIR)\$(SUBDIR3)}.obj:
	@ECHO.>"$*.obj.@"
	$(CPP) /nologo $(CFLAGS) $(PRECOMPILE_CFLAGS1)$(SUBDIRPATH3)$(PRECOMPILE_CFLAGS2) $(INCS) /Fo$*.obj /c $<
	@DEL "$*.obj.@"

{$(SOURCE_DIR)\$(SUBDIR4)}.cpp{$(TARGET_DIR)\$(SUBDIR4)}.obj:
	@ECHO.>"$*.obj.@"
	$(CPP) /nologo $(CFLAGS) $(PRECOMPILE_CFLAGS1)$(SUBDIRPATH4)$(PRECOMPILE_CFLAGS2) $(INCS) /Fo$*.obj /c $<
	@DEL "$*.obj.@"
	
{$(SOURCE_DIR)\$(SUBDIR5)}.cpp{$(TARGET_DIR)\$(SUBDIR5)}.obj:
	@ECHO.>"$*.obj.@"
	$(CPP) /nologo $(CFLAGS) $(PRECOMPILE_CFLAGS1)$(SUBDIRPATH5)$(PRECOMPILE_CFLAGS2) $(INCS) /Fo$*.obj /c $<
	@DEL "$*.obj.@"

{$(SOURCE_DIR)\$(SUBDIR6)}.cpp{$(TARGET_DIR)\$(SUBDIR6)}.obj:
	@ECHO.>"$*.obj.@"
	$(CPP) /nologo $(CFLAGS) $(PRECOMPILE_CFLAGS1)$(SUBDIRPATH6)$(PRECOMPILE_CFLAGS2) $(INCS) /Fo$*.obj /c $<
	@DEL "$*.obj.@"

{$(SOURCE_DIR)\$(SUBDIR7)}.cpp{$(TARGET_DIR)\$(SUBDIR7)}.obj:
	@ECHO.>"$*.obj.@"
	$(CPP) /nologo $(CFLAGS) $(PRECOMPILE_CFLAGS1)$(SUBDIRPATH7)$(PRECOMPILE_CFLAGS2) $(INCS) /Fo$*.obj /c $<
	@DEL "$*.obj.@"

{$(SOURCE_DIR)\$(SUBDIR8)}.cpp{$(TARGET_DIR)\$(SUBDIR8)}.obj:
	@ECHO.>"$*.obj.@"
	$(CPP) /nologo $(CFLAGS) $(PRECOMPILE_CFLAGS1)$(SUBDIRPATH8)$(PRECOMPILE_CFLAGS2) $(INCS) /Fo$*.obj /c $<
	@DEL "$*.obj.@"
	
{$(SOURCE_DIR)\$(SUBDIR9)}.cpp{$(TARGET_DIR)\$(SUBDIR9)}.obj:
	@ECHO.>"$*.obj.@"
	$(CPP) /nologo $(CFLAGS) $(PRECOMPILE_CFLAGS1)$(SUBDIRPATH9)$(PRECOMPILE_CFLAGS2) $(INCS) /Fo$*.obj /c $<
	@DEL "$*.obj.@"

{$(SOURCE_DIR)\$(SUBDIR10)}.cpp{$(TARGET_DIR)\$(SUBDIR10)}.obj:
	@ECHO.>"$*.obj.@"
	$(CPP) /nologo $(CFLAGS) $(PRECOMPILE_CFLAGS1)$(SUBDIRPATH10)$(PRECOMPILE_CFLAGS2) $(INCS) /Fo$*.obj /c $<
	@DEL "$*.obj.@"

{$(SOURCE_DIR)\$(SUBDIR11)}.cpp{$(TARGET_DIR)\$(SUBDIR11)}.obj:
	@ECHO.>"$*.obj.@"
	$(CPP) /nologo $(CFLAGS) $(PRECOMPILE_CFLAGS1)$(SUBDIRPATH11)$(PRECOMPILE_CFLAGS2) $(INCS) /Fo$*.obj /c $<
	@DEL "$*.obj.@"

{$(SOURCE_DIR)\$(SUBDIR12)}.cpp{$(TARGET_DIR)\$(SUBDIR12)}.obj:
	@ECHO.>"$*.obj.@"
	$(CPP) /nologo $(CFLAGS) $(PRECOMPILE_CFLAGS1)$(SUBDIRPATH12)$(PRECOMPILE_CFLAGS2) $(INCS) /Fo$*.obj /c $<
	@DEL "$*.obj.@"

{$(SOURCE_DIR)\$(SUBDIR13)}.cpp{$(TARGET_DIR)\$(SUBDIR13)}.obj:
	@ECHO.>"$*.obj.@"
	$(CPP) /nologo $(CFLAGS) $(PRECOMPILE_CFLAGS1)$(SUBDIRPATH13)$(PRECOMPILE_CFLAGS2) $(INCS) /Fo$*.obj /c $<
	@DEL "$*.obj.@"

{$(SOURCE_DIR)\$(SUBDIR14)}.cpp{$(TARGET_DIR)\$(SUBDIR14)}.obj:
	@ECHO.>"$*.obj.@"
	$(CPP) /nologo $(CFLAGS) $(PRECOMPILE_CFLAGS1)$(SUBDIRPATH14)$(PRECOMPILE_CFLAGS2) $(INCS) /Fo$*.obj /c $<
	@DEL "$*.obj.@"

{$(SOURCE_DIR)\$(SUBDIR15)}.cpp{$(TARGET_DIR)\$(SUBDIR15)}.obj:
	@ECHO.>"$*.obj.@"
	$(CPP) /nologo $(CFLAGS) $(PRECOMPILE_CFLAGS1)$(SUBDIRPATH15)$(PRECOMPILE_CFLAGS2) $(INCS) /Fo$*.obj /c $<
	@DEL "$*.obj.@"	

$(Target_PCH) $(TARGET_DIR)\_precompile.obj: $(PRECOMPILE_DEPENDENCIES)
	@ECHO.>"$(Target_PCH).@"
	@ECHO.>"$(TARGET_DIR)\_precompile.obj.@"
	@FOR %i IN ($(TARGET_DIR)\*.obj) DO @IF EXIST "%i" DEL "%i"
	$(CPP) /nologo $(CFLAGS) $(PRECOMPILE_CFLAGS1)$(PRECOMPILE_CFLAGS2) $(INCS) /YcCvGameCoreDLL.h /Fo"$(TARGET_DIR)\_precompile.obj" /c $(SOURCE_DIR)\_precompile.cpp
	@DEL "$(Target_PCH).@"
	@DEL "$(TARGET_DIR)\_precompile.obj.@"

.rc{$(TARGET_DIR)}.res:
	@ECHO.>"$*.res.@"
	$(RC) /Fo$@ $(INCS) $<
	@DEL "$*.res.@"
