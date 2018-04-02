
DEBUG=0
PDB=0
YORI_BUILD_ID=0
ARCH=win32

!IF [$(CC) -? 2>&1 | findstr /C:"for x64" >NUL 2>&1]==0
ARCH=amd64
!ELSE
!IF [$(CC) -? 2>&1 | findstr /C:"for AMD64" >NUL 2>&1]==0
ARCH=amd64
!ELSE
!IF [$(CC) -? 2>&1 | findstr /C:"for ARM64" >NUL 2>&1]==0
ARCH=arm64
!ENDIF
!ENDIF
!ENDIF

BINDIR=bin\$(ARCH)
SYMDIR=sym\$(ARCH)
MODDIR=bin\$(ARCH)\modules
BUILD=$(MAKE) -nologo DEBUG=$(DEBUG) PDB=$(PDB) YORI_BUILD_ID=$(YORI_BUILD_ID) BINDIR=..\$(BINDIR) SYMDIR=..\$(SYMDIR) MODDIR=..\$(MODDIR)

FOR=for
MKDIR=mkdir
RMDIR=rmdir
STARTCMD=
CURRENTTIME=REM
WRITECONFIGCACHEFILE=cache.mk

all: all.real

!INCLUDE "config\common.mk"

!IF [yfor.exe -? >NUL 2>&1]==0
FOR=yfor -c -p %NUMBER_OF_PROCESSORS%
STARTCMD="
!ENDIF
!IF [ydate.exe -? >NUL 2>&1]==0
CURRENTTIME=echo. & echo For: $(FOR) & ydate $$HOUR$$:$$MIN$$:$$SEC$$ & echo.
!ENDIF
!IF [ymkdir.exe -? >NUL 2>&1]==0
MKDIR=ymkdir
!ENDIF
!IF [yrmdir.exe -? >NUL 2>&1]==0
RMDIR=yrmdir
!ENDIF


DIRS=crt       \
     lib       \
     builtins  \
     sh        \
     clip      \
     clmp      \
     cls       \
     copy      \
     cshot     \
     cut       \
     cvtvt     \
     date      \
     dir       \
     echo      \
     erase     \
     expr      \
     for       \
     fscmp     \
     get       \
     grpcmp    \
     help      \
     hilite    \
     iconv     \
     intcmp    \
     lines     \
     mkdir     \
     mklink    \
     move      \
     nice      \
     osver     \
     path      \
     pause     \
     readline  \
     rmdir     \
     scut      \
     sdir      \
     sleep     \
     split     \
     start     \
     strcmp    \
     tail      \
     tee       \
     title     \
     type      \
     vol       \
     which     \

all.real: writeconfigcache
	@$(CURRENTTIME)
	@$(FOR) %%i in ($(BINDIR) $(SYMDIR) $(MODDIR)) do $(STARTCMD)@if not exist %%i $(MKDIR) %%i$(STARTCMD)
	@$(FOR) %%i in ($(DIRS)) do $(STARTCMD)@if exist %%i echo *** Compiling %%i & cd %%i & $(BUILD) compile READCONFIGCACHEFILE=..\$(WRITECONFIGCACHEFILE) & cd ..$(STARTCMD)
	@$(FOR) %%i in ($(DIRS)) do $(STARTCMD)@if exist %%i echo *** Linking %%i & cd %%i & $(BUILD) link READCONFIGCACHEFILE=..\$(WRITECONFIGCACHEFILE) & cd ..$(STARTCMD)
	@$(FOR) %%i in ($(DIRS)) do $(STARTCMD)@if exist %%i echo *** Installing %%i & cd %%i & $(BUILD) install READCONFIGCACHEFILE=..\$(WRITECONFIGCACHEFILE) & cd ..$(STARTCMD)
	@$(CURRENTTIME)

beta: all.real
	@if not exist beta $(MKDIR) beta
	@move $(BINDIR) beta\$(ARCH)
	@move $(SYMDIR) beta\$(ARCH)\sym

clean:
	@$(FOR) %%i in ($(DIRS)) do $(STARTCMD)@if exist %%i echo *** Cleaning %%i & cd %%i & $(BUILD) PROBECOMPILER=0 PROBELINKER=0 clean & cd ..$(STARTCMD)
	@if exist *~ erase *~
	@for /D %%i in ($(MODDIR) $(BINDIR) $(SYMDIR)) do @if exist %%i $(RMDIR) /s/q %%i
	@if exist $(WRITECONFIGCACHEFILE) erase $(WRITECONFIGCACHEFILE)

distclean: clean
	@for %%i in (beta out doc bin sym) do @if exist %%i $(RMDIR) /s/q %%i

help:
	@echo "DEBUG=[0|1] - If set, will compile debug build without optimization and with instrumentation"
	@echo "PDB=[0|1] - If set, will generate debug symbols"
