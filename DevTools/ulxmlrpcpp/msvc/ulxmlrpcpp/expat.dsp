# Microsoft Developer Studio Project File - Name="expat" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** NICHT BEARBEITEN **

# TARGTYPE "Win32 (x86) Static Library" 0x0104

CFG=expat - Win32 Debug
!MESSAGE Dies ist kein gültiges Makefile. Zum Erstellen dieses Projekts mit NMAKE
!MESSAGE verwenden Sie den Befehl "Makefile exportieren" und führen Sie den Befehl
!MESSAGE 
!MESSAGE NMAKE /f "expat.mak".
!MESSAGE 
!MESSAGE Sie können beim Ausführen von NMAKE eine Konfiguration angeben
!MESSAGE durch Definieren des Makros CFG in der Befehlszeile. Zum Beispiel:
!MESSAGE 
!MESSAGE NMAKE /f "expat.mak" CFG="expat - Win32 Debug"
!MESSAGE 
!MESSAGE Für die Konfiguration stehen zur Auswahl:
!MESSAGE 
!MESSAGE "expat - Win32 Release" (basierend auf  "Win32 (x86) Static Library")
!MESSAGE "expat - Win32 Debug" (basierend auf  "Win32 (x86) Static Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "expat - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "expat___Win32_Release"
# PROP BASE Intermediate_Dir "expat___Win32_Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "Release"
# PROP Target_Dir ""
LINK32=link.exe
MTL=midl.exe
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /YX /FD /c
# ADD CPP /nologo /MD /W3 /GX /O2 /D "NDEBUG" /D "WIN32" /D "COMPILED_FROM_DSP" /D "_MBCS" /D "_LIB" /D "_STATIC" /FD /c
# SUBTRACT CPP /YX
# ADD BASE RSC /l 0x1009 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo /out:"Release\libexpat.lib"

!ELSEIF  "$(CFG)" == "expat - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "expat___Win32_Debug"
# PROP BASE Intermediate_Dir "expat___Win32_Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 2
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug"
# PROP Target_Dir ""
LINK32=link.exe
MTL=midl.exe
# ADD BASE CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /YX /FD /GZ /c
# ADD CPP /nologo /MDd /W3 /GX /ZI /Od /D "_DEBUG" /D "_STATIC" /D "WIN32" /D "COMPILED_FROM_DSP" /D "_MBCS" /D "_LIB" /D "BUILDING_DLL" /FR /FD /GZ /c
# SUBTRACT CPP /YX
# ADD BASE RSC /l 0x1009 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo /out:"Debug\libexpat.lib"

!ENDIF 

# Begin Target

# Name "expat - Win32 Release"
# Name "expat - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=..\..\expat\xmlparse.c
# End Source File
# Begin Source File

SOURCE=..\..\expat\xmlrole.c
# End Source File
# Begin Source File

SOURCE=..\..\expat\xmltok.c
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=..\..\expat\ascii.h
# End Source File
# Begin Source File

SOURCE=..\..\expat\asciitab.h
# End Source File
# Begin Source File

SOURCE=..\..\expat\expat.h
# End Source File
# Begin Source File

SOURCE=..\..\expat\iasciitab.h
# End Source File
# Begin Source File

SOURCE=..\..\expat\internal.h
# End Source File
# Begin Source File

SOURCE=..\..\expat\latin1tab.h
# End Source File
# Begin Source File

SOURCE=..\..\expat\nametab.h
# End Source File
# Begin Source File

SOURCE=..\..\expat\utf8tab.h
# End Source File
# Begin Source File

SOURCE=..\..\expat\winconfig.h
# End Source File
# Begin Source File

SOURCE=..\..\expat\xmlrole.h
# End Source File
# Begin Source File

SOURCE=..\..\expat\xmltok.h
# End Source File
# Begin Source File

SOURCE=..\..\expat\xmltok_impl.h
# End Source File
# End Group
# End Target
# End Project
