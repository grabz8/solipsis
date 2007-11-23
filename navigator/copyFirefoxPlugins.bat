@echo off

rem Flash plugin
if exist ..\Common\bin\navigator\Debug copy "C:\Program Files\Mozilla Firefox\plugins\NPSWF32.dll" ..\Common\bin\navigator\Debug /y

rem Flash plugin
if exist ..\Common\bin\navigator\Release copy "C:\Program Files\Mozilla Firefox\plugins\NPSWF32.dll" ..\Common\bin\navigator\Release /y

pause
