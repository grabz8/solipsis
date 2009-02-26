@echo off

rem Flash plugin
if exist C:\Program Files\Mozilla Firefox\plugins\NPSWF32.dll goto InMozilla_exist
if exist C:\WINDOWS\system32\Macromed\Flash\NPSWF32.dll goto InWinSys32MacromedFlash_exist
echo Plugin file NPSWF32.dll not found ! Check your Adobe Flash Player installation and run again this script ...
goto End

:InMozilla_exist
if exist ..\..\..\Common\bin\navigator\Debug\plugins copy "C:\Program Files\Mozilla Firefox\plugins\NPSWF32.dll" ..\..\..\Common\bin\navigator\Debug\plugins /y
if exist ..\..\..\Common\bin\navigator\Release\plugins copy "C:\Program Files\Mozilla Firefox\plugins\NPSWF32.dll" ..\..\..\Common\bin\navigator\Release\plugins /y
goto End

:InWinSys32MacromedFlash_exist
if exist ..\..\..\Common\bin\navigator\Debug\plugins copy "C:\WINDOWS\system32\Macromed\Flash\NPSWF32.dll" ..\..\..\Common\bin\navigator\Debug\plugins /y
if exist ..\..\..\Common\bin\navigator\Release\plugins copy "C:\WINDOWS\system32\Macromed\Flash\NPSWF32.dll" ..\..\..\Common\bin\navigator\Release\plugins /y
goto End

:End
pause
