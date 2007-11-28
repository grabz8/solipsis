@echo off

rem Flash plugin
if exist ..\Common\bin\navigator\Debug\plugins copy "C:\Program Files\Mozilla Firefox\plugins\NPSWF32.dll" ..\Common\bin\navigator\Debug\plugins /y

rem Flash plugin
if exist ..\Common\bin\navigator\Release\plugins copy "C:\Program Files\Mozilla Firefox\plugins\NPSWF32.dll" ..\Common\bin\navigator\Release\plugins /y

pause
