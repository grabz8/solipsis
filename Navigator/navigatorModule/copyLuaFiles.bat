@echo off

if exist ..\Common\bin\navigator\Debug\lua xcopy ..\Media\lua\*.* ..\Common\bin\navigator\Debug\lua /s /y

if exist ..\Common\bin\navigator\Release\lua xcopy ..\Media\lua\*.* ..\Common\bin\navigator\Release\lua /s /y

pause
