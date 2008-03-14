# Microsoft Developer Studio Generated NMAKE File, Based on Abuse32.dsp
!IF "$(CFG)" == ""
CFG=Abuse32 - Win32 Debug
!MESSAGE No configuration specified. Defaulting to Abuse32 - Win32 Debug.
!ENDIF 

!IF "$(CFG)" != "Abuse32 - Win32 Release" && "$(CFG)" != "Abuse32 - Win32 Debug"
!MESSAGE Invalid configuration "$(CFG)" specified.
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "Abuse32.mak" CFG="Abuse32 - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "Abuse32 - Win32 Release" (based on "Win32 (x86) Application")
!MESSAGE "Abuse32 - Win32 Debug" (based on "Win32 (x86) Application")
!MESSAGE 
!ERROR An invalid configuration is specified.
!ENDIF 

!IF "$(OS)" == "Windows_NT"
NULL=
!ELSE 
NULL=nul
!ENDIF 

CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "Abuse32 - Win32 Release"

OUTDIR=.\Release
INTDIR=.\Release
# Begin Custom Macros
OutDir=.\Release
# End Custom Macros

ALL : "$(OUTDIR)\Abuse32.exe"


CLEAN :
	-@erase "$(INTDIR)\ability.obj"
	-@erase "$(INTDIR)\Abuse32.obj"
	-@erase "$(INTDIR)\Abuse32.res"
	-@erase "$(INTDIR)\ant.obj"
	-@erase "$(INTDIR)\automap.obj"
	-@erase "$(INTDIR)\cache.obj"
	-@erase "$(INTDIR)\chars.obj"
	-@erase "$(INTDIR)\chat.obj"
	-@erase "$(INTDIR)\clisp.obj"
	-@erase "$(INTDIR)\collide.obj"
	-@erase "$(INTDIR)\compiled.obj"
	-@erase "$(INTDIR)\config.obj"
	-@erase "$(INTDIR)\console.obj"
	-@erase "$(INTDIR)\cop.obj"
	-@erase "$(INTDIR)\crc.obj"
	-@erase "$(INTDIR)\decoder.obj"
	-@erase "$(INTDIR)\demo.obj"
	-@erase "$(INTDIR)\dev.obj"
	-@erase "$(INTDIR)\devsel.obj"
	-@erase "$(INTDIR)\director.obj"
	-@erase "$(INTDIR)\dp_prot.obj"
	-@erase "$(INTDIR)\dprint.obj"
	-@erase "$(INTDIR)\endgame.obj"
	-@erase "$(INTDIR)\event.obj"
	-@erase "$(INTDIR)\extend.obj"
	-@erase "$(INTDIR)\fileman.obj"
	-@erase "$(INTDIR)\filesel.obj"
	-@erase "$(INTDIR)\filter.obj"
	-@erase "$(INTDIR)\fnt6x13.obj"
	-@erase "$(INTDIR)\fonts.obj"
	-@erase "$(INTDIR)\game.obj"
	-@erase "$(INTDIR)\gamma.obj"
	-@erase "$(INTDIR)\gclient.obj"
	-@erase "$(INTDIR)\globals.obj"
	-@erase "$(INTDIR)\glread.obj"
	-@erase "$(INTDIR)\gserver.obj"
	-@erase "$(INTDIR)\gui.obj"
	-@erase "$(INTDIR)\guistat.obj"
	-@erase "$(INTDIR)\help.obj"
	-@erase "$(INTDIR)\image.obj"
	-@erase "$(INTDIR)\image24.obj"
	-@erase "$(INTDIR)\innet.obj"
	-@erase "$(INTDIR)\input.obj"
	-@erase "$(INTDIR)\intsect.obj"
	-@erase "$(INTDIR)\items.obj"
	-@erase "$(INTDIR)\jdir.obj"
	-@erase "$(INTDIR)\jmalloc.obj"
	-@erase "$(INTDIR)\jnet.obj"
	-@erase "$(INTDIR)\joystick.obj"
	-@erase "$(INTDIR)\jrand.obj"
	-@erase "$(INTDIR)\jwindow.obj"
	-@erase "$(INTDIR)\keys.obj"
	-@erase "$(INTDIR)\language.obj"
	-@erase "$(INTDIR)\lcache.obj"
	-@erase "$(INTDIR)\letters.obj"
	-@erase "$(INTDIR)\level.obj"
	-@erase "$(INTDIR)\light.obj"
	-@erase "$(INTDIR)\linked.obj"
	-@erase "$(INTDIR)\lisp.obj"
	-@erase "$(INTDIR)\lisp_gc.obj"
	-@erase "$(INTDIR)\lisp_opt.obj"
	-@erase "$(INTDIR)\loader2.obj"
	-@erase "$(INTDIR)\loadgame.obj"
	-@erase "$(INTDIR)\menu.obj"
	-@erase "$(INTDIR)\morpher.obj"
	-@erase "$(INTDIR)\mouse.obj"
	-@erase "$(INTDIR)\netcfg.obj"
	-@erase "$(INTDIR)\nfclient.obj"
	-@erase "$(INTDIR)\objects.obj"
	-@erase "$(INTDIR)\palette.obj"
	-@erase "$(INTDIR)\parse.obj"
	-@erase "$(INTDIR)\particle.obj"
	-@erase "$(INTDIR)\pcxread.obj"
	-@erase "$(INTDIR)\pmenu.obj"
	-@erase "$(INTDIR)\points.obj"
	-@erase "$(INTDIR)\profile.obj"
	-@erase "$(INTDIR)\property.obj"
	-@erase "$(INTDIR)\readwav.obj"
	-@erase "$(INTDIR)\scroller.obj"
	-@erase "$(INTDIR)\sensor.obj"
	-@erase "$(INTDIR)\seq.obj"
	-@erase "$(INTDIR)\SessionPicker.obj"
	-@erase "$(INTDIR)\smallfnt.obj"
	-@erase "$(INTDIR)\sock.obj"
	-@erase "$(INTDIR)\sound.obj"
	-@erase "$(INTDIR)\specache.obj"
	-@erase "$(INTDIR)\specs.obj"
	-@erase "$(INTDIR)\sprite.obj"
	-@erase "$(INTDIR)\statbar.obj"
	-@erase "$(INTDIR)\status.obj"
	-@erase "$(INTDIR)\supmorph.obj"
	-@erase "$(INTDIR)\text_gui.obj"
	-@erase "$(INTDIR)\timage.obj"
	-@erase "$(INTDIR)\timing.obj"
	-@erase "$(INTDIR)\tools.obj"
	-@erase "$(INTDIR)\transp.obj"
	-@erase "$(INTDIR)\trig.obj"
	-@erase "$(INTDIR)\username.obj"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(INTDIR)\version.obj"
	-@erase "$(INTDIR)\video.obj"
	-@erase "$(INTDIR)\view.obj"
	-@erase "$(INTDIR)\visobj.obj"
	-@erase "$(OUTDIR)\Abuse32.exe"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP_PROJ=/nologo /ML /W3 /GX /O2 /Ob2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /Fp"$(INTDIR)\Abuse32.pch" /Yu"stdafx.h" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 
MTL_PROJ=/nologo /I "Win32src" /D "NDEBUG" /mktyplib203 /win32 
RSC_PROJ=/l 0x409 /fo"$(INTDIR)\Abuse32.res" /d "NDEBUG" 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\Abuse32.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib dxguid.lib ddraw.lib dsound.lib /nologo /subsystem:windows /incremental:no /pdb:"$(OUTDIR)\Abuse32.pdb" /machine:I386 /out:"$(OUTDIR)\Abuse32.exe" 
LINK32_OBJS= \
	"$(INTDIR)\dp_prot.obj" \
	"$(INTDIR)\SessionPicker.obj" \
	"$(INTDIR)\sock.obj" \
	"$(INTDIR)\sound.obj" \
	"$(INTDIR)\video.obj" \
	"$(INTDIR)\decoder.obj" \
	"$(INTDIR)\filter.obj" \
	"$(INTDIR)\fonts.obj" \
	"$(INTDIR)\glread.obj" \
	"$(INTDIR)\image.obj" \
	"$(INTDIR)\image24.obj" \
	"$(INTDIR)\palette.obj" \
	"$(INTDIR)\pcxread.obj" \
	"$(INTDIR)\sprite.obj" \
	"$(INTDIR)\supmorph.obj" \
	"$(INTDIR)\timage.obj" \
	"$(INTDIR)\visobj.obj" \
	"$(INTDIR)\specs.obj" \
	"$(INTDIR)\filesel.obj" \
	"$(INTDIR)\jwindow.obj" \
	"$(INTDIR)\pmenu.obj" \
	"$(INTDIR)\scroller.obj" \
	"$(INTDIR)\tools.obj" \
	"$(INTDIR)\jmalloc.obj" \
	"$(INTDIR)\jrand.obj" \
	"$(INTDIR)\linked.obj" \
	"$(INTDIR)\dprint.obj" \
	"$(INTDIR)\globals.obj" \
	"$(INTDIR)\guistat.obj" \
	"$(INTDIR)\input.obj" \
	"$(INTDIR)\keys.obj" \
	"$(INTDIR)\readwav.obj" \
	"$(INTDIR)\status.obj" \
	"$(INTDIR)\event.obj" \
	"$(INTDIR)\jdir.obj" \
	"$(INTDIR)\jnet.obj" \
	"$(INTDIR)\joystick.obj" \
	"$(INTDIR)\mouse.obj" \
	"$(INTDIR)\timing.obj" \
	"$(INTDIR)\fileman.obj" \
	"$(INTDIR)\gclient.obj" \
	"$(INTDIR)\gserver.obj" \
	"$(INTDIR)\innet.obj" \
	"$(INTDIR)\netcfg.obj" \
	"$(INTDIR)\nfclient.obj" \
	"$(INTDIR)\clisp.obj" \
	"$(INTDIR)\compiled.obj" \
	"$(INTDIR)\lisp.obj" \
	"$(INTDIR)\lisp_gc.obj" \
	"$(INTDIR)\lisp_opt.obj" \
	"$(INTDIR)\cache.obj" \
	"$(INTDIR)\lcache.obj" \
	"$(INTDIR)\ability.obj" \
	"$(INTDIR)\ant.obj" \
	"$(INTDIR)\automap.obj" \
	"$(INTDIR)\chars.obj" \
	"$(INTDIR)\chat.obj" \
	"$(INTDIR)\collide.obj" \
	"$(INTDIR)\config.obj" \
	"$(INTDIR)\console.obj" \
	"$(INTDIR)\cop.obj" \
	"$(INTDIR)\crc.obj" \
	"$(INTDIR)\demo.obj" \
	"$(INTDIR)\dev.obj" \
	"$(INTDIR)\devsel.obj" \
	"$(INTDIR)\director.obj" \
	"$(INTDIR)\endgame.obj" \
	"$(INTDIR)\extend.obj" \
	"$(INTDIR)\fnt6x13.obj" \
	"$(INTDIR)\game.obj" \
	"$(INTDIR)\gamma.obj" \
	"$(INTDIR)\gui.obj" \
	"$(INTDIR)\help.obj" \
	"$(INTDIR)\intsect.obj" \
	"$(INTDIR)\items.obj" \
	"$(INTDIR)\language.obj" \
	"$(INTDIR)\letters.obj" \
	"$(INTDIR)\level.obj" \
	"$(INTDIR)\light.obj" \
	"$(INTDIR)\loader2.obj" \
	"$(INTDIR)\loadgame.obj" \
	"$(INTDIR)\menu.obj" \
	"$(INTDIR)\morpher.obj" \
	"$(INTDIR)\objects.obj" \
	"$(INTDIR)\parse.obj" \
	"$(INTDIR)\particle.obj" \
	"$(INTDIR)\points.obj" \
	"$(INTDIR)\profile.obj" \
	"$(INTDIR)\property.obj" \
	"$(INTDIR)\sensor.obj" \
	"$(INTDIR)\seq.obj" \
	"$(INTDIR)\smallfnt.obj" \
	"$(INTDIR)\specache.obj" \
	"$(INTDIR)\statbar.obj" \
	"$(INTDIR)\text_gui.obj" \
	"$(INTDIR)\transp.obj" \
	"$(INTDIR)\trig.obj" \
	"$(INTDIR)\username.obj" \
	"$(INTDIR)\version.obj" \
	"$(INTDIR)\view.obj" \
	"$(INTDIR)\Abuse32.obj" \
	"$(INTDIR)\Abuse32.res"

"$(OUTDIR)\Abuse32.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "Abuse32 - Win32 Debug"

OUTDIR=.\Debug
INTDIR=.\Debug
# Begin Custom Macros
OutDir=.\Debug
# End Custom Macros

ALL : "$(OUTDIR)\Abuse32.exe" "$(OUTDIR)\Abuse32.bsc"


CLEAN :
	-@erase "$(INTDIR)\ability.obj"
	-@erase "$(INTDIR)\ability.sbr"
	-@erase "$(INTDIR)\Abuse32.obj"
	-@erase "$(INTDIR)\Abuse32.res"
	-@erase "$(INTDIR)\Abuse32.sbr"
	-@erase "$(INTDIR)\ant.obj"
	-@erase "$(INTDIR)\ant.sbr"
	-@erase "$(INTDIR)\automap.obj"
	-@erase "$(INTDIR)\automap.sbr"
	-@erase "$(INTDIR)\cache.obj"
	-@erase "$(INTDIR)\cache.sbr"
	-@erase "$(INTDIR)\chars.obj"
	-@erase "$(INTDIR)\chars.sbr"
	-@erase "$(INTDIR)\chat.obj"
	-@erase "$(INTDIR)\chat.sbr"
	-@erase "$(INTDIR)\clisp.obj"
	-@erase "$(INTDIR)\clisp.sbr"
	-@erase "$(INTDIR)\collide.obj"
	-@erase "$(INTDIR)\collide.sbr"
	-@erase "$(INTDIR)\compiled.obj"
	-@erase "$(INTDIR)\compiled.sbr"
	-@erase "$(INTDIR)\config.obj"
	-@erase "$(INTDIR)\config.sbr"
	-@erase "$(INTDIR)\console.obj"
	-@erase "$(INTDIR)\console.sbr"
	-@erase "$(INTDIR)\cop.obj"
	-@erase "$(INTDIR)\cop.sbr"
	-@erase "$(INTDIR)\crc.obj"
	-@erase "$(INTDIR)\crc.sbr"
	-@erase "$(INTDIR)\decoder.obj"
	-@erase "$(INTDIR)\decoder.sbr"
	-@erase "$(INTDIR)\demo.obj"
	-@erase "$(INTDIR)\demo.sbr"
	-@erase "$(INTDIR)\dev.obj"
	-@erase "$(INTDIR)\dev.sbr"
	-@erase "$(INTDIR)\devsel.obj"
	-@erase "$(INTDIR)\devsel.sbr"
	-@erase "$(INTDIR)\director.obj"
	-@erase "$(INTDIR)\director.sbr"
	-@erase "$(INTDIR)\dp_prot.obj"
	-@erase "$(INTDIR)\dp_prot.sbr"
	-@erase "$(INTDIR)\dprint.obj"
	-@erase "$(INTDIR)\dprint.sbr"
	-@erase "$(INTDIR)\endgame.obj"
	-@erase "$(INTDIR)\endgame.sbr"
	-@erase "$(INTDIR)\event.obj"
	-@erase "$(INTDIR)\event.sbr"
	-@erase "$(INTDIR)\extend.obj"
	-@erase "$(INTDIR)\extend.sbr"
	-@erase "$(INTDIR)\fileman.obj"
	-@erase "$(INTDIR)\fileman.sbr"
	-@erase "$(INTDIR)\filesel.obj"
	-@erase "$(INTDIR)\filesel.sbr"
	-@erase "$(INTDIR)\filter.obj"
	-@erase "$(INTDIR)\filter.sbr"
	-@erase "$(INTDIR)\fnt6x13.obj"
	-@erase "$(INTDIR)\fnt6x13.sbr"
	-@erase "$(INTDIR)\fonts.obj"
	-@erase "$(INTDIR)\fonts.sbr"
	-@erase "$(INTDIR)\game.obj"
	-@erase "$(INTDIR)\game.sbr"
	-@erase "$(INTDIR)\gamma.obj"
	-@erase "$(INTDIR)\gamma.sbr"
	-@erase "$(INTDIR)\gclient.obj"
	-@erase "$(INTDIR)\gclient.sbr"
	-@erase "$(INTDIR)\globals.obj"
	-@erase "$(INTDIR)\globals.sbr"
	-@erase "$(INTDIR)\glread.obj"
	-@erase "$(INTDIR)\glread.sbr"
	-@erase "$(INTDIR)\gserver.obj"
	-@erase "$(INTDIR)\gserver.sbr"
	-@erase "$(INTDIR)\gui.obj"
	-@erase "$(INTDIR)\gui.sbr"
	-@erase "$(INTDIR)\guistat.obj"
	-@erase "$(INTDIR)\guistat.sbr"
	-@erase "$(INTDIR)\help.obj"
	-@erase "$(INTDIR)\help.sbr"
	-@erase "$(INTDIR)\image.obj"
	-@erase "$(INTDIR)\image.sbr"
	-@erase "$(INTDIR)\image24.obj"
	-@erase "$(INTDIR)\image24.sbr"
	-@erase "$(INTDIR)\innet.obj"
	-@erase "$(INTDIR)\innet.sbr"
	-@erase "$(INTDIR)\input.obj"
	-@erase "$(INTDIR)\input.sbr"
	-@erase "$(INTDIR)\intsect.obj"
	-@erase "$(INTDIR)\intsect.sbr"
	-@erase "$(INTDIR)\items.obj"
	-@erase "$(INTDIR)\items.sbr"
	-@erase "$(INTDIR)\jdir.obj"
	-@erase "$(INTDIR)\jdir.sbr"
	-@erase "$(INTDIR)\jmalloc.obj"
	-@erase "$(INTDIR)\jmalloc.sbr"
	-@erase "$(INTDIR)\jnet.obj"
	-@erase "$(INTDIR)\jnet.sbr"
	-@erase "$(INTDIR)\joystick.obj"
	-@erase "$(INTDIR)\joystick.sbr"
	-@erase "$(INTDIR)\jrand.obj"
	-@erase "$(INTDIR)\jrand.sbr"
	-@erase "$(INTDIR)\jwindow.obj"
	-@erase "$(INTDIR)\jwindow.sbr"
	-@erase "$(INTDIR)\keys.obj"
	-@erase "$(INTDIR)\keys.sbr"
	-@erase "$(INTDIR)\language.obj"
	-@erase "$(INTDIR)\language.sbr"
	-@erase "$(INTDIR)\lcache.obj"
	-@erase "$(INTDIR)\lcache.sbr"
	-@erase "$(INTDIR)\letters.obj"
	-@erase "$(INTDIR)\letters.sbr"
	-@erase "$(INTDIR)\level.obj"
	-@erase "$(INTDIR)\level.sbr"
	-@erase "$(INTDIR)\light.obj"
	-@erase "$(INTDIR)\light.sbr"
	-@erase "$(INTDIR)\linked.obj"
	-@erase "$(INTDIR)\linked.sbr"
	-@erase "$(INTDIR)\lisp.obj"
	-@erase "$(INTDIR)\lisp.sbr"
	-@erase "$(INTDIR)\lisp_gc.obj"
	-@erase "$(INTDIR)\lisp_gc.sbr"
	-@erase "$(INTDIR)\lisp_opt.obj"
	-@erase "$(INTDIR)\lisp_opt.sbr"
	-@erase "$(INTDIR)\loader2.obj"
	-@erase "$(INTDIR)\loader2.sbr"
	-@erase "$(INTDIR)\loadgame.obj"
	-@erase "$(INTDIR)\loadgame.sbr"
	-@erase "$(INTDIR)\menu.obj"
	-@erase "$(INTDIR)\menu.sbr"
	-@erase "$(INTDIR)\morpher.obj"
	-@erase "$(INTDIR)\morpher.sbr"
	-@erase "$(INTDIR)\mouse.obj"
	-@erase "$(INTDIR)\mouse.sbr"
	-@erase "$(INTDIR)\netcfg.obj"
	-@erase "$(INTDIR)\netcfg.sbr"
	-@erase "$(INTDIR)\nfclient.obj"
	-@erase "$(INTDIR)\nfclient.sbr"
	-@erase "$(INTDIR)\objects.obj"
	-@erase "$(INTDIR)\objects.sbr"
	-@erase "$(INTDIR)\palette.obj"
	-@erase "$(INTDIR)\palette.sbr"
	-@erase "$(INTDIR)\parse.obj"
	-@erase "$(INTDIR)\parse.sbr"
	-@erase "$(INTDIR)\particle.obj"
	-@erase "$(INTDIR)\particle.sbr"
	-@erase "$(INTDIR)\pcxread.obj"
	-@erase "$(INTDIR)\pcxread.sbr"
	-@erase "$(INTDIR)\pmenu.obj"
	-@erase "$(INTDIR)\pmenu.sbr"
	-@erase "$(INTDIR)\points.obj"
	-@erase "$(INTDIR)\points.sbr"
	-@erase "$(INTDIR)\profile.obj"
	-@erase "$(INTDIR)\profile.sbr"
	-@erase "$(INTDIR)\property.obj"
	-@erase "$(INTDIR)\property.sbr"
	-@erase "$(INTDIR)\readwav.obj"
	-@erase "$(INTDIR)\readwav.sbr"
	-@erase "$(INTDIR)\scroller.obj"
	-@erase "$(INTDIR)\scroller.sbr"
	-@erase "$(INTDIR)\sensor.obj"
	-@erase "$(INTDIR)\sensor.sbr"
	-@erase "$(INTDIR)\seq.obj"
	-@erase "$(INTDIR)\seq.sbr"
	-@erase "$(INTDIR)\SessionPicker.obj"
	-@erase "$(INTDIR)\SessionPicker.sbr"
	-@erase "$(INTDIR)\smallfnt.obj"
	-@erase "$(INTDIR)\smallfnt.sbr"
	-@erase "$(INTDIR)\sock.obj"
	-@erase "$(INTDIR)\sock.sbr"
	-@erase "$(INTDIR)\sound.obj"
	-@erase "$(INTDIR)\sound.sbr"
	-@erase "$(INTDIR)\specache.obj"
	-@erase "$(INTDIR)\specache.sbr"
	-@erase "$(INTDIR)\specs.obj"
	-@erase "$(INTDIR)\specs.sbr"
	-@erase "$(INTDIR)\sprite.obj"
	-@erase "$(INTDIR)\sprite.sbr"
	-@erase "$(INTDIR)\statbar.obj"
	-@erase "$(INTDIR)\statbar.sbr"
	-@erase "$(INTDIR)\status.obj"
	-@erase "$(INTDIR)\status.sbr"
	-@erase "$(INTDIR)\supmorph.obj"
	-@erase "$(INTDIR)\supmorph.sbr"
	-@erase "$(INTDIR)\text_gui.obj"
	-@erase "$(INTDIR)\text_gui.sbr"
	-@erase "$(INTDIR)\timage.obj"
	-@erase "$(INTDIR)\timage.sbr"
	-@erase "$(INTDIR)\timing.obj"
	-@erase "$(INTDIR)\timing.sbr"
	-@erase "$(INTDIR)\tools.obj"
	-@erase "$(INTDIR)\tools.sbr"
	-@erase "$(INTDIR)\transp.obj"
	-@erase "$(INTDIR)\transp.sbr"
	-@erase "$(INTDIR)\trig.obj"
	-@erase "$(INTDIR)\trig.sbr"
	-@erase "$(INTDIR)\username.obj"
	-@erase "$(INTDIR)\username.sbr"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(INTDIR)\vc60.pdb"
	-@erase "$(INTDIR)\version.obj"
	-@erase "$(INTDIR)\version.sbr"
	-@erase "$(INTDIR)\video.obj"
	-@erase "$(INTDIR)\video.sbr"
	-@erase "$(INTDIR)\view.obj"
	-@erase "$(INTDIR)\view.sbr"
	-@erase "$(INTDIR)\visobj.obj"
	-@erase "$(INTDIR)\visobj.sbr"
	-@erase "$(OUTDIR)\Abuse32.bsc"
	-@erase "$(OUTDIR)\Abuse32.exe"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP_PROJ=/nologo /MLd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /FR"$(INTDIR)\\" /Fp"$(INTDIR)\Abuse32.pch" /Yu"stdafx.h" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ /c 
MTL_PROJ=/nologo /I "C:\My Documents\My Source\Abuse32\Win32src\\" /D "_DEBUG" /mktyplib203 /win32 
RSC_PROJ=/l 0x409 /fo"$(INTDIR)\Abuse32.res" /d "_DEBUG" 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\Abuse32.bsc" 
BSC32_SBRS= \
	"$(INTDIR)\dp_prot.sbr" \
	"$(INTDIR)\SessionPicker.sbr" \
	"$(INTDIR)\sock.sbr" \
	"$(INTDIR)\sound.sbr" \
	"$(INTDIR)\video.sbr" \
	"$(INTDIR)\decoder.sbr" \
	"$(INTDIR)\filter.sbr" \
	"$(INTDIR)\fonts.sbr" \
	"$(INTDIR)\glread.sbr" \
	"$(INTDIR)\image.sbr" \
	"$(INTDIR)\image24.sbr" \
	"$(INTDIR)\palette.sbr" \
	"$(INTDIR)\pcxread.sbr" \
	"$(INTDIR)\sprite.sbr" \
	"$(INTDIR)\supmorph.sbr" \
	"$(INTDIR)\timage.sbr" \
	"$(INTDIR)\visobj.sbr" \
	"$(INTDIR)\specs.sbr" \
	"$(INTDIR)\filesel.sbr" \
	"$(INTDIR)\jwindow.sbr" \
	"$(INTDIR)\pmenu.sbr" \
	"$(INTDIR)\scroller.sbr" \
	"$(INTDIR)\tools.sbr" \
	"$(INTDIR)\jmalloc.sbr" \
	"$(INTDIR)\jrand.sbr" \
	"$(INTDIR)\linked.sbr" \
	"$(INTDIR)\dprint.sbr" \
	"$(INTDIR)\globals.sbr" \
	"$(INTDIR)\guistat.sbr" \
	"$(INTDIR)\input.sbr" \
	"$(INTDIR)\keys.sbr" \
	"$(INTDIR)\readwav.sbr" \
	"$(INTDIR)\status.sbr" \
	"$(INTDIR)\event.sbr" \
	"$(INTDIR)\jdir.sbr" \
	"$(INTDIR)\jnet.sbr" \
	"$(INTDIR)\joystick.sbr" \
	"$(INTDIR)\mouse.sbr" \
	"$(INTDIR)\timing.sbr" \
	"$(INTDIR)\fileman.sbr" \
	"$(INTDIR)\gclient.sbr" \
	"$(INTDIR)\gserver.sbr" \
	"$(INTDIR)\innet.sbr" \
	"$(INTDIR)\netcfg.sbr" \
	"$(INTDIR)\nfclient.sbr" \
	"$(INTDIR)\clisp.sbr" \
	"$(INTDIR)\compiled.sbr" \
	"$(INTDIR)\lisp.sbr" \
	"$(INTDIR)\lisp_gc.sbr" \
	"$(INTDIR)\lisp_opt.sbr" \
	"$(INTDIR)\cache.sbr" \
	"$(INTDIR)\lcache.sbr" \
	"$(INTDIR)\ability.sbr" \
	"$(INTDIR)\ant.sbr" \
	"$(INTDIR)\automap.sbr" \
	"$(INTDIR)\chars.sbr" \
	"$(INTDIR)\chat.sbr" \
	"$(INTDIR)\collide.sbr" \
	"$(INTDIR)\config.sbr" \
	"$(INTDIR)\console.sbr" \
	"$(INTDIR)\cop.sbr" \
	"$(INTDIR)\crc.sbr" \
	"$(INTDIR)\demo.sbr" \
	"$(INTDIR)\dev.sbr" \
	"$(INTDIR)\devsel.sbr" \
	"$(INTDIR)\director.sbr" \
	"$(INTDIR)\endgame.sbr" \
	"$(INTDIR)\extend.sbr" \
	"$(INTDIR)\fnt6x13.sbr" \
	"$(INTDIR)\game.sbr" \
	"$(INTDIR)\gamma.sbr" \
	"$(INTDIR)\gui.sbr" \
	"$(INTDIR)\help.sbr" \
	"$(INTDIR)\intsect.sbr" \
	"$(INTDIR)\items.sbr" \
	"$(INTDIR)\language.sbr" \
	"$(INTDIR)\letters.sbr" \
	"$(INTDIR)\level.sbr" \
	"$(INTDIR)\light.sbr" \
	"$(INTDIR)\loader2.sbr" \
	"$(INTDIR)\loadgame.sbr" \
	"$(INTDIR)\menu.sbr" \
	"$(INTDIR)\morpher.sbr" \
	"$(INTDIR)\objects.sbr" \
	"$(INTDIR)\parse.sbr" \
	"$(INTDIR)\particle.sbr" \
	"$(INTDIR)\points.sbr" \
	"$(INTDIR)\profile.sbr" \
	"$(INTDIR)\property.sbr" \
	"$(INTDIR)\sensor.sbr" \
	"$(INTDIR)\seq.sbr" \
	"$(INTDIR)\smallfnt.sbr" \
	"$(INTDIR)\specache.sbr" \
	"$(INTDIR)\statbar.sbr" \
	"$(INTDIR)\text_gui.sbr" \
	"$(INTDIR)\transp.sbr" \
	"$(INTDIR)\trig.sbr" \
	"$(INTDIR)\username.sbr" \
	"$(INTDIR)\version.sbr" \
	"$(INTDIR)\view.sbr" \
	"$(INTDIR)\Abuse32.sbr"

"$(OUTDIR)\Abuse32.bsc" : "$(OUTDIR)" $(BSC32_SBRS)
    $(BSC32) @<<
  $(BSC32_FLAGS) $(BSC32_SBRS)
<<

LINK32=link.exe
LINK32_FLAGS=kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib dxguid.lib ddraw.lib dsound.lib /nologo /subsystem:windows /profile /debug /machine:I386 /out:"$(OUTDIR)\Abuse32.exe" 
LINK32_OBJS= \
	"$(INTDIR)\dp_prot.obj" \
	"$(INTDIR)\SessionPicker.obj" \
	"$(INTDIR)\sock.obj" \
	"$(INTDIR)\sound.obj" \
	"$(INTDIR)\video.obj" \
	"$(INTDIR)\decoder.obj" \
	"$(INTDIR)\filter.obj" \
	"$(INTDIR)\fonts.obj" \
	"$(INTDIR)\glread.obj" \
	"$(INTDIR)\image.obj" \
	"$(INTDIR)\image24.obj" \
	"$(INTDIR)\palette.obj" \
	"$(INTDIR)\pcxread.obj" \
	"$(INTDIR)\sprite.obj" \
	"$(INTDIR)\supmorph.obj" \
	"$(INTDIR)\timage.obj" \
	"$(INTDIR)\visobj.obj" \
	"$(INTDIR)\specs.obj" \
	"$(INTDIR)\filesel.obj" \
	"$(INTDIR)\jwindow.obj" \
	"$(INTDIR)\pmenu.obj" \
	"$(INTDIR)\scroller.obj" \
	"$(INTDIR)\tools.obj" \
	"$(INTDIR)\jmalloc.obj" \
	"$(INTDIR)\jrand.obj" \
	"$(INTDIR)\linked.obj" \
	"$(INTDIR)\dprint.obj" \
	"$(INTDIR)\globals.obj" \
	"$(INTDIR)\guistat.obj" \
	"$(INTDIR)\input.obj" \
	"$(INTDIR)\keys.obj" \
	"$(INTDIR)\readwav.obj" \
	"$(INTDIR)\status.obj" \
	"$(INTDIR)\event.obj" \
	"$(INTDIR)\jdir.obj" \
	"$(INTDIR)\jnet.obj" \
	"$(INTDIR)\joystick.obj" \
	"$(INTDIR)\mouse.obj" \
	"$(INTDIR)\timing.obj" \
	"$(INTDIR)\fileman.obj" \
	"$(INTDIR)\gclient.obj" \
	"$(INTDIR)\gserver.obj" \
	"$(INTDIR)\innet.obj" \
	"$(INTDIR)\netcfg.obj" \
	"$(INTDIR)\nfclient.obj" \
	"$(INTDIR)\clisp.obj" \
	"$(INTDIR)\compiled.obj" \
	"$(INTDIR)\lisp.obj" \
	"$(INTDIR)\lisp_gc.obj" \
	"$(INTDIR)\lisp_opt.obj" \
	"$(INTDIR)\cache.obj" \
	"$(INTDIR)\lcache.obj" \
	"$(INTDIR)\ability.obj" \
	"$(INTDIR)\ant.obj" \
	"$(INTDIR)\automap.obj" \
	"$(INTDIR)\chars.obj" \
	"$(INTDIR)\chat.obj" \
	"$(INTDIR)\collide.obj" \
	"$(INTDIR)\config.obj" \
	"$(INTDIR)\console.obj" \
	"$(INTDIR)\cop.obj" \
	"$(INTDIR)\crc.obj" \
	"$(INTDIR)\demo.obj" \
	"$(INTDIR)\dev.obj" \
	"$(INTDIR)\devsel.obj" \
	"$(INTDIR)\director.obj" \
	"$(INTDIR)\endgame.obj" \
	"$(INTDIR)\extend.obj" \
	"$(INTDIR)\fnt6x13.obj" \
	"$(INTDIR)\game.obj" \
	"$(INTDIR)\gamma.obj" \
	"$(INTDIR)\gui.obj" \
	"$(INTDIR)\help.obj" \
	"$(INTDIR)\intsect.obj" \
	"$(INTDIR)\items.obj" \
	"$(INTDIR)\language.obj" \
	"$(INTDIR)\letters.obj" \
	"$(INTDIR)\level.obj" \
	"$(INTDIR)\light.obj" \
	"$(INTDIR)\loader2.obj" \
	"$(INTDIR)\loadgame.obj" \
	"$(INTDIR)\menu.obj" \
	"$(INTDIR)\morpher.obj" \
	"$(INTDIR)\objects.obj" \
	"$(INTDIR)\parse.obj" \
	"$(INTDIR)\particle.obj" \
	"$(INTDIR)\points.obj" \
	"$(INTDIR)\profile.obj" \
	"$(INTDIR)\property.obj" \
	"$(INTDIR)\sensor.obj" \
	"$(INTDIR)\seq.obj" \
	"$(INTDIR)\smallfnt.obj" \
	"$(INTDIR)\specache.obj" \
	"$(INTDIR)\statbar.obj" \
	"$(INTDIR)\text_gui.obj" \
	"$(INTDIR)\transp.obj" \
	"$(INTDIR)\trig.obj" \
	"$(INTDIR)\username.obj" \
	"$(INTDIR)\version.obj" \
	"$(INTDIR)\view.obj" \
	"$(INTDIR)\Abuse32.obj" \
	"$(INTDIR)\Abuse32.res"

"$(OUTDIR)\Abuse32.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ENDIF 

.c{$(INTDIR)}.obj::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cpp{$(INTDIR)}.obj::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cxx{$(INTDIR)}.obj::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.c{$(INTDIR)}.sbr::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cpp{$(INTDIR)}.sbr::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cxx{$(INTDIR)}.sbr::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<


!IF "$(NO_EXTERNAL_DEPS)" != "1"
!IF EXISTS("Abuse32.dep")
!INCLUDE "Abuse32.dep"
!ELSE 
!MESSAGE Warning: cannot find "Abuse32.dep"
!ENDIF 
!ENDIF 


!IF "$(CFG)" == "Abuse32 - Win32 Release" || "$(CFG)" == "Abuse32 - Win32 Debug"
SOURCE=.\Win32src\dp_prot.cpp

!IF  "$(CFG)" == "Abuse32 - Win32 Release"

CPP_SWITCHES=/nologo /ML /W3 /GX /O2 /Ob2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\dp_prot.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "Abuse32 - Win32 Debug"

CPP_SWITCHES=/nologo /MLd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /FR"$(INTDIR)\\" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ /c 

"$(INTDIR)\dp_prot.obj"	"$(INTDIR)\dp_prot.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ENDIF 

SOURCE=.\Win32src\SessionPicker.cpp

!IF  "$(CFG)" == "Abuse32 - Win32 Release"

CPP_SWITCHES=/nologo /ML /W3 /GX /O2 /Ob2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\SessionPicker.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "Abuse32 - Win32 Debug"

CPP_SWITCHES=/nologo /MLd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /FR"$(INTDIR)\\" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ /c 

"$(INTDIR)\SessionPicker.obj"	"$(INTDIR)\SessionPicker.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ENDIF 

SOURCE=.\Win32src\sock.cpp

!IF  "$(CFG)" == "Abuse32 - Win32 Release"

CPP_SWITCHES=/nologo /ML /W3 /GX /O2 /Ob2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\sock.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "Abuse32 - Win32 Debug"

CPP_SWITCHES=/nologo /MLd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /FR"$(INTDIR)\\" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ /c 

"$(INTDIR)\sock.obj"	"$(INTDIR)\sock.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ENDIF 

SOURCE=.\Win32src\sound.cpp

!IF  "$(CFG)" == "Abuse32 - Win32 Release"

CPP_SWITCHES=/nologo /ML /W3 /GX /O2 /Ob2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\sound.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "Abuse32 - Win32 Debug"

CPP_SWITCHES=/nologo /MLd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /FR"$(INTDIR)\\" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ /c 

"$(INTDIR)\sound.obj"	"$(INTDIR)\sound.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ENDIF 

SOURCE=.\Win32src\video.cpp

!IF  "$(CFG)" == "Abuse32 - Win32 Release"

CPP_SWITCHES=/nologo /ML /W3 /GX /O2 /Ob2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\video.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "Abuse32 - Win32 Debug"

CPP_SWITCHES=/nologo /MLd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /FR"$(INTDIR)\\" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ /c 

"$(INTDIR)\video.obj"	"$(INTDIR)\video.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ENDIF 

SOURCE=.\imlib\decoder.cpp

!IF  "$(CFG)" == "Abuse32 - Win32 Release"

CPP_SWITCHES=/nologo /ML /W3 /GX /O2 /Ob2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\decoder.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "Abuse32 - Win32 Debug"

CPP_SWITCHES=/nologo /MLd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /FR"$(INTDIR)\\" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ /c 

"$(INTDIR)\decoder.obj"	"$(INTDIR)\decoder.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ENDIF 

SOURCE=.\imlib\filter.cpp

!IF  "$(CFG)" == "Abuse32 - Win32 Release"

CPP_SWITCHES=/nologo /ML /W3 /GX /O2 /Ob2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\filter.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "Abuse32 - Win32 Debug"

CPP_SWITCHES=/nologo /MLd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /FR"$(INTDIR)\\" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ /c 

"$(INTDIR)\filter.obj"	"$(INTDIR)\filter.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ENDIF 

SOURCE=.\imlib\fonts.cpp

!IF  "$(CFG)" == "Abuse32 - Win32 Release"

CPP_SWITCHES=/nologo /ML /W3 /GX /O2 /Ob2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\fonts.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "Abuse32 - Win32 Debug"

CPP_SWITCHES=/nologo /MLd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /FR"$(INTDIR)\\" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ /c 

"$(INTDIR)\fonts.obj"	"$(INTDIR)\fonts.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ENDIF 

SOURCE=.\imlib\glread.cpp

!IF  "$(CFG)" == "Abuse32 - Win32 Release"

CPP_SWITCHES=/nologo /ML /W3 /GX /O2 /Ob2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\glread.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "Abuse32 - Win32 Debug"

CPP_SWITCHES=/nologo /MLd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /FR"$(INTDIR)\\" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ /c 

"$(INTDIR)\glread.obj"	"$(INTDIR)\glread.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ENDIF 

SOURCE=.\imlib\image.cpp

!IF  "$(CFG)" == "Abuse32 - Win32 Release"

CPP_SWITCHES=/nologo /ML /W3 /GX /O2 /Ob2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\image.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "Abuse32 - Win32 Debug"

CPP_SWITCHES=/nologo /MLd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /FR"$(INTDIR)\\" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ /c 

"$(INTDIR)\image.obj"	"$(INTDIR)\image.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ENDIF 

SOURCE=.\imlib\image24.cpp

!IF  "$(CFG)" == "Abuse32 - Win32 Release"

CPP_SWITCHES=/nologo /ML /W3 /GX /O2 /Ob2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\image24.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "Abuse32 - Win32 Debug"

CPP_SWITCHES=/nologo /MLd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /FR"$(INTDIR)\\" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ /c 

"$(INTDIR)\image24.obj"	"$(INTDIR)\image24.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ENDIF 

SOURCE=.\imlib\palette.cpp

!IF  "$(CFG)" == "Abuse32 - Win32 Release"

CPP_SWITCHES=/nologo /ML /W3 /GX /O2 /Ob2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\palette.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "Abuse32 - Win32 Debug"

CPP_SWITCHES=/nologo /MLd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /FR"$(INTDIR)\\" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ /c 

"$(INTDIR)\palette.obj"	"$(INTDIR)\palette.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ENDIF 

SOURCE=.\imlib\pcxread.cpp

!IF  "$(CFG)" == "Abuse32 - Win32 Release"

CPP_SWITCHES=/nologo /ML /W3 /GX /O2 /Ob2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\pcxread.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "Abuse32 - Win32 Debug"

CPP_SWITCHES=/nologo /MLd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /FR"$(INTDIR)\\" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ /c 

"$(INTDIR)\pcxread.obj"	"$(INTDIR)\pcxread.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ENDIF 

SOURCE=.\imlib\sprite.cpp

!IF  "$(CFG)" == "Abuse32 - Win32 Release"

CPP_SWITCHES=/nologo /ML /W3 /GX /O2 /Ob2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\sprite.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "Abuse32 - Win32 Debug"

CPP_SWITCHES=/nologo /MLd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /FR"$(INTDIR)\\" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ /c 

"$(INTDIR)\sprite.obj"	"$(INTDIR)\sprite.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ENDIF 

SOURCE=.\imlib\supmorph.cpp

!IF  "$(CFG)" == "Abuse32 - Win32 Release"

CPP_SWITCHES=/nologo /ML /W3 /GX /O2 /Ob2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\supmorph.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "Abuse32 - Win32 Debug"

CPP_SWITCHES=/nologo /MLd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /FR"$(INTDIR)\\" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ /c 

"$(INTDIR)\supmorph.obj"	"$(INTDIR)\supmorph.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ENDIF 

SOURCE=.\imlib\timage.cpp

!IF  "$(CFG)" == "Abuse32 - Win32 Release"

CPP_SWITCHES=/nologo /ML /W3 /GX /O2 /Ob2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\timage.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "Abuse32 - Win32 Debug"

CPP_SWITCHES=/nologo /MLd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /FR"$(INTDIR)\\" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ /c 

"$(INTDIR)\timage.obj"	"$(INTDIR)\timage.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ENDIF 

SOURCE=.\imlib\visobj.cpp

!IF  "$(CFG)" == "Abuse32 - Win32 Release"

CPP_SWITCHES=/nologo /ML /W3 /GX /O2 /Ob2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\visobj.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "Abuse32 - Win32 Debug"

CPP_SWITCHES=/nologo /MLd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /FR"$(INTDIR)\\" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ /c 

"$(INTDIR)\visobj.obj"	"$(INTDIR)\visobj.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ENDIF 

SOURCE=.\imlib\specs.cpp

!IF  "$(CFG)" == "Abuse32 - Win32 Release"

CPP_SWITCHES=/nologo /ML /W3 /GX /O2 /Ob2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\specs.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "Abuse32 - Win32 Debug"

CPP_SWITCHES=/nologo /MLd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /FR"$(INTDIR)\\" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ /c 

"$(INTDIR)\specs.obj"	"$(INTDIR)\specs.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ENDIF 

SOURCE=.\imlib\filesel.cpp

!IF  "$(CFG)" == "Abuse32 - Win32 Release"

CPP_SWITCHES=/nologo /ML /W3 /GX /O2 /Ob2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\filesel.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "Abuse32 - Win32 Debug"

CPP_SWITCHES=/nologo /MLd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /FR"$(INTDIR)\\" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ /c 

"$(INTDIR)\filesel.obj"	"$(INTDIR)\filesel.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ENDIF 

SOURCE=.\imlib\jwindow.cpp

!IF  "$(CFG)" == "Abuse32 - Win32 Release"

CPP_SWITCHES=/nologo /ML /W3 /GX /O2 /Ob2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\jwindow.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "Abuse32 - Win32 Debug"

CPP_SWITCHES=/nologo /MLd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /FR"$(INTDIR)\\" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ /c 

"$(INTDIR)\jwindow.obj"	"$(INTDIR)\jwindow.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ENDIF 

SOURCE=.\imlib\pmenu.cpp

!IF  "$(CFG)" == "Abuse32 - Win32 Release"

CPP_SWITCHES=/nologo /ML /W3 /GX /O2 /Ob2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\pmenu.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "Abuse32 - Win32 Debug"

CPP_SWITCHES=/nologo /MLd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /FR"$(INTDIR)\\" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ /c 

"$(INTDIR)\pmenu.obj"	"$(INTDIR)\pmenu.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ENDIF 

SOURCE=.\imlib\scroller.cpp

!IF  "$(CFG)" == "Abuse32 - Win32 Release"

CPP_SWITCHES=/nologo /ML /W3 /GX /O2 /Ob2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\scroller.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "Abuse32 - Win32 Debug"

CPP_SWITCHES=/nologo /MLd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /FR"$(INTDIR)\\" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ /c 

"$(INTDIR)\scroller.obj"	"$(INTDIR)\scroller.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ENDIF 

SOURCE=.\imlib\tools.cpp

!IF  "$(CFG)" == "Abuse32 - Win32 Release"

CPP_SWITCHES=/nologo /ML /W3 /GX /O2 /Ob2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\tools.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "Abuse32 - Win32 Debug"

CPP_SWITCHES=/nologo /MLd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /FR"$(INTDIR)\\" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ /c 

"$(INTDIR)\tools.obj"	"$(INTDIR)\tools.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ENDIF 

SOURCE=.\imlib\jmalloc.cpp

!IF  "$(CFG)" == "Abuse32 - Win32 Release"

CPP_SWITCHES=/nologo /ML /W3 /GX /O2 /Ob2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\jmalloc.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "Abuse32 - Win32 Debug"

CPP_SWITCHES=/nologo /MLd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /FR"$(INTDIR)\\" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ /c 

"$(INTDIR)\jmalloc.obj"	"$(INTDIR)\jmalloc.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ENDIF 

SOURCE=.\imlib\jrand.cpp

!IF  "$(CFG)" == "Abuse32 - Win32 Release"

CPP_SWITCHES=/nologo /ML /W3 /GX /O2 /Ob2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\jrand.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "Abuse32 - Win32 Debug"

CPP_SWITCHES=/nologo /MLd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /FR"$(INTDIR)\\" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ /c 

"$(INTDIR)\jrand.obj"	"$(INTDIR)\jrand.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ENDIF 

SOURCE=.\imlib\linked.cpp

!IF  "$(CFG)" == "Abuse32 - Win32 Release"

CPP_SWITCHES=/nologo /ML /W3 /GX /O2 /Ob2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\linked.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "Abuse32 - Win32 Debug"

CPP_SWITCHES=/nologo /MLd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /FR"$(INTDIR)\\" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ /c 

"$(INTDIR)\linked.obj"	"$(INTDIR)\linked.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ENDIF 

SOURCE=.\imlib\dprint.cpp

!IF  "$(CFG)" == "Abuse32 - Win32 Release"

CPP_SWITCHES=/nologo /ML /W3 /GX /O2 /Ob2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\dprint.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "Abuse32 - Win32 Debug"

CPP_SWITCHES=/nologo /MLd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /FR"$(INTDIR)\\" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ /c 

"$(INTDIR)\dprint.obj"	"$(INTDIR)\dprint.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ENDIF 

SOURCE=.\imlib\globals.cpp

!IF  "$(CFG)" == "Abuse32 - Win32 Release"

CPP_SWITCHES=/nologo /ML /W3 /GX /O2 /Ob2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\globals.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "Abuse32 - Win32 Debug"

CPP_SWITCHES=/nologo /MLd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /FR"$(INTDIR)\\" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ /c 

"$(INTDIR)\globals.obj"	"$(INTDIR)\globals.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ENDIF 

SOURCE=.\imlib\guistat.cpp

!IF  "$(CFG)" == "Abuse32 - Win32 Release"

CPP_SWITCHES=/nologo /ML /W3 /GX /O2 /Ob2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\guistat.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "Abuse32 - Win32 Debug"

CPP_SWITCHES=/nologo /MLd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /FR"$(INTDIR)\\" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ /c 

"$(INTDIR)\guistat.obj"	"$(INTDIR)\guistat.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ENDIF 

SOURCE=.\imlib\input.cpp

!IF  "$(CFG)" == "Abuse32 - Win32 Release"

CPP_SWITCHES=/nologo /ML /W3 /GX /O2 /Ob2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\input.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "Abuse32 - Win32 Debug"

CPP_SWITCHES=/nologo /MLd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /FR"$(INTDIR)\\" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ /c 

"$(INTDIR)\input.obj"	"$(INTDIR)\input.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ENDIF 

SOURCE=.\imlib\keys.cpp

!IF  "$(CFG)" == "Abuse32 - Win32 Release"

CPP_SWITCHES=/nologo /ML /W3 /GX /O2 /Ob2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\keys.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "Abuse32 - Win32 Debug"

CPP_SWITCHES=/nologo /MLd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /FR"$(INTDIR)\\" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ /c 

"$(INTDIR)\keys.obj"	"$(INTDIR)\keys.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ENDIF 

SOURCE=.\imlib\readwav.cpp

!IF  "$(CFG)" == "Abuse32 - Win32 Release"

CPP_SWITCHES=/nologo /ML /W3 /GX /O2 /Ob2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\readwav.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "Abuse32 - Win32 Debug"

CPP_SWITCHES=/nologo /MLd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /FR"$(INTDIR)\\" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ /c 

"$(INTDIR)\readwav.obj"	"$(INTDIR)\readwav.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ENDIF 

SOURCE=.\imlib\status.cpp

!IF  "$(CFG)" == "Abuse32 - Win32 Release"

CPP_SWITCHES=/nologo /ML /W3 /GX /O2 /Ob2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\status.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "Abuse32 - Win32 Debug"

CPP_SWITCHES=/nologo /MLd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /FR"$(INTDIR)\\" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ /c 

"$(INTDIR)\status.obj"	"$(INTDIR)\status.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ENDIF 

SOURCE=.\imlib\port\dos4gw\event.cpp

!IF  "$(CFG)" == "Abuse32 - Win32 Release"

CPP_SWITCHES=/nologo /ML /W3 /GX /O2 /Ob2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\event.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "Abuse32 - Win32 Debug"

CPP_SWITCHES=/nologo /MLd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /FR"$(INTDIR)\\" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ /c 

"$(INTDIR)\event.obj"	"$(INTDIR)\event.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ENDIF 

SOURCE=.\imlib\port\dos4gw\jdir.cpp

!IF  "$(CFG)" == "Abuse32 - Win32 Release"

CPP_SWITCHES=/nologo /ML /W3 /GX /O2 /Ob2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\jdir.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "Abuse32 - Win32 Debug"

CPP_SWITCHES=/nologo /MLd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /FR"$(INTDIR)\\" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ /c 

"$(INTDIR)\jdir.obj"	"$(INTDIR)\jdir.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ENDIF 

SOURCE=.\imlib\port\dos4gw\jnet.cpp

!IF  "$(CFG)" == "Abuse32 - Win32 Release"

CPP_SWITCHES=/nologo /ML /W3 /GX /O2 /Ob2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\jnet.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "Abuse32 - Win32 Debug"

CPP_SWITCHES=/nologo /MLd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /FR"$(INTDIR)\\" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ /c 

"$(INTDIR)\jnet.obj"	"$(INTDIR)\jnet.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ENDIF 

SOURCE=.\imlib\port\dos4gw\joystick.cpp

!IF  "$(CFG)" == "Abuse32 - Win32 Release"

CPP_SWITCHES=/nologo /ML /W3 /GX /O2 /Ob2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\joystick.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "Abuse32 - Win32 Debug"

CPP_SWITCHES=/nologo /MLd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /FR"$(INTDIR)\\" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ /c 

"$(INTDIR)\joystick.obj"	"$(INTDIR)\joystick.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ENDIF 

SOURCE=.\imlib\port\dos4gw\mouse.cpp

!IF  "$(CFG)" == "Abuse32 - Win32 Release"

CPP_SWITCHES=/nologo /ML /W3 /GX /O2 /Ob2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\mouse.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "Abuse32 - Win32 Debug"

CPP_SWITCHES=/nologo /MLd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /FR"$(INTDIR)\\" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ /c 

"$(INTDIR)\mouse.obj"	"$(INTDIR)\mouse.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ENDIF 

SOURCE=.\imlib\port\dos4gw\timing.cpp

!IF  "$(CFG)" == "Abuse32 - Win32 Release"

CPP_SWITCHES=/nologo /ML /W3 /GX /O2 /Ob2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\timing.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "Abuse32 - Win32 Debug"

CPP_SWITCHES=/nologo /MLd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /FR"$(INTDIR)\\" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ /c 

"$(INTDIR)\timing.obj"	"$(INTDIR)\timing.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ENDIF 

SOURCE=.\abuse\src\net\unix\fileman.cpp

!IF  "$(CFG)" == "Abuse32 - Win32 Release"

CPP_SWITCHES=/nologo /ML /W3 /GX /O2 /Ob2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\fileman.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "Abuse32 - Win32 Debug"

CPP_SWITCHES=/nologo /MLd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /FR"$(INTDIR)\\" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ /c 

"$(INTDIR)\fileman.obj"	"$(INTDIR)\fileman.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ENDIF 

SOURCE=.\abuse\src\net\unix\gclient.cpp

!IF  "$(CFG)" == "Abuse32 - Win32 Release"

CPP_SWITCHES=/nologo /ML /W3 /GX /O2 /Ob2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\gclient.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "Abuse32 - Win32 Debug"

CPP_SWITCHES=/nologo /MLd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /FR"$(INTDIR)\\" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ /c 

"$(INTDIR)\gclient.obj"	"$(INTDIR)\gclient.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ENDIF 

SOURCE=.\abuse\src\net\unix\gserver.cpp

!IF  "$(CFG)" == "Abuse32 - Win32 Release"

CPP_SWITCHES=/nologo /ML /W3 /GX /O2 /Ob2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\gserver.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "Abuse32 - Win32 Debug"

CPP_SWITCHES=/nologo /MLd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /FR"$(INTDIR)\\" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ /c 

"$(INTDIR)\gserver.obj"	"$(INTDIR)\gserver.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ENDIF 

SOURCE=.\abuse\src\innet.cpp

!IF  "$(CFG)" == "Abuse32 - Win32 Release"

CPP_SWITCHES=/nologo /ML /W3 /GX /O2 /Ob2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\innet.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "Abuse32 - Win32 Debug"

CPP_SWITCHES=/nologo /MLd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /FR"$(INTDIR)\\" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ /c 

"$(INTDIR)\innet.obj"	"$(INTDIR)\innet.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ENDIF 

SOURCE=.\abuse\src\netcfg.cpp

!IF  "$(CFG)" == "Abuse32 - Win32 Release"

CPP_SWITCHES=/nologo /ML /W3 /GX /O2 /Ob2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\netcfg.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "Abuse32 - Win32 Debug"

CPP_SWITCHES=/nologo /MLd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /FR"$(INTDIR)\\" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ /c 

"$(INTDIR)\netcfg.obj"	"$(INTDIR)\netcfg.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ENDIF 

SOURCE=.\abuse\src\nfclient.cpp

!IF  "$(CFG)" == "Abuse32 - Win32 Release"

CPP_SWITCHES=/nologo /ML /W3 /GX /O2 /Ob2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\nfclient.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "Abuse32 - Win32 Debug"

CPP_SWITCHES=/nologo /MLd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /FR"$(INTDIR)\\" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ /c 

"$(INTDIR)\nfclient.obj"	"$(INTDIR)\nfclient.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ENDIF 

SOURCE=.\abuse\src\clisp.cpp

!IF  "$(CFG)" == "Abuse32 - Win32 Release"

CPP_SWITCHES=/nologo /ML /W3 /GX /O2 /Ob2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\clisp.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "Abuse32 - Win32 Debug"

CPP_SWITCHES=/nologo /MLd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /FR"$(INTDIR)\\" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ /c 

"$(INTDIR)\clisp.obj"	"$(INTDIR)\clisp.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ENDIF 

SOURCE=.\abuse\src\compiled.cpp

!IF  "$(CFG)" == "Abuse32 - Win32 Release"

CPP_SWITCHES=/nologo /ML /W3 /GX /O2 /Ob2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\compiled.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "Abuse32 - Win32 Debug"

CPP_SWITCHES=/nologo /MLd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /FR"$(INTDIR)\\" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ /c 

"$(INTDIR)\compiled.obj"	"$(INTDIR)\compiled.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ENDIF 

SOURCE=.\abuse\src\lisp.cpp

!IF  "$(CFG)" == "Abuse32 - Win32 Release"

CPP_SWITCHES=/nologo /ML /W3 /GX /O2 /Ob2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\lisp.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "Abuse32 - Win32 Debug"

CPP_SWITCHES=/nologo /MLd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /FR"$(INTDIR)\\" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ /c 

"$(INTDIR)\lisp.obj"	"$(INTDIR)\lisp.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ENDIF 

SOURCE=.\abuse\src\lisp_gc.cpp

!IF  "$(CFG)" == "Abuse32 - Win32 Release"

CPP_SWITCHES=/nologo /ML /W3 /GX /O2 /Ob2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\lisp_gc.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "Abuse32 - Win32 Debug"

CPP_SWITCHES=/nologo /MLd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /FR"$(INTDIR)\\" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ /c 

"$(INTDIR)\lisp_gc.obj"	"$(INTDIR)\lisp_gc.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ENDIF 

SOURCE=.\abuse\src\lisp_opt.cpp

!IF  "$(CFG)" == "Abuse32 - Win32 Release"

CPP_SWITCHES=/nologo /ML /W3 /GX /O2 /Ob2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\lisp_opt.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "Abuse32 - Win32 Debug"

CPP_SWITCHES=/nologo /MLd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /FR"$(INTDIR)\\" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ /c 

"$(INTDIR)\lisp_opt.obj"	"$(INTDIR)\lisp_opt.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ENDIF 

SOURCE=.\abuse\src\cache.cpp

!IF  "$(CFG)" == "Abuse32 - Win32 Release"

CPP_SWITCHES=/nologo /ML /W3 /GX /O2 /Ob2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\cache.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "Abuse32 - Win32 Debug"

CPP_SWITCHES=/nologo /MLd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /FR"$(INTDIR)\\" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ /c 

"$(INTDIR)\cache.obj"	"$(INTDIR)\cache.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ENDIF 

SOURCE=.\abuse\src\lcache.cpp

!IF  "$(CFG)" == "Abuse32 - Win32 Release"

CPP_SWITCHES=/nologo /ML /W3 /GX /O2 /Ob2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\lcache.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "Abuse32 - Win32 Debug"

CPP_SWITCHES=/nologo /MLd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /FR"$(INTDIR)\\" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ /c 

"$(INTDIR)\lcache.obj"	"$(INTDIR)\lcache.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ENDIF 

SOURCE=.\abuse\src\ability.cpp

!IF  "$(CFG)" == "Abuse32 - Win32 Release"

CPP_SWITCHES=/nologo /ML /W3 /GX /O2 /Ob2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\ability.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "Abuse32 - Win32 Debug"

CPP_SWITCHES=/nologo /MLd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /FR"$(INTDIR)\\" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ /c 

"$(INTDIR)\ability.obj"	"$(INTDIR)\ability.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ENDIF 

SOURCE=.\abuse\src\ant.cpp

!IF  "$(CFG)" == "Abuse32 - Win32 Release"

CPP_SWITCHES=/nologo /ML /W3 /GX /O2 /Ob2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\ant.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "Abuse32 - Win32 Debug"

CPP_SWITCHES=/nologo /MLd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /FR"$(INTDIR)\\" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ /c 

"$(INTDIR)\ant.obj"	"$(INTDIR)\ant.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ENDIF 

SOURCE=.\abuse\src\automap.cpp

!IF  "$(CFG)" == "Abuse32 - Win32 Release"

CPP_SWITCHES=/nologo /ML /W3 /GX /O2 /Ob2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\automap.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "Abuse32 - Win32 Debug"

CPP_SWITCHES=/nologo /MLd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /FR"$(INTDIR)\\" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ /c 

"$(INTDIR)\automap.obj"	"$(INTDIR)\automap.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ENDIF 

SOURCE=.\abuse\src\chars.cpp

!IF  "$(CFG)" == "Abuse32 - Win32 Release"

CPP_SWITCHES=/nologo /ML /W3 /GX /O2 /Ob2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\chars.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "Abuse32 - Win32 Debug"

CPP_SWITCHES=/nologo /MLd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /FR"$(INTDIR)\\" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ /c 

"$(INTDIR)\chars.obj"	"$(INTDIR)\chars.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ENDIF 

SOURCE=.\abuse\src\chat.cpp

!IF  "$(CFG)" == "Abuse32 - Win32 Release"

CPP_SWITCHES=/nologo /ML /W3 /GX /O2 /Ob2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\chat.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "Abuse32 - Win32 Debug"

CPP_SWITCHES=/nologo /MLd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /FR"$(INTDIR)\\" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ /c 

"$(INTDIR)\chat.obj"	"$(INTDIR)\chat.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ENDIF 

SOURCE=.\abuse\src\collide.cpp

!IF  "$(CFG)" == "Abuse32 - Win32 Release"

CPP_SWITCHES=/nologo /ML /W3 /GX /O2 /Ob2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\collide.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "Abuse32 - Win32 Debug"

CPP_SWITCHES=/nologo /MLd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /FR"$(INTDIR)\\" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ /c 

"$(INTDIR)\collide.obj"	"$(INTDIR)\collide.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ENDIF 

SOURCE=.\abuse\src\config.cpp

!IF  "$(CFG)" == "Abuse32 - Win32 Release"

CPP_SWITCHES=/nologo /ML /W3 /GX /O2 /Ob2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\config.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "Abuse32 - Win32 Debug"

CPP_SWITCHES=/nologo /MLd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /FR"$(INTDIR)\\" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ /c 

"$(INTDIR)\config.obj"	"$(INTDIR)\config.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ENDIF 

SOURCE=.\abuse\src\console.cpp

!IF  "$(CFG)" == "Abuse32 - Win32 Release"

CPP_SWITCHES=/nologo /ML /W3 /GX /O2 /Ob2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\console.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "Abuse32 - Win32 Debug"

CPP_SWITCHES=/nologo /MLd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /FR"$(INTDIR)\\" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ /c 

"$(INTDIR)\console.obj"	"$(INTDIR)\console.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ENDIF 

SOURCE=.\abuse\src\cop.cpp

!IF  "$(CFG)" == "Abuse32 - Win32 Release"

CPP_SWITCHES=/nologo /ML /W3 /GX /O2 /Ob2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\cop.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "Abuse32 - Win32 Debug"

CPP_SWITCHES=/nologo /MLd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /FR"$(INTDIR)\\" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ /c 

"$(INTDIR)\cop.obj"	"$(INTDIR)\cop.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ENDIF 

SOURCE=.\abuse\src\crc.cpp

!IF  "$(CFG)" == "Abuse32 - Win32 Release"

CPP_SWITCHES=/nologo /ML /W3 /GX /O2 /Ob2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\crc.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "Abuse32 - Win32 Debug"

CPP_SWITCHES=/nologo /MLd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /FR"$(INTDIR)\\" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ /c 

"$(INTDIR)\crc.obj"	"$(INTDIR)\crc.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ENDIF 

SOURCE=.\abuse\src\demo.cpp

!IF  "$(CFG)" == "Abuse32 - Win32 Release"

CPP_SWITCHES=/nologo /ML /W3 /GX /O2 /Ob2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\demo.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "Abuse32 - Win32 Debug"

CPP_SWITCHES=/nologo /MLd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /FR"$(INTDIR)\\" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ /c 

"$(INTDIR)\demo.obj"	"$(INTDIR)\demo.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ENDIF 

SOURCE=.\abuse\src\dev.cpp

!IF  "$(CFG)" == "Abuse32 - Win32 Release"

CPP_SWITCHES=/nologo /ML /W3 /GX /O2 /Ob2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\dev.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "Abuse32 - Win32 Debug"

CPP_SWITCHES=/nologo /MLd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /FR"$(INTDIR)\\" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ /c 

"$(INTDIR)\dev.obj"	"$(INTDIR)\dev.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ENDIF 

SOURCE=.\abuse\src\devsel.cpp

!IF  "$(CFG)" == "Abuse32 - Win32 Release"

CPP_SWITCHES=/nologo /ML /W3 /GX /O2 /Ob2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\devsel.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "Abuse32 - Win32 Debug"

CPP_SWITCHES=/nologo /MLd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /FR"$(INTDIR)\\" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ /c 

"$(INTDIR)\devsel.obj"	"$(INTDIR)\devsel.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ENDIF 

SOURCE=.\abuse\src\director.cpp

!IF  "$(CFG)" == "Abuse32 - Win32 Release"

CPP_SWITCHES=/nologo /ML /W3 /GX /O2 /Ob2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\director.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "Abuse32 - Win32 Debug"

CPP_SWITCHES=/nologo /MLd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /FR"$(INTDIR)\\" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ /c 

"$(INTDIR)\director.obj"	"$(INTDIR)\director.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ENDIF 

SOURCE=.\abuse\src\endgame.cpp

!IF  "$(CFG)" == "Abuse32 - Win32 Release"

CPP_SWITCHES=/nologo /ML /W3 /GX /O2 /Ob2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\endgame.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "Abuse32 - Win32 Debug"

CPP_SWITCHES=/nologo /MLd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /FR"$(INTDIR)\\" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ /c 

"$(INTDIR)\endgame.obj"	"$(INTDIR)\endgame.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ENDIF 

SOURCE=.\abuse\src\extend.cpp

!IF  "$(CFG)" == "Abuse32 - Win32 Release"

CPP_SWITCHES=/nologo /ML /W3 /GX /O2 /Ob2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\extend.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "Abuse32 - Win32 Debug"

CPP_SWITCHES=/nologo /MLd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /FR"$(INTDIR)\\" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ /c 

"$(INTDIR)\extend.obj"	"$(INTDIR)\extend.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ENDIF 

SOURCE=.\abuse\src\fnt6x13.cpp

!IF  "$(CFG)" == "Abuse32 - Win32 Release"

CPP_SWITCHES=/nologo /ML /W3 /GX /O2 /Ob2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\fnt6x13.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "Abuse32 - Win32 Debug"

CPP_SWITCHES=/nologo /MLd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /FR"$(INTDIR)\\" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ /c 

"$(INTDIR)\fnt6x13.obj"	"$(INTDIR)\fnt6x13.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ENDIF 

SOURCE=.\abuse\src\game.cpp

!IF  "$(CFG)" == "Abuse32 - Win32 Release"

CPP_SWITCHES=/nologo /ML /W3 /GX /O2 /Ob2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\game.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "Abuse32 - Win32 Debug"

CPP_SWITCHES=/nologo /MLd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /FR"$(INTDIR)\\" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ /c 

"$(INTDIR)\game.obj"	"$(INTDIR)\game.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ENDIF 

SOURCE=.\abuse\src\gamma.cpp

!IF  "$(CFG)" == "Abuse32 - Win32 Release"

CPP_SWITCHES=/nologo /ML /W3 /GX /O2 /Ob2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\gamma.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "Abuse32 - Win32 Debug"

CPP_SWITCHES=/nologo /MLd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /FR"$(INTDIR)\\" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ /c 

"$(INTDIR)\gamma.obj"	"$(INTDIR)\gamma.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ENDIF 

SOURCE=.\abuse\src\gui.cpp

!IF  "$(CFG)" == "Abuse32 - Win32 Release"

CPP_SWITCHES=/nologo /ML /W3 /GX /O2 /Ob2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\gui.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "Abuse32 - Win32 Debug"

CPP_SWITCHES=/nologo /MLd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /FR"$(INTDIR)\\" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ /c 

"$(INTDIR)\gui.obj"	"$(INTDIR)\gui.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ENDIF 

SOURCE=.\abuse\src\help.cpp

!IF  "$(CFG)" == "Abuse32 - Win32 Release"

CPP_SWITCHES=/nologo /ML /W3 /GX /O2 /Ob2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\help.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "Abuse32 - Win32 Debug"

CPP_SWITCHES=/nologo /MLd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /FR"$(INTDIR)\\" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ /c 

"$(INTDIR)\help.obj"	"$(INTDIR)\help.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ENDIF 

SOURCE=.\abuse\src\intsect.cpp

!IF  "$(CFG)" == "Abuse32 - Win32 Release"

CPP_SWITCHES=/nologo /ML /W3 /GX /O2 /Ob2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\intsect.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "Abuse32 - Win32 Debug"

CPP_SWITCHES=/nologo /MLd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /FR"$(INTDIR)\\" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ /c 

"$(INTDIR)\intsect.obj"	"$(INTDIR)\intsect.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ENDIF 

SOURCE=.\abuse\src\items.cpp

!IF  "$(CFG)" == "Abuse32 - Win32 Release"

CPP_SWITCHES=/nologo /ML /W3 /GX /O2 /Ob2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\items.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "Abuse32 - Win32 Debug"

CPP_SWITCHES=/nologo /MLd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /FR"$(INTDIR)\\" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ /c 

"$(INTDIR)\items.obj"	"$(INTDIR)\items.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ENDIF 

SOURCE=.\abuse\src\language.cpp

!IF  "$(CFG)" == "Abuse32 - Win32 Release"

CPP_SWITCHES=/nologo /ML /W3 /GX /O2 /Ob2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\language.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "Abuse32 - Win32 Debug"

CPP_SWITCHES=/nologo /MLd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /FR"$(INTDIR)\\" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ /c 

"$(INTDIR)\language.obj"	"$(INTDIR)\language.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ENDIF 

SOURCE=.\abuse\src\letters.cpp

!IF  "$(CFG)" == "Abuse32 - Win32 Release"

CPP_SWITCHES=/nologo /ML /W3 /GX /O2 /Ob2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\letters.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "Abuse32 - Win32 Debug"

CPP_SWITCHES=/nologo /MLd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /FR"$(INTDIR)\\" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ /c 

"$(INTDIR)\letters.obj"	"$(INTDIR)\letters.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ENDIF 

SOURCE=.\abuse\src\level.cpp

!IF  "$(CFG)" == "Abuse32 - Win32 Release"

CPP_SWITCHES=/nologo /ML /W3 /GX /O2 /Ob2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\level.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "Abuse32 - Win32 Debug"

CPP_SWITCHES=/nologo /MLd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /FR"$(INTDIR)\\" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ /c 

"$(INTDIR)\level.obj"	"$(INTDIR)\level.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ENDIF 

SOURCE=.\abuse\src\light.cpp

!IF  "$(CFG)" == "Abuse32 - Win32 Release"

CPP_SWITCHES=/nologo /ML /W3 /GX /O2 /Ob2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\light.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "Abuse32 - Win32 Debug"

CPP_SWITCHES=/nologo /MLd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /FR"$(INTDIR)\\" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ /c 

"$(INTDIR)\light.obj"	"$(INTDIR)\light.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ENDIF 

SOURCE=.\abuse\src\loader2.cpp

!IF  "$(CFG)" == "Abuse32 - Win32 Release"

CPP_SWITCHES=/nologo /ML /W3 /GX /O2 /Ob2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\loader2.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "Abuse32 - Win32 Debug"

CPP_SWITCHES=/nologo /MLd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /FR"$(INTDIR)\\" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ /c 

"$(INTDIR)\loader2.obj"	"$(INTDIR)\loader2.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ENDIF 

SOURCE=.\abuse\src\loadgame.cpp

!IF  "$(CFG)" == "Abuse32 - Win32 Release"

CPP_SWITCHES=/nologo /ML /W3 /GX /O2 /Ob2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\loadgame.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "Abuse32 - Win32 Debug"

CPP_SWITCHES=/nologo /MLd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /FR"$(INTDIR)\\" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ /c 

"$(INTDIR)\loadgame.obj"	"$(INTDIR)\loadgame.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ENDIF 

SOURCE=.\abuse\src\menu.cpp

!IF  "$(CFG)" == "Abuse32 - Win32 Release"

CPP_SWITCHES=/nologo /ML /W3 /GX /O2 /Ob2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\menu.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "Abuse32 - Win32 Debug"

CPP_SWITCHES=/nologo /MLd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /FR"$(INTDIR)\\" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ /c 

"$(INTDIR)\menu.obj"	"$(INTDIR)\menu.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ENDIF 

SOURCE=.\abuse\src\morpher.cpp

!IF  "$(CFG)" == "Abuse32 - Win32 Release"

CPP_SWITCHES=/nologo /ML /W3 /GX /O2 /Ob2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\morpher.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "Abuse32 - Win32 Debug"

CPP_SWITCHES=/nologo /MLd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /FR"$(INTDIR)\\" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ /c 

"$(INTDIR)\morpher.obj"	"$(INTDIR)\morpher.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ENDIF 

SOURCE=.\abuse\src\objects.cpp

!IF  "$(CFG)" == "Abuse32 - Win32 Release"

CPP_SWITCHES=/nologo /ML /W3 /GX /O2 /Ob2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\objects.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "Abuse32 - Win32 Debug"

CPP_SWITCHES=/nologo /MLd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /FR"$(INTDIR)\\" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ /c 

"$(INTDIR)\objects.obj"	"$(INTDIR)\objects.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ENDIF 

SOURCE=.\abuse\src\parse.cpp

!IF  "$(CFG)" == "Abuse32 - Win32 Release"

CPP_SWITCHES=/nologo /ML /W3 /GX /O2 /Ob2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\parse.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "Abuse32 - Win32 Debug"

CPP_SWITCHES=/nologo /MLd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /FR"$(INTDIR)\\" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ /c 

"$(INTDIR)\parse.obj"	"$(INTDIR)\parse.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ENDIF 

SOURCE=.\abuse\src\particle.cpp

!IF  "$(CFG)" == "Abuse32 - Win32 Release"

CPP_SWITCHES=/nologo /ML /W3 /GX /O2 /Ob2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\particle.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "Abuse32 - Win32 Debug"

CPP_SWITCHES=/nologo /MLd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /FR"$(INTDIR)\\" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ /c 

"$(INTDIR)\particle.obj"	"$(INTDIR)\particle.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ENDIF 

SOURCE=.\abuse\src\points.cpp

!IF  "$(CFG)" == "Abuse32 - Win32 Release"

CPP_SWITCHES=/nologo /ML /W3 /GX /O2 /Ob2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\points.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "Abuse32 - Win32 Debug"

CPP_SWITCHES=/nologo /MLd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /FR"$(INTDIR)\\" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ /c 

"$(INTDIR)\points.obj"	"$(INTDIR)\points.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ENDIF 

SOURCE=.\abuse\src\profile.cpp

!IF  "$(CFG)" == "Abuse32 - Win32 Release"

CPP_SWITCHES=/nologo /ML /W3 /GX /O2 /Ob2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\profile.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "Abuse32 - Win32 Debug"

CPP_SWITCHES=/nologo /MLd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /FR"$(INTDIR)\\" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ /c 

"$(INTDIR)\profile.obj"	"$(INTDIR)\profile.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ENDIF 

SOURCE=.\abuse\src\property.cpp

!IF  "$(CFG)" == "Abuse32 - Win32 Release"

CPP_SWITCHES=/nologo /ML /W3 /GX /O2 /Ob2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\property.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "Abuse32 - Win32 Debug"

CPP_SWITCHES=/nologo /MLd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /FR"$(INTDIR)\\" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ /c 

"$(INTDIR)\property.obj"	"$(INTDIR)\property.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ENDIF 

SOURCE=.\abuse\src\sensor.cpp

!IF  "$(CFG)" == "Abuse32 - Win32 Release"

CPP_SWITCHES=/nologo /ML /W3 /GX /O2 /Ob2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\sensor.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "Abuse32 - Win32 Debug"

CPP_SWITCHES=/nologo /MLd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /FR"$(INTDIR)\\" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ /c 

"$(INTDIR)\sensor.obj"	"$(INTDIR)\sensor.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ENDIF 

SOURCE=.\abuse\src\seq.cpp

!IF  "$(CFG)" == "Abuse32 - Win32 Release"

CPP_SWITCHES=/nologo /ML /W3 /GX /O2 /Ob2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\seq.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "Abuse32 - Win32 Debug"

CPP_SWITCHES=/nologo /MLd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /FR"$(INTDIR)\\" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ /c 

"$(INTDIR)\seq.obj"	"$(INTDIR)\seq.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ENDIF 

SOURCE=.\abuse\src\smallfnt.cpp

!IF  "$(CFG)" == "Abuse32 - Win32 Release"

CPP_SWITCHES=/nologo /ML /W3 /GX /O2 /Ob2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\smallfnt.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "Abuse32 - Win32 Debug"

CPP_SWITCHES=/nologo /MLd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /FR"$(INTDIR)\\" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ /c 

"$(INTDIR)\smallfnt.obj"	"$(INTDIR)\smallfnt.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ENDIF 

SOURCE=.\abuse\src\specache.cpp

!IF  "$(CFG)" == "Abuse32 - Win32 Release"

CPP_SWITCHES=/nologo /ML /W3 /GX /O2 /Ob2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\specache.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "Abuse32 - Win32 Debug"

CPP_SWITCHES=/nologo /MLd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /FR"$(INTDIR)\\" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ /c 

"$(INTDIR)\specache.obj"	"$(INTDIR)\specache.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ENDIF 

SOURCE=.\abuse\src\statbar.cpp

!IF  "$(CFG)" == "Abuse32 - Win32 Release"

CPP_SWITCHES=/nologo /ML /W3 /GX /O2 /Ob2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\statbar.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "Abuse32 - Win32 Debug"

CPP_SWITCHES=/nologo /MLd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /FR"$(INTDIR)\\" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ /c 

"$(INTDIR)\statbar.obj"	"$(INTDIR)\statbar.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ENDIF 

SOURCE=.\abuse\src\text_gui.cpp

!IF  "$(CFG)" == "Abuse32 - Win32 Release"

CPP_SWITCHES=/nologo /ML /W3 /GX /O2 /Ob2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\text_gui.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "Abuse32 - Win32 Debug"

CPP_SWITCHES=/nologo /MLd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /FR"$(INTDIR)\\" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ /c 

"$(INTDIR)\text_gui.obj"	"$(INTDIR)\text_gui.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ENDIF 

SOURCE=.\abuse\src\transp.cpp

!IF  "$(CFG)" == "Abuse32 - Win32 Release"

CPP_SWITCHES=/nologo /ML /W3 /GX /O2 /Ob2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\transp.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "Abuse32 - Win32 Debug"

CPP_SWITCHES=/nologo /MLd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /FR"$(INTDIR)\\" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ /c 

"$(INTDIR)\transp.obj"	"$(INTDIR)\transp.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ENDIF 

SOURCE=.\abuse\src\trig.cpp

!IF  "$(CFG)" == "Abuse32 - Win32 Release"

CPP_SWITCHES=/nologo /ML /W3 /GX /O2 /Ob2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\trig.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "Abuse32 - Win32 Debug"

CPP_SWITCHES=/nologo /MLd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /FR"$(INTDIR)\\" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ /c 

"$(INTDIR)\trig.obj"	"$(INTDIR)\trig.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ENDIF 

SOURCE=.\abuse\src\username.cpp

!IF  "$(CFG)" == "Abuse32 - Win32 Release"

CPP_SWITCHES=/nologo /ML /W3 /GX /O2 /Ob2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\username.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "Abuse32 - Win32 Debug"

CPP_SWITCHES=/nologo /MLd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /FR"$(INTDIR)\\" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ /c 

"$(INTDIR)\username.obj"	"$(INTDIR)\username.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ENDIF 

SOURCE=.\abuse\src\version.cpp

!IF  "$(CFG)" == "Abuse32 - Win32 Release"

CPP_SWITCHES=/nologo /ML /W3 /GX /O2 /Ob2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\version.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "Abuse32 - Win32 Debug"

CPP_SWITCHES=/nologo /MLd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /FR"$(INTDIR)\\" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ /c 

"$(INTDIR)\version.obj"	"$(INTDIR)\version.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ENDIF 

SOURCE=.\abuse\src\view.cpp

!IF  "$(CFG)" == "Abuse32 - Win32 Release"

CPP_SWITCHES=/nologo /ML /W3 /GX /O2 /Ob2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\view.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "Abuse32 - Win32 Debug"

CPP_SWITCHES=/nologo /MLd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /FR"$(INTDIR)\\" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ /c 

"$(INTDIR)\view.obj"	"$(INTDIR)\view.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ENDIF 

SOURCE=.\Abuse32.cpp

!IF  "$(CFG)" == "Abuse32 - Win32 Release"

CPP_SWITCHES=/nologo /ML /W3 /GX /O2 /Ob2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\Abuse32.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "Abuse32 - Win32 Debug"

CPP_SWITCHES=/nologo /MLd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /FR"$(INTDIR)\\" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ /c 

"$(INTDIR)\Abuse32.obj"	"$(INTDIR)\Abuse32.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ENDIF 

SOURCE=.\Abuse32.rc

"$(INTDIR)\Abuse32.res" : $(SOURCE) "$(INTDIR)"
	$(RSC) $(RSC_PROJ) $(SOURCE)



!ENDIF 

