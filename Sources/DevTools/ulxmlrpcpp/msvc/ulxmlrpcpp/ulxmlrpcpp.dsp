# Microsoft Developer Studio Project File - Name="ulxmlrpcpp" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** NICHT BEARBEITEN **

# TARGTYPE "Win32 (x86) Static Library" 0x0104

CFG=ulxmlrpcpp - Win32 Debug
!MESSAGE Dies ist kein gültiges Makefile. Zum Erstellen dieses Projekts mit NMAKE
!MESSAGE verwenden Sie den Befehl "Makefile exportieren" und führen Sie den Befehl
!MESSAGE 
!MESSAGE NMAKE /f "ulxmlrpcpp.mak".
!MESSAGE 
!MESSAGE Sie können beim Ausführen von NMAKE eine Konfiguration angeben
!MESSAGE durch Definieren des Makros CFG in der Befehlszeile. Zum Beispiel:
!MESSAGE 
!MESSAGE NMAKE /f "ulxmlrpcpp.mak" CFG="ulxmlrpcpp - Win32 Debug"
!MESSAGE 
!MESSAGE Für die Konfiguration stehen zur Auswahl:
!MESSAGE 
!MESSAGE "ulxmlrpcpp - Win32 Release" (basierend auf  "Win32 (x86) Static Library")
!MESSAGE "ulxmlrpcpp - Win32 Debug" (basierend auf  "Win32 (x86) Static Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "ulxmlrpcpp - Win32 Release"

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
LINK32=link.exe
# ADD BASE LINK32 /machine:IX86
# ADD LINK32 /machine:IX86
MTL=midl.exe
# ADD BASE CPP /nologo /MD /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /YX /FD /c
# ADD CPP /nologo /MD /W3 /GX /O2 /I "../.." /I "../../ulxmlrpcpp" /I "../../expat" /D "NDEBUG" /D "WIN32" /D "_MBCS" /D "_LIB" /D "__NO_WSTRING__" /D "_STATIC" /YX /FD /c
# ADD BASE RSC /l 0x1009 /d "NDEBUG"
# ADD RSC /l 0x407 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /machine:I386
# ADD LIB32 wsock32.lib /nologo

!ELSEIF  "$(CFG)" == "ulxmlrpcpp - Win32 Debug"

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
LINK32=link.exe
# ADD BASE LINK32 /machine:IX86
# ADD LINK32 /machine:IX86
MTL=midl.exe
# ADD BASE CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /YX /FD /GZ /c
# ADD CPP /nologo /MDd /W2 /Gm /GX /ZI /Od /I "../.." /I "../../ulxmlrpcpp" /I "../../expat" /D "_DEBUG" /D "WIN32" /D "_MBCS" /D "_LIB" /D "__NO_WSTRING__" /YX /FD /GZ /c
# ADD BASE RSC /l 0x1009 /d "_DEBUG"
# ADD RSC /l 0x407 /i "../../../expat" /i "../../ulxmlrpcpp" /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /debug /machine:I386 /pdbtype:sept
# ADD LIB32 wsock32.lib /nologo

!ENDIF 

# Begin Target

# Name "ulxmlrpcpp - Win32 Release"
# Name "ulxmlrpcpp - Win32 Debug"
# Begin Group "Quellcodedateien"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=..\..\ulxmlrpcpp\ulxmlrpcpp.cpp
# End Source File
# Begin Source File

SOURCE=..\..\ulxmlrpcpp\ulxr_cached_resource.cpp
# End Source File
# Begin Source File

SOURCE=..\..\ulxmlrpcpp\ulxr_call.cpp
# End Source File
# Begin Source File

SOURCE=..\..\ulxmlrpcpp\ulxr_callparse.cpp
# End Source File
# Begin Source File

SOURCE=..\..\ulxmlrpcpp\ulxr_callparse_base.cpp
# End Source File
# Begin Source File

SOURCE=..\..\ulxmlrpcpp\ulxr_callparse_wb.cpp
# End Source File
# Begin Source File

SOURCE=..\..\ulxmlrpcpp\ulxr_connection.cpp
# End Source File
# Begin Source File

SOURCE=..\..\ulxmlrpcpp\ulxr_dispatcher.cpp
# End Source File
# Begin Source File

SOURCE=..\..\ulxmlrpcpp\ulxr_except.cpp
# End Source File
# Begin Source File

SOURCE=..\..\ulxmlrpcpp\ulxr_expatwrap.cpp
# End Source File
# Begin Source File

SOURCE=..\..\ulxmlrpcpp\ulxr_file_resource.cpp
# End Source File
# Begin Source File

SOURCE=..\..\ulxmlrpcpp\ulxr_htmlform_handler.cpp
# End Source File
# Begin Source File

SOURCE=..\..\ulxmlrpcpp\ulxr_http_client.cpp
# End Source File
# Begin Source File

SOURCE=..\..\ulxmlrpcpp\ulxr_http_protocol.cpp
# End Source File
# Begin Source File

SOURCE=..\..\ulxmlrpcpp\ulxr_http_server.cpp
# End Source File
# Begin Source File

SOURCE=..\..\ulxmlrpcpp\ulxr_mtrpc_server.cpp
# End Source File
# Begin Source File

SOURCE=..\..\ulxmlrpcpp\ulxr_mutex.cpp
# End Source File
# Begin Source File

SOURCE=..\..\ulxmlrpcpp\ulxr_protocol.cpp
# End Source File
# Begin Source File

SOURCE=..\..\ulxmlrpcpp\ulxr_requester.cpp
# End Source File
# Begin Source File

SOURCE=..\..\ulxmlrpcpp\ulxr_response.cpp
# End Source File
# Begin Source File

SOURCE=..\..\ulxmlrpcpp\ulxr_responseparse.cpp
# End Source File
# Begin Source File

SOURCE=..\..\ulxmlrpcpp\ulxr_responseparse_base.cpp
# End Source File
# Begin Source File

SOURCE=..\..\ulxmlrpcpp\ulxr_responseparse_wb.cpp
# End Source File
# Begin Source File

SOURCE=..\..\ulxmlrpcpp\ulxr_signature.cpp
# End Source File
# Begin Source File

SOURCE=..\..\ulxmlrpcpp\ulxr_ssl_connection.cpp
# End Source File
# Begin Source File

SOURCE=..\..\ulxmlrpcpp\ulxr_tcpip_connection.cpp
# End Source File
# Begin Source File

SOURCE=..\..\ulxmlrpcpp\ulxr_value.cpp
# End Source File
# Begin Source File

SOURCE=..\..\ulxmlrpcpp\ulxr_valueparse.cpp
# End Source File
# Begin Source File

SOURCE=..\..\ulxmlrpcpp\ulxr_valueparse_base.cpp
# End Source File
# Begin Source File

SOURCE=..\..\ulxmlrpcpp\ulxr_valueparse_wb.cpp
# End Source File
# Begin Source File

SOURCE=..\..\ulxmlrpcpp\ulxr_virtual_resource.cpp
# End Source File
# Begin Source File

SOURCE=..\..\ulxmlrpcpp\ulxr_wbxmlparse.cpp
# End Source File
# Begin Source File

SOURCE=..\..\ulxmlrpcpp\ulxr_xmlparse.cpp
# End Source File
# Begin Source File

SOURCE=..\..\ulxmlrpcpp\ulxr_xmlparse_base.cpp
# End Source File
# End Group
# Begin Group "Header-Dateien"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=..\..\ulxmlrpcpp\ulxmlrpcpp.h
# End Source File
# Begin Source File

SOURCE=..\..\ulxmlrpcpp\ulxr_cached_resource.h
# End Source File
# Begin Source File

SOURCE=..\..\ulxmlrpcpp\ulxr_call.h
# End Source File
# Begin Source File

SOURCE=..\..\ulxmlrpcpp\ulxr_callparse.h
# End Source File
# Begin Source File

SOURCE=..\..\ulxmlrpcpp\ulxr_callparse_base.h
# End Source File
# Begin Source File

SOURCE=..\..\ulxmlrpcpp\ulxr_callparse_wb.h
# End Source File
# Begin Source File

SOURCE="..\..\ulxmlrpcpp\ulxr_config-msvc.h"
# End Source File
# Begin Source File

SOURCE=..\..\ulxmlrpcpp\ulxr_connection.h
# End Source File
# Begin Source File

SOURCE=..\..\ulxmlrpcpp\ulxr_dispatcher.h
# End Source File
# Begin Source File

SOURCE=..\..\ulxmlrpcpp\ulxr_except.h
# End Source File
# Begin Source File

SOURCE=..\..\ulxmlrpcpp\ulxr_expatwrap.h
# End Source File
# Begin Source File

SOURCE=..\..\ulxmlrpcpp\ulxr_file_resource.h
# End Source File
# Begin Source File

SOURCE=..\..\ulxmlrpcpp\ulxr_htmlform_handler.h
# End Source File
# Begin Source File

SOURCE=..\..\ulxmlrpcpp\ulxr_http_base.h
# End Source File
# Begin Source File

SOURCE=..\..\ulxmlrpcpp\ulxr_http_client.h
# End Source File
# Begin Source File

SOURCE=..\..\ulxmlrpcpp\ulxr_http_protocol.h
# End Source File
# Begin Source File

SOURCE=..\..\ulxmlrpcpp\ulxr_http_server.h
# End Source File
# Begin Source File

SOURCE=..\..\ulxmlrpcpp\ulxr_mtrpc_server.h
# End Source File
# Begin Source File

SOURCE=..\..\ulxmlrpcpp\ulxr_mutex.h
# End Source File
# Begin Source File

SOURCE=..\..\ulxmlrpcpp\ulxr_protocol.h
# End Source File
# Begin Source File

SOURCE=..\..\ulxmlrpcpp\ulxr_requester.h
# End Source File
# Begin Source File

SOURCE=..\..\ulxmlrpcpp\ulxr_response.h
# End Source File
# Begin Source File

SOURCE=..\..\ulxmlrpcpp\ulxr_responseparse.h
# End Source File
# Begin Source File

SOURCE=..\..\ulxmlrpcpp\ulxr_responseparse_base.h
# End Source File
# Begin Source File

SOURCE=..\..\ulxmlrpcpp\ulxr_responseparse_wb.h
# End Source File
# Begin Source File

SOURCE=..\..\ulxmlrpcpp\ulxr_signature.h
# End Source File
# Begin Source File

SOURCE=..\..\ulxmlrpcpp\ulxr_ssl_connection.h
# End Source File
# Begin Source File

SOURCE=..\..\ulxmlrpcpp\ulxr_tcpip_connection.h
# End Source File
# Begin Source File

SOURCE=..\..\ulxmlrpcpp\ulxr_value.h
# End Source File
# Begin Source File

SOURCE=..\..\ulxmlrpcpp\ulxr_valueparse.h
# End Source File
# Begin Source File

SOURCE=..\..\ulxmlrpcpp\ulxr_valueparse_base.h
# End Source File
# Begin Source File

SOURCE=..\..\ulxmlrpcpp\ulxr_valueparse_bw.h
# End Source File
# Begin Source File

SOURCE=..\..\ulxmlrpcpp\ulxr_valueparse_wb.h
# End Source File
# Begin Source File

SOURCE=..\..\ulxmlrpcpp\ulxr_virtual_resource.h
# End Source File
# Begin Source File

SOURCE=..\..\ulxmlrpcpp\ulxr_wbxmlparse.h
# End Source File
# Begin Source File

SOURCE=..\..\ulxmlrpcpp\ulxr_xmlparse.h
# End Source File
# Begin Source File

SOURCE=..\..\ulxmlrpcpp\ulxr_xmlparse_base.h
# End Source File
# Begin Source File

SOURCE=..\..\ulxmlrpcpp\ulxr_xmlparse_wb.h
# End Source File
# End Group
# Begin Group "Ressourcendateien"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# End Group
# Begin Source File

SOURCE=Debug\libexpat.lib
# End Source File
# End Target
# End Project
