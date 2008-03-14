# Microsoft Developer Studio Project File - Name="Satan Paint" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Application" 0x0101

CFG=Satan Paint - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "Satan Paint.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "Satan Paint.mak" CFG="Satan Paint - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "Satan Paint - Win32 Release" (based on "Win32 (x86) Application")
!MESSAGE "Satan Paint - Win32 Debug" (based on "Win32 (x86) Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "Satan Paint - Win32 Release"

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
# ADD CPP /nologo /W3 /GX /O2 /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D "_MBCS" /D "_SPAINT" /FD /c
# SUBTRACT CPP /YX /Yc /Yu
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /machine:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib ddraw.lib dinput.lib  dsound.lib dplay.lib dxguid.lib /nologo /subsystem:windows /machine:I386

!ELSEIF  "$(CFG)" == "Satan Paint - Win32 Debug"

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
# ADD CPP /nologo /W3 /Gm /Gi /GX /ZI /Od /D "_DEBUG" /D "WIN32" /D "_WINDOWS" /D "_MBCS" /D "_SPAINT" /FR /FD /GZ /c
# SUBTRACT CPP /YX /Yc /Yu
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /I /win32 "" C:\My Documents\My Source\Satan Paint\spaint\Include""
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /debug /machine:I386 /pdbtype:sept
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib ddraw.lib dinput.lib dsound.lib dplay.lib dxguid.lib /nologo /subsystem:windows /debug /machine:I386 /pdbtype:sept

!ENDIF 

# Begin Target

# Name "Satan Paint - Win32 Release"
# Name "Satan Paint - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Group "Satan Paint Source"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\spaint\24bit.cpp
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=.\spaint\action.cpp
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=.\spaint\area.cpp
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=.\spaint\command2.cpp
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=.\spaint\commands.cpp
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=.\spaint\debug.cpp
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=.\spaint\edit.cpp
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=.\spaint\fnt6x13.cpp
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=.\spaint\fnt8x8.cpp
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=.\spaint\icon.cpp
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=.\spaint\items.cpp
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=.\spaint\letters.cpp
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=.\spaint\loader.cpp
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=.\spaint\logo.cpp
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=.\spaint\paint.cpp
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=.\spaint\pal.cpp
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=.\spaint\pcommand.cpp
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=.\spaint\plisp.cpp
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=.\spaint\points.cpp
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=.\spaint\sat.cpp
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=.\spaint\scale.cpp
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=.\spaint\smallfnt.cpp
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=.\spaint\toolinit.cpp
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=.\spaint\tools.cpp
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=.\spaint\undo.cpp
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=.\spaint\wildargv.cpp
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# End Group
# Begin Group "Unused Source"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\spaint\inp.cpp
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=.\spaint\menu.cpp
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=.\spaint\mkfont.cpp
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=.\spaint\otree.cpp
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=.\spaint\pretty.cpp
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=.\spaint\sadd.cpp
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=.\spaint\schop.cpp
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=.\spaint\sdel.cpp
# PROP Exclude_From_Build 1
# End Source File
# End Group
# Begin Group "IMLib"

# PROP Default_Filter ""
# Begin Group "Spaint imlib"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\imlib\include.cpp
# End Source File
# Begin Source File

SOURCE=.\imlib\lbmread.cpp
# End Source File
# Begin Source File

SOURCE=.\imlib\mdlimage.cpp
# End Source File
# Begin Source File

SOURCE=.\imlib\mdlread.cpp
# End Source File
# Begin Source File

SOURCE=.\imlib\ppmread.cpp
# End Source File
# Begin Source File

SOURCE=.\imlib\targa.cpp
# End Source File
# Begin Source File

SOURCE=.\imlib\texture.cpp
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=.\imlib\xwdread.cpp
# End Source File
# End Group
# Begin Source File

SOURCE="D:\My Documents\My Source\Abuse32\imlib\decoder.cpp"
# End Source File
# Begin Source File

SOURCE="D:\My Documents\My Source\Abuse32\imlib\dprint.cpp"
# End Source File
# Begin Source File

SOURCE="D:\My Documents\My Source\Abuse32\imlib\filesel.cpp"
# End Source File
# Begin Source File

SOURCE="D:\My Documents\My Source\Abuse32\imlib\filter.cpp"
# End Source File
# Begin Source File

SOURCE=.\imlib\fnt6x13.cpp
# End Source File
# Begin Source File

SOURCE="D:\My Documents\My Source\Abuse32\imlib\fonts.cpp"
# End Source File
# Begin Source File

SOURCE="D:\My Documents\My Source\Abuse32\imlib\globals.cpp"
# End Source File
# Begin Source File

SOURCE="D:\My Documents\My Source\Abuse32\imlib\glread.cpp"
# End Source File
# Begin Source File

SOURCE="D:\My Documents\My Source\Abuse32\imlib\guistat.cpp"
# End Source File
# Begin Source File

SOURCE="D:\My Documents\My Source\Abuse32\imlib\image.cpp"
# End Source File
# Begin Source File

SOURCE="D:\My Documents\My Source\Abuse32\imlib\image24.cpp"
# End Source File
# Begin Source File

SOURCE="D:\My Documents\My Source\Abuse32\imlib\input.cpp"
# End Source File
# Begin Source File

SOURCE="D:\My Documents\My Source\Abuse32\imlib\jmalloc.cpp"
# End Source File
# Begin Source File

SOURCE="D:\My Documents\My Source\Abuse32\imlib\jrand.cpp"
# End Source File
# Begin Source File

SOURCE="D:\My Documents\My Source\Abuse32\imlib\jwindow.cpp"
# End Source File
# Begin Source File

SOURCE="D:\My Documents\My Source\Abuse32\imlib\keys.cpp"
# End Source File
# Begin Source File

SOURCE="D:\My Documents\My Source\Abuse32\imlib\linked.cpp"
# End Source File
# Begin Source File

SOURCE="D:\My Documents\My Source\Abuse32\imlib\morph.cpp"
# End Source File
# Begin Source File

SOURCE="D:\My Documents\My Source\Abuse32\imlib\packet.cpp"
# End Source File
# Begin Source File

SOURCE="D:\My Documents\My Source\Abuse32\imlib\palette.cpp"
# End Source File
# Begin Source File

SOURCE="D:\My Documents\My Source\Abuse32\imlib\pcxread.cpp"
# End Source File
# Begin Source File

SOURCE="D:\My Documents\My Source\Abuse32\imlib\pmenu.cpp"
# End Source File
# Begin Source File

SOURCE="D:\My Documents\My Source\Abuse32\imlib\readwav.cpp"
# End Source File
# Begin Source File

SOURCE="D:\My Documents\My Source\Abuse32\imlib\scroller.cpp"
# End Source File
# Begin Source File

SOURCE="D:\My Documents\My Source\Abuse32\imlib\specs.cpp"
# End Source File
# Begin Source File

SOURCE="D:\My Documents\My Source\Abuse32\imlib\sprite.cpp"
# End Source File
# Begin Source File

SOURCE="D:\My Documents\My Source\Abuse32\imlib\status.cpp"
# End Source File
# Begin Source File

SOURCE="D:\My Documents\My Source\Abuse32\imlib\supmorph.cpp"
# End Source File
# Begin Source File

SOURCE="D:\My Documents\My Source\Abuse32\imlib\timage.cpp"
# End Source File
# Begin Source File

SOURCE="D:\My Documents\My Source\Abuse32\imlib\visobj.cpp"
# End Source File
# End Group
# Begin Group "Port"

# PROP Default_Filter ""
# Begin Source File

SOURCE="D:\My Documents\My Source\Abuse32\Win32src\event.cpp"
# End Source File
# Begin Source File

SOURCE="D:\My Documents\My Source\Abuse32\Win32src\jdir.cpp"
# End Source File
# Begin Source File

SOURCE="D:\My Documents\My Source\Abuse32\Win32src\joystick.cpp"
# End Source File
# Begin Source File

SOURCE="D:\My Documents\My Source\Abuse32\Win32src\macs.cpp"
# End Source File
# Begin Source File

SOURCE="D:\My Documents\My Source\Abuse32\Win32src\mouse.cpp"
# End Source File
# Begin Source File

SOURCE="D:\My Documents\My Source\Abuse32\Win32src\sock.cpp"
# End Source File
# Begin Source File

SOURCE="D:\My Documents\My Source\Abuse32\Win32src\timing.cpp"
# End Source File
# Begin Source File

SOURCE="D:\My Documents\My Source\Abuse32\Win32src\video.cpp"
# End Source File
# End Group
# Begin Source File

SOURCE=".\Satan Paint.cpp"
# End Source File
# Begin Source File

SOURCE=".\Satan Paint.rc"
# End Source File
# Begin Source File

SOURCE=.\StdAfx.cpp
# ADD CPP /Yc"stdafx.h"
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Group "Satain Paint Headers"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\spaint\Include\24bit.h
# End Source File
# Begin Source File

SOURCE=.\spaint\Include\action.h
# End Source File
# Begin Source File

SOURCE=.\spaint\Include\area.h
# End Source File
# Begin Source File

SOURCE=.\spaint\Include\debug.h
# End Source File
# Begin Source File

SOURCE=.\spaint\Include\edit.h
# End Source File
# Begin Source File

SOURCE=.\spaint\Include\id.h
# End Source File
# Begin Source File

SOURCE=.\spaint\Include\items.h
# End Source File
# Begin Source File

SOURCE=.\spaint\Include\logo.h
# End Source File
# Begin Source File

SOURCE=.\spaint\Include\otree.h
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=.\spaint\Include\paint.h
# End Source File
# Begin Source File

SOURCE=.\spaint\Include\pal.h
# End Source File
# Begin Source File

SOURCE=.\spaint\Include\pcommand.h
# End Source File
# Begin Source File

SOURCE=.\spaint\Include\points.h
# End Source File
# Begin Source File

SOURCE=.\spaint\Include\sat.h
# End Source File
# Begin Source File

SOURCE=.\spaint\Include\scale.h
# End Source File
# Begin Source File

SOURCE=.\spaint\Include\t.h
# End Source File
# Begin Source File

SOURCE=.\spaint\Include\ticons.h
# End Source File
# Begin Source File

SOURCE=.\spaint\Include\toolinit.h
# End Source File
# Begin Source File

SOURCE=.\spaint\Include\undo.h
# End Source File
# Begin Source File

SOURCE=.\spaint\Include\zoomer.h
# End Source File
# End Group
# Begin Source File

SOURCE=.\resource.h
# End Source File
# Begin Source File

SOURCE=".\Satan Paint.h"
# End Source File
# Begin Source File

SOURCE=.\StdAfx.h
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# Begin Source File

SOURCE=".\Satan Paint.ico"
# End Source File
# Begin Source File

SOURCE=.\small.ico
# End Source File
# End Group
# Begin Source File

SOURCE=.\ReadMe.txt
# End Source File
# End Target
# End Project
