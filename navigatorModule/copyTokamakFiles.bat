@echo off

if exist ..\Common\bin\navigator\Debug copy ..\Dependencies\tokamak\lib\debugdll\tokamakdll.dll ..\Common\bin\navigator\Debug /y

if exist ..\Common\bin\navigator\Release copy ..\Dependencies\tokamak\lib\releasedll\tokamakdll.dll ..\Common\bin\navigator\Release /y

pause
