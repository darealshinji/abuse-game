# Microsoft Developer Studio Project File - Name="Abuse32" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Application" 0x0101

CFG=Abuse32 - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "Abuse32.mak".
!MESSAGE 
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

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "Abuse32 - Win32 Release"

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
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /Yu"stdafx.h" /FD /c
# ADD CPP /nologo /G5 /W3 /GX /O2 /Ob2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /FD /c
# SUBTRACT CPP /YX /Yc /Yu
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /I "Win32src" /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /machine:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib dxguid.lib ddraw.lib dsound.lib dinput.lib /nologo /subsystem:windows /machine:I386

!ELSEIF  "$(CFG)" == "Abuse32 - Win32 Debug"

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
# ADD BASE CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /Yu"stdafx.h" /FD /GZ /c
# ADD CPP /nologo /W3 /Gm /Gi /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /FR"C:/Compile/Debug/" /FD /GZ /c
# SUBTRACT CPP /YX /Yc /Yu
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /I "C:\My Documents\My Source\Abuse32\Win32src\\" /D "_DEBUG" /mktyplib203 /win32
# SUBTRACT MTL /Oicf
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /debug /machine:I386 /pdbtype:sept
# ADD LINK32 dinput.lib dxguid.lib ddraw.lib dsound.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /profile /map /debug /machine:I386
# SUBTRACT LINK32 /verbose

!ENDIF 

# Begin Target

# Name "Abuse32 - Win32 Release"
# Name "Abuse32 - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Group "Library"

# PROP Default_Filter ""
# Begin Group "Win32"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\Win32src\dp_prot.cpp
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=.\Win32src\event.cpp
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=.\Win32src\jdir.cpp
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=.\Win32src\joystick.cpp
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=.\Win32src\macs.cpp
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=.\Win32src\mouse.cpp
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=.\Win32src\sock.cpp
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=.\Win32src\sound.cpp
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=.\Win32src\timing.cpp
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=.\Win32src\video.cpp
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=.\Win32src\wildargv.cpp
# End Source File
# End Group
# Begin Group "Lib Video"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\imlib\decoder.cpp
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=.\imlib\filter.cpp
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=.\imlib\fonts.cpp
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=.\imlib\glread.cpp
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=.\imlib\image.cpp
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=.\imlib\image24.cpp
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=.\imlib\palette.cpp
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=.\imlib\pcxread.cpp
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=.\imlib\sprite.cpp
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=.\imlib\supmorph.cpp
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=.\imlib\timage.cpp
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=.\imlib\visobj.cpp
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# End Group
# Begin Group "file I/O"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\imlib\specs.cpp
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# End Group
# Begin Group "jWindow"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\imlib\filesel.cpp
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=.\imlib\input.cpp
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=.\imlib\jwindow.cpp
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=.\imlib\pmenu.cpp
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=.\imlib\scroller.cpp
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=.\imlib\tools.cpp
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# End Group
# Begin Group "General Purpose"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\imlib\jmalloc.cpp
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=.\imlib\jrand.cpp
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=.\imlib\linked.cpp
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# End Group
# Begin Source File

SOURCE=.\imlib\dprint.cpp
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=.\imlib\globals.cpp
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=.\imlib\guistat.cpp
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=.\imlib\keys.cpp
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=.\imlib\readwav.cpp
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=.\imlib\status.cpp
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# End Group
# Begin Group "Game"

# PROP Default_Filter ""
# Begin Group "net"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\abuse\src\net\unix\fileman.cpp
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=.\abuse\src\net\unix\gclient.cpp
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=.\abuse\src\net\unix\gserver.cpp
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=.\abuse\src\innet.cpp
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=.\abuse\src\netcfg.cpp
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=.\abuse\src\nfclient.cpp
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# End Group
# Begin Group "Lisp"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\abuse\src\clisp.cpp
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=.\abuse\src\compiled.cpp
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=.\abuse\src\language.cpp
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=.\abuse\src\lisp.cpp
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=.\abuse\src\lisp_gc.cpp
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=.\abuse\src\lisp_opt.cpp
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=.\abuse\src\parse.cpp
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# End Group
# Begin Group "cache"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\abuse\src\cache.cpp
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=.\abuse\src\lcache.cpp
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# End Group
# Begin Group "Game Video"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\abuse\src\director.cpp
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=.\abuse\src\fnt6x13.cpp
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=.\abuse\src\light.cpp
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=.\abuse\src\morpher.cpp
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=.\abuse\src\smallfnt.cpp
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=.\abuse\src\statbar.cpp
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# End Group
# Begin Group "Game jWindow"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\abuse\src\automap.cpp
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=.\abuse\src\chat.cpp
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=.\abuse\src\console.cpp
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=.\abuse\src\devsel.cpp
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=.\abuse\src\gamma.cpp
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=.\abuse\src\gui.cpp
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=.\abuse\src\loadgame.cpp
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=.\abuse\src\menu.cpp
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# End Group
# Begin Group "Game Objects"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\abuse\src\ability.cpp
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=.\abuse\src\ant.cpp
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=.\abuse\src\chars.cpp
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=.\abuse\src\cop.cpp
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=.\abuse\src\extend.cpp
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=.\abuse\src\level.cpp
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=.\abuse\src\objects.cpp
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=.\abuse\src\particle.cpp
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=.\abuse\src\property.cpp
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=.\abuse\src\sensor.cpp
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=.\abuse\src\view.cpp
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# End Group
# Begin Source File

SOURCE=.\abuse\src\collide.cpp
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=.\abuse\src\config.cpp
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=.\abuse\src\crc.cpp
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=.\abuse\src\demo.cpp
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=.\abuse\src\dev.cpp
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=.\abuse\src\endgame.cpp
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=.\abuse\src\game.cpp
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=.\abuse\src\help.cpp
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=.\abuse\src\intsect.cpp
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=.\abuse\src\items.cpp
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=.\abuse\src\letters.cpp
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=.\abuse\src\loader2.cpp
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=.\abuse\src\points.cpp
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=.\abuse\src\profile.cpp
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=.\abuse\src\seq.cpp
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=.\abuse\src\specache.cpp
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=.\abuse\src\trig.cpp
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=.\abuse\src\username.cpp
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=.\abuse\src\version.cpp
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# End Group
# Begin Source File

SOURCE=.\Abuse32.cpp
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=.\Abuse32.rc
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Group "Library Headers"

# PROP Default_Filter ""
# Begin Group "Win32 Headers"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\Win32src\dp_prot.h
# End Source File
# Begin Source File

SOURCE=.\Win32src\sock.h
# End Source File
# Begin Source File

SOURCE=.\Win32src\sound.hpp
# End Source File
# Begin Source File

SOURCE=.\Win32src\video.h
# End Source File
# End Group
# Begin Source File

SOURCE=.\imlib\include\bitmap.h
# End Source File
# Begin Source File

SOURCE=.\imlib\include\conio.h
# End Source File
# Begin Source File

SOURCE=.\imlib\include\dir.h
# End Source File
# Begin Source File

SOURCE=.\imlib\include\dprint.hpp
# End Source File
# Begin Source File

SOURCE=.\imlib\include\errs.h
# End Source File
# Begin Source File

SOURCE=.\imlib\include\event.hpp
# End Source File
# Begin Source File

SOURCE=.\imlib\include\exitproc.hpp
# End Source File
# Begin Source File

SOURCE=.\imlib\include\filesel.hpp
# End Source File
# Begin Source File

SOURCE=.\imlib\include\filter.hpp
# End Source File
# Begin Source File

SOURCE=.\imlib\include\fonts.hpp
# End Source File
# Begin Source File

SOURCE=.\imlib\include\gifdecod.hpp
# End Source File
# Begin Source File

SOURCE=.\imlib\include\globals.hpp
# End Source File
# Begin Source File

SOURCE=.\imlib\include\glread.hpp
# End Source File
# Begin Source File

SOURCE=.\imlib\include\guistat.hpp
# End Source File
# Begin Source File

SOURCE=.\imlib\include\image.hpp
# End Source File
# Begin Source File

SOURCE=.\imlib\include\image24.hpp
# End Source File
# Begin Source File

SOURCE=.\imlib\include\include.hpp
# End Source File
# Begin Source File

SOURCE=.\imlib\include\input.hpp
# End Source File
# Begin Source File

SOURCE=.\imlib\include\jdir.hpp
# End Source File
# Begin Source File

SOURCE=.\imlib\include\jmalloc.hpp
# End Source File
# Begin Source File

SOURCE=.\imlib\include\jnet.hpp
# End Source File
# Begin Source File

SOURCE=.\imlib\include\joy.hpp
# End Source File
# Begin Source File

SOURCE=.\imlib\include\jrand.hpp
# End Source File
# Begin Source File

SOURCE=.\imlib\include\jwindow.hpp
# End Source File
# Begin Source File

SOURCE=.\imlib\include\keys.hpp
# End Source File
# Begin Source File

SOURCE=.\imlib\include\linked.hpp
# End Source File
# Begin Source File

SOURCE=.\imlib\include\macs.hpp
# End Source File
# Begin Source File

SOURCE=.\imlib\include\main.hpp
# End Source File
# Begin Source File

SOURCE=.\imlib\include\morph.h
# End Source File
# Begin Source File

SOURCE=.\imlib\include\morph.hpp
# End Source File
# Begin Source File

SOURCE=.\imlib\include\mouse.hpp
# End Source File
# Begin Source File

SOURCE=.\imlib\include\palette.hpp
# End Source File
# Begin Source File

SOURCE=.\imlib\include\pcxread.hpp
# End Source File
# Begin Source File

SOURCE=.\imlib\include\pmenu.hpp
# End Source File
# Begin Source File

SOURCE=.\imlib\include\readwav.hpp
# End Source File
# Begin Source File

SOURCE=.\imlib\include\scroller.hpp
# End Source File
# Begin Source File

SOURCE=.\imlib\include\specs.hpp
# End Source File
# Begin Source File

SOURCE=.\imlib\include\sprite.hpp
# End Source File
# Begin Source File

SOURCE=.\imlib\include\status.hpp
# End Source File
# Begin Source File

SOURCE=.\imlib\include\std.h
# End Source File
# Begin Source File

SOURCE=.\imlib\include\supmorph.hpp
# End Source File
# Begin Source File

SOURCE=.\imlib\include\system.h
# End Source File
# Begin Source File

SOURCE=.\imlib\include\timage.hpp
# End Source File
# Begin Source File

SOURCE=.\imlib\include\timing.hpp
# End Source File
# Begin Source File

SOURCE=.\imlib\include\tools.hpp
# End Source File
# Begin Source File

SOURCE=.\imlib\include\visobj.hpp
# End Source File
# Begin Source File

SOURCE=.\imlib\include\xinclude.h
# End Source File
# End Group
# Begin Group "Game Headers"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\abuse\inc\ability.hpp
# End Source File
# Begin Source File

SOURCE=.\abuse\inc\ant.hpp
# End Source File
# Begin Source File

SOURCE=.\abuse\inc\automap.hpp
# End Source File
# Begin Source File

SOURCE=.\abuse\inc\bus_type.hpp
# End Source File
# Begin Source File

SOURCE=.\abuse\inc\cache.hpp
# End Source File
# Begin Source File

SOURCE=.\abuse\inc\chars.hpp
# End Source File
# Begin Source File

SOURCE=.\abuse\inc\chat.hpp
# End Source File
# Begin Source File

SOURCE=.\abuse\inc\chmorph.hpp
# End Source File
# Begin Source File

SOURCE=.\abuse\inc\clisp.hpp
# End Source File
# Begin Source File

SOURCE=.\abuse\inc\compiled.hpp
# End Source File
# Begin Source File

SOURCE=.\abuse\inc\config.hpp
# End Source File
# Begin Source File

SOURCE=.\abuse\inc\console.hpp
# End Source File
# Begin Source File

SOURCE=.\abuse\inc\control.hpp
# End Source File
# Begin Source File

SOURCE=.\abuse\inc\cop.hpp
# End Source File
# Begin Source File

SOURCE=.\abuse\inc\crc.hpp
# End Source File
# Begin Source File

SOURCE=.\abuse\inc\demo.hpp
# End Source File
# Begin Source File

SOURCE=.\abuse\inc\dev.hpp
# End Source File
# Begin Source File

SOURCE=.\abuse\inc\devsel.hpp
# End Source File
# Begin Source File

SOURCE=.\abuse\inc\director.hpp
# End Source File
# Begin Source File

SOURCE=.\abuse\inc\extend.hpp
# End Source File
# Begin Source File

SOURCE=.\abuse\inc\fakelib.hpp
# End Source File
# Begin Source File

SOURCE=.\abuse\inc\game.hpp
# End Source File
# Begin Source File

SOURCE=.\abuse\inc\gamma.hpp
# End Source File
# Begin Source File

SOURCE=.\abuse\inc\go.hpp
# End Source File
# Begin Source File

SOURCE=.\abuse\inc\gui.hpp
# End Source File
# Begin Source File

SOURCE=.\abuse\inc\help.hpp
# End Source File
# Begin Source File

SOURCE=.\abuse\inc\human.hpp
# End Source File
# Begin Source File

SOURCE=.\abuse\inc\id.hpp
# End Source File
# Begin Source File

SOURCE=.\abuse\inc\intsect.hpp
# End Source File
# Begin Source File

SOURCE=.\abuse\inc\isllist.hpp
# End Source File
# Begin Source File

SOURCE=.\abuse\inc\isllist_bak.hpp
# End Source File
# Begin Source File

SOURCE=.\abuse\inc\items.hpp
# End Source File
# Begin Source File

SOURCE=.\abuse\inc\language.hpp
# End Source File
# Begin Source File

SOURCE=.\abuse\inc\laser.hpp
# End Source File
# Begin Source File

SOURCE=.\abuse\inc\lcache.hpp
# End Source File
# Begin Source File

SOURCE=.\abuse\inc\level.hpp
# End Source File
# Begin Source File

SOURCE=.\abuse\inc\light.hpp
# End Source File
# Begin Source File

SOURCE=.\abuse\inc\lisp.hpp
# End Source File
# Begin Source File

SOURCE=.\abuse\inc\lisp2.hpp
# End Source File
# Begin Source File

SOURCE=.\abuse\inc\lisp_gc.hpp
# End Source File
# Begin Source File

SOURCE=.\abuse\inc\lisp_opt.hpp
# End Source File
# Begin Source File

SOURCE=.\abuse\inc\loader2.hpp
# End Source File
# Begin Source File

SOURCE=.\abuse\inc\loadgame.hpp
# End Source File
# Begin Source File

SOURCE=.\abuse\inc\menu.hpp
# End Source File
# Begin Source File

SOURCE=.\abuse\inc\morpher.hpp
# End Source File
# Begin Source File

SOURCE=.\abuse\inc\netcfg.hpp
# End Source File
# Begin Source File

SOURCE=.\abuse\inc\netface.hpp
# End Source File
# Begin Source File

SOURCE=.\abuse\inc\nfclient.hpp
# End Source File
# Begin Source File

SOURCE=.\abuse\inc\nfserver.hpp
# End Source File
# Begin Source File

SOURCE=.\abuse\inc\objects.hpp
# End Source File
# Begin Source File

SOURCE=.\abuse\inc\parse.hpp
# End Source File
# Begin Source File

SOURCE=.\abuse\inc\particle.hpp
# End Source File
# Begin Source File

SOURCE=.\abuse\inc\points.hpp
# End Source File
# Begin Source File

SOURCE=.\abuse\inc\profile.hpp
# End Source File
# Begin Source File

SOURCE=.\abuse\inc\property.hpp
# End Source File
# Begin Source File

SOURCE=.\abuse\inc\ramfile.hpp
# End Source File
# Begin Source File

SOURCE=.\abuse\inc\sbar.hpp
# End Source File
# Begin Source File

SOURCE=.\abuse\inc\scene.hpp
# End Source File
# Begin Source File

SOURCE=.\abuse\inc\seq.hpp
# End Source File
# Begin Source File

SOURCE=.\abuse\inc\server2.hpp
# End Source File
# Begin Source File

SOURCE=.\abuse\inc\specache.hpp
# End Source File
# Begin Source File

SOURCE=.\abuse\inc\stack.hpp
# End Source File
# Begin Source File

SOURCE=.\abuse\inc\tiles.hpp
# End Source File
# Begin Source File

SOURCE=.\abuse\inc\view.hpp
# End Source File
# End Group
# Begin Group "Unix Port Headers"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\abuse\src\net\unix\endian.hpp
# End Source File
# Begin Source File

SOURCE=.\abuse\src\net\unix\engine.hpp
# End Source File
# Begin Source File

SOURCE=.\abuse\src\net\unix\fileman.hpp
# End Source File
# Begin Source File

SOURCE=.\abuse\src\net\unix\gclient.hpp
# End Source File
# Begin Source File

SOURCE=.\abuse\src\net\unix\ghandler.hpp
# End Source File
# Begin Source File

SOURCE=.\abuse\src\net\unix\gserver.hpp
# End Source File
# Begin Source File

SOURCE=.\abuse\src\net\unix\netdrv.hpp
# End Source File
# Begin Source File

SOURCE=.\abuse\src\net\unix\netfile.hpp
# End Source File
# Begin Source File

SOURCE=.\abuse\src\net\unix\tcpip.hpp
# End Source File
# Begin Source File

SOURCE=.\abuse\src\net\unix\undrv.hpp
# End Source File
# End Group
# Begin Source File

SOURCE=.\Abuse32.h
# End Source File
# Begin Source File

SOURCE=.\abuse\src\net\inc\ipx.hpp
# End Source File
# Begin Source File

SOURCE=.\resource.h
# End Source File
# Begin Source File

SOURCE=.\StdAfx.h
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# Begin Source File

SOURCE=.\Abuse32.ico
# End Source File
# Begin Source File

SOURCE=.\Debug\ICON.ICO
# End Source File
# Begin Source File

SOURCE=.\ICON.ICO
# End Source File
# Begin Source File

SOURCE=.\small.ico
# End Source File
# End Group
# Begin Group "LISP files"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\Debug\ABUSE.LSP
# End Source File
# Begin Source File

SOURCE=.\LISP\CHAT.LSP
# End Source File
# Begin Source File

SOURCE=.\LISP\objects.lsp
# End Source File
# Begin Source File

SOURCE=.\LISP\PEOPLE.LSP
# End Source File
# Begin Source File

SOURCE=.\LISP\Persist.lsp
# End Source File
# End Group
# Begin Source File

SOURCE=.\preload.lsp
# End Source File
# Begin Source File

SOURCE=.\ReadMe.txt
# End Source File
# Begin Source File

SOURCE=.\scratch.txt
# End Source File
# Begin Source File

SOURCE=.\ToDo.txt
# End Source File
# Begin Source File

SOURCE=".\What's New.txt"
# End Source File
# End Target
# End Project
